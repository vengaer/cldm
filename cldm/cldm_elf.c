#include "cldm_rtassert.h"
#include "cldm_elf.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_mem.h"
#include "cldm_ntbs.h"

#include <errno.h>
#include <string.h>

#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static inline void cldm_elf_map_section_fail(struct cldm_elfmap const *map) {
    if(!map->strtab) {
        cldm_err("No .strtab found in binary");
    }
    if(!map->dynstr) {
        cldm_err("No .dynstr found in binary");
    }
}

/* Get nth section header */
static inline void *cldm_elf_shdr(struct cldm_elfmap const *map, unsigned n) {
    return (unsigned char *)map->addr + ((Elf64_Ehdr *)map->addr)->e_shoff + n * ((Elf64_Ehdr *)map->addr)->e_shentsize;
}

/* Get address of section header string table */
static char const *cldm_elf_shstrtab(struct cldm_elfmap const *map) {
    Elf64_Ehdr *ehdr;
    Elf64_Shdr shdr;

    ehdr = map->addr;
    if(ehdr->e_shstrndx == SHN_UNDEF) {
        return 0;
    }

    cldm_mcpy(&shdr, cldm_elf_shdr(map, ehdr->e_shstrndx), sizeof(shdr));
    return (char const *)map->addr + shdr.sh_offset;
}

/* Map .strtab and .dynstr sections */
static int cldm_elf_map_sections(struct cldm_elfmap *map) {
    Elf64_Ehdr *ehdr;
    Elf64_Shdr shdr;
    void **p;
    ehdr = map->addr;
    int nmapped;

    nmapped = 0;

    for(Elf64_Half i = 0; i < ehdr->e_shnum; i++) {
        cldm_mcpy(&shdr, cldm_elf_shdr(map, i), sizeof(shdr));
        p = 0;

        if(shdr.sh_type == SHT_STRTAB) {
            if(cldm_ntbscmp(map->shstrtab + shdr.sh_name, ".strtab") == 0) {
                p = &map->strtab;
            }
            else if(cldm_ntbscmp(map->shstrtab + shdr.sh_name, ".dynstr") == 0) {
                p = &map->dynstr;
            }
        }

        if(p) {
            *p = cldm_elf_shdr(map, i);
            ++nmapped;
        }
    }

    return nmapped;
}

/* Find section based with given type and name */
static void *cldm_elf_section(struct cldm_elfmap const *restrict map, Elf64_Word type, char const *restrict secname) {
    Elf64_Ehdr *ehdr;
    Elf64_Shdr shdr;
    void *addr;

    ehdr = map->addr;

    for(Elf64_Half i = 0; i < ehdr->e_shnum; i++) {
        addr = cldm_elf_shdr(map, i);
        cldm_mcpy(&shdr, addr, sizeof(shdr));
        if(shdr.sh_type == type && cldm_ntbscmp(map->shstrtab + shdr.sh_name, secname) == 0) {
            return addr;
        }
    }

    return 0;
}

/* Get strtab for given section */
static inline void *cldm_elf_strtab(struct cldm_elfmap const *restrict map, char const *restrict section) {
    return cldm_elf_section(map, SHT_STRTAB, section);
}

/* Get .dynamic section */
static inline void *cldm_elf_dynamic(struct cldm_elfmap const *map) {
    return cldm_elf_section(map, SHT_DYNAMIC, ".dynamic");
}

/* Get address of Elf64_Sym struct for the given name */
static void *cldm_elf_sym(struct cldm_elfmap const *restrict map, char const *restrict symbol) {
    Elf64_Ehdr *ehdr;
    Elf64_Shdr shdr;
    Elf64_Sym sym;
    void *addr;
    char const *strtab;
    ehdr = map->addr;

    cldm_mcpy(&shdr, map->strtab, sizeof(shdr));
    strtab = (char const *)map->addr + shdr.sh_offset;

    for(Elf64_Half i = 0; i < ehdr->e_shnum; i++) {
        cldm_mcpy(&shdr, cldm_elf_shdr(map, i), sizeof(shdr));
        if(shdr.sh_type == SHT_SYMTAB) {
            for(Elf64_Xword j = 0; j < shdr.sh_size; j += sizeof(sym)) {
                addr = (unsigned char *)map->addr + shdr.sh_offset + j;
                cldm_mcpy(&sym, addr, sizeof(sym));
                if(sym.st_name != STN_UNDEF && cldm_ntbscmp(symbol, strtab + sym.st_name) == 0) {
                    return addr;
                }
            }
        }
    }

    return 0;
}

/* Get address of where executable is loaded in memory */
static void *cldm_elf_baseaddr(struct cldm_elfmap const *map) {
    extern int main(void);
    Elf64_Sym sym;
    Elf64_Shdr shdr;
    void *relmain;
    size_t offset;

    relmain = cldm_elf_sym(map, "main");
    cldm_mcpy(&sym, relmain, sizeof(sym));
    switch(sym.st_shndx) {
        case SHN_UNDEF:
            cldm_err("Cannot lookup external main");
            return 0;
        case SHN_ABS:
            cldm_err("Failed to resolve base address of executable");
            return 0;
        default:
            // NOP
            break;
    }
    cldm_mcpy(&shdr, cldm_elf_shdr(map, sym.st_shndx), sizeof(shdr));

    offset = sym.st_value + shdr.sh_offset;
    return (void *)((size_t)main - offset);
}

/* Get address of where symbol is loaded */
static size_t cldm_elf_symaddr(struct cldm_elfmap const *restrict map, Elf64_Sym const *restrict sym) {
    Elf64_Shdr shdr;
    switch(sym->st_shndx) {
        case SHN_UNDEF:
            cldm_rtassert(0, "External symbols not supported");
            break;
        case SHN_ABS:
            return sym->st_value;
        default:
            // NOP
            break;
    }

    cldm_mcpy(&shdr, cldm_elf_shdr(map, sym->st_shndx), sizeof(shdr));
    return (size_t)map->baseaddr  + sym->st_value + shdr.sh_offset;
}

/* Memory map elf file */
int cldm_map_elf(struct cldm_elfmap *restrict map, char const *restrict file) {
    int fd;
    struct stat sb;
    void *addr;
    int status;
    int nmapped;

    status = -1;

    fd = open(file, O_RDONLY);

    if(fd == -1) {
        cldm_err("open %s: %s", file, strerror(errno));
        goto epilogue;
    }

    if(fstat(fd, &sb) == -1) {
        cldm_err("fstat %s: %s", file, strerror(errno));
        goto epilogue;
    }

    addr = mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if(addr == MAP_FAILED) {
        cldm_err("mmap %s: %s", file, strerror(errno));
        goto epilogue;
    }

    *map = (struct cldm_elfmap){
        .addr = addr,
        .size = sb.st_size
    };
    map->shstrtab = cldm_elf_shstrtab(map);

    if(!map->shstrtab) {
        cldm_err("Found no shstrtab in %s", file);
        cldm_unmap_elf(map);
        goto epilogue;
    }

    nmapped = cldm_elf_map_sections(map);
    if(nmapped != 2) {
        cldm_elf_map_section_fail(map);
        goto epilogue;
    }

    map->baseaddr = cldm_elf_baseaddr(map);
    if(!map->baseaddr) {
        goto epilogue;
    }

    status = 0;

epilogue:
    if(fd != -1) {
        close(fd);
    }

    return status;
}

int cldm_unmap_elf(struct cldm_elfmap *map) {
    if(munmap(map->addr, map->size) == -1) {
        cldm_err("munmap: %s", strerror(errno));
        return -1;
    }

    return 0;
}

bool cldm_is_elf64(struct cldm_elfmap const *map) {
    Elf64_Ehdr ehdr;
    cldm_mcpy(&ehdr, map->addr, sizeof(ehdr));

    return ehdr.e_ident[EI_MAG0] == ELFMAG0 &&
           ehdr.e_ident[EI_MAG1] == ELFMAG1 &&
           ehdr.e_ident[EI_MAG2] == ELFMAG2 &&
           ehdr.e_ident[EI_MAG3] == ELFMAG3 &&
           ehdr.e_ident[EI_CLASS] == ELFCLASS64;
}

ssize_t cldm_elf_read_strtab(struct cldm_elfmap const *restrict map, char *restrict buffer, char const *restrict section, size_t bufsize) {
    Elf64_Shdr shdr;
    void *strtab;

    if(cldm_ntbscmp(section, ".strtab") == 0) {
        strtab = map->strtab;
    }
    else if(cldm_ntbscmp(section, ".dynstr") == 0) {
        strtab = map->dynstr;
    }
    else {
        strtab = cldm_elf_strtab(map, section);
    }
    if(!strtab) {
        return -1;
    }

    cldm_mcpy(&shdr, strtab, sizeof(shdr));

    if(bufsize < shdr.sh_size) {
        return -E2BIG;
    }

    cldm_mcpy(buffer, (unsigned char *)map->addr + shdr.sh_offset + 1, shdr.sh_size - 1);
    return shdr.sh_size - 1;
}

ssize_t cldm_elf_read_needed(struct cldm_elfmap const *restrict map, char *restrict buffer, size_t bufsize) {
    Elf64_Shdr shdr;
    Elf64_Dyn dyn;
    void *dynamic;
    char const *dynstr;
    ssize_t nbytes;
    ssize_t offset;

    dynstr = map->dynstr;
    if(!dynstr) {
        return -1;
    }

    cldm_mcpy(&shdr, dynstr, sizeof(shdr));
    dynstr = (char const *)map->addr + shdr.sh_offset;

    dynamic = cldm_elf_dynamic(map);
    if(!dynamic) {
        return -1;
    }

    cldm_mcpy(&shdr, dynamic, sizeof(shdr));

    offset = 0;
    for(Elf64_Xword i = 0; i < shdr.sh_size; i += sizeof(dyn)) {
        cldm_mcpy(&dyn, (unsigned char *)map->addr + shdr.sh_offset + i, sizeof(dyn));
        if(dyn.d_tag == DT_NEEDED) {
            nbytes = cldm_ntbscpy(buffer + offset, dynstr + dyn.d_un.d_val, bufsize - offset);
            if(nbytes < 0 || (size_t)(offset + nbytes + 1) >= bufsize) {
                cldm_err("Adding needed library %s to the list would cause overflow", dynstr + dyn.d_un.d_val);
                return nbytes;
            }
            offset += nbytes + 1;
        }
    }

    return offset - 1;
}

void (*cldm_elf_func(struct cldm_elfmap const *restrict map, char const *restrict func))(void) {
    void *symaddr;
    Elf64_Sym sym;

    symaddr = cldm_elf_sym(map, func);
    if(!symaddr) {
        cldm_err("Could not locate address of %s in .text segment", func);
        return 0;
    }

    cldm_mcpy(&sym, symaddr, sizeof(sym));
    return (void (*)(void))cldm_elf_symaddr(map, &sym);
}
