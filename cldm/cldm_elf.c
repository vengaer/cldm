#include "cldm_rtassert.h"
#include "cldm_elf.h"
#include "cldm_log.h"
#include "cldm_macro.h"

#include "cldm_ntbs.h"

#include <errno.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static inline void cldm_elf_map_section_fail(struct cldm_elfmap const *map) {
    if(!map->strtab.addr) {
        cldm_err("No .strtab found in binary");
    }
    if(!map->dynstr.addr) {
        cldm_err("No .dynstr found in binary");
    }
}

/* Get nth section header */
void *cldm_elf_shdr(struct cldm_elfmap const *map, unsigned n);

/* Get address of section header string table */
static char const *cldm_elf_shstrtab(struct cldm_elfmap const *map) {
    Elf64_Shdr shdr;

    if(map->m_un.ehdr->e_shstrndx == SHN_UNDEF) {
        return 0;
    }

    memcpy(&shdr, cldm_elf_shdr(map, map->m_un.ehdr->e_shstrndx), sizeof(shdr));
    return (char const *)map->m_un.addr + shdr.sh_offset;
}

static inline void cldm_map_strtab(struct cldm_strtab *restrict strtab, size_t mapaddr, void *restrict hdr) {
    Elf64_Shdr shdr;
    memcpy(&shdr, hdr, sizeof(shdr));
    strtab->addr = (char const *)mapaddr + shdr.sh_offset;
    strtab->size = shdr.sh_size - 1;
}

/* Map .strtab and .dynstr sections */
static int cldm_elf_map_sections(struct cldm_elfmap *map) {
    Elf64_Shdr shdr;
    void *addr;
    void *strtab_hdr = 0;
    void *dynstr_hdr = 0;

    for(Elf64_Half i = 0; i < map->m_un.ehdr->e_shnum; i++) {
        addr = cldm_elf_shdr(map, i);
        memcpy(&shdr, addr, sizeof(shdr));

        if(shdr.sh_type == SHT_STRTAB) {
            if(strcmp(map->shstrtab + shdr.sh_name, ".strtab") == 0) {
                strtab_hdr = addr;
            }
            else if(strcmp(map->shstrtab + shdr.sh_name, ".dynstr") == 0) {
                dynstr_hdr = addr;
            }
        }
    }

    if(strtab_hdr) {
        cldm_map_strtab(&map->strtab, (size_t)map->m_un.addr, strtab_hdr);
    }

    if(dynstr_hdr) {
        cldm_map_strtab(&map->dynstr, (size_t)map->m_un.addr, dynstr_hdr);
    }

    return !!strtab_hdr + !!dynstr_hdr;
}

/* Find section based with given type and name */
static void *cldm_elf_section(struct cldm_elfmap const *restrict map, Elf64_Word type, char const *restrict secname) {
    Elf64_Shdr shdr;
    void *addr;

    for(Elf64_Half i = 0; i < map->m_un.ehdr->e_shnum; i++) {
        addr = cldm_elf_shdr(map, i);
        memcpy(&shdr, addr, sizeof(shdr));
        if(shdr.sh_type == type && strcmp(map->shstrtab + shdr.sh_name, secname) == 0) {
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
    Elf64_Shdr shdr;
    Elf64_Sym sym;
    void *addr;

    for(Elf64_Half i = 0; i < map->m_un.ehdr->e_shnum; i++) {
        memcpy(&shdr, cldm_elf_shdr(map, i), sizeof(shdr));
        if(shdr.sh_type == SHT_SYMTAB) {
            for(Elf64_Xword j = 0; j < shdr.sh_size; j += sizeof(sym)) {
                addr = (unsigned char *)map->m_un.addr + shdr.sh_offset + j;
                memcpy(&sym, addr, sizeof(sym));
                if(sym.st_name != STN_UNDEF && strcmp(symbol, map->strtab.addr + sym.st_name) == 0) {
                    return addr;
                }
            }
        }
    }

    return 0;
}

static void *cldm_elf_baseaddr(struct cldm_elfmap const *map) {
    extern int main(void);

    Elf64_Phdr phdr;
    Elf64_Addr baseaddr;
    Elf64_Shdr shdr;
    Elf64_Sym  sym;
    void *relmain;
    void *phaddr;

    baseaddr = ~(Elf64_Addr)0;

    for(Elf64_Half i = 0; i < map->m_un.ehdr->e_phnum; i++) {
        phaddr = (void *)((unsigned char const *)map->m_un.addr + map->m_un.ehdr->e_phoff + i * map->m_un.ehdr->e_phentsize);

        memcpy(&phdr, phaddr, sizeof(phdr));

        /* min(baseaddr, phdr.p_vaddr) */
        baseaddr = baseaddr ^ ((phdr.p_vaddr ^ baseaddr) & -(phdr.p_vaddr < baseaddr));
    }

    relmain = cldm_elf_sym(map, "main");
    memcpy(&sym, relmain, sizeof(sym));

    switch(sym.st_shndx) {
        case SHN_UNDEF:
            cldm_err("Cannot lookup external main");
            return 0;
        case SHN_ABS:
            cldm_err("Failed to resolve base address of executable");
            return 0;
        default:
            /* nop */
            break;
    }

    memcpy(&shdr, cldm_elf_shdr(map, sym.st_shndx), sizeof(shdr));

    return (void *)((size_t)main - sym.st_value - baseaddr);
}

/* Get address of where symbol is loaded */
static size_t cldm_elf_symaddr(struct cldm_elfmap const *restrict map, Elf64_Sym const *restrict sym) {
    Elf64_Shdr shdr;
    switch(sym->st_shndx) {
        case SHN_UNDEF:
            cldm_err("External test symbols not supported");
            return 0;
        case SHN_ABS:
            return sym->st_value;
        default:
            /* nop */
            break;
    }

    memcpy(&shdr, cldm_elf_shdr(map, sym->st_shndx), sizeof(shdr));
    return (size_t)map->baseaddr + sym->st_value;
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
        .m_un.addr = addr,
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
    if(munmap(map->m_un.addr, map->size) == -1) {
        cldm_err("munmap: %s", strerror(errno));
        return -1;
    }

    return 0;
}

bool cldm_is_elf64(struct cldm_elfmap const *map) {
    return map->m_un.ehdr->e_ident[EI_MAG0]  == ELFMAG0 &&
           map->m_un.ehdr->e_ident[EI_MAG1]  == ELFMAG1 &&
           map->m_un.ehdr->e_ident[EI_MAG2]  == ELFMAG2 &&
           map->m_un.ehdr->e_ident[EI_MAG3]  == ELFMAG3 &&
           map->m_un.ehdr->e_ident[EI_CLASS] == ELFCLASS64;
}

bool cldm_elf_is_executable(struct cldm_elfmap const *map) {
    return map->m_un.ehdr->e_type == ET_EXEC ||
           map->m_un.ehdr->e_type == ET_DYN;
}

ssize_t cldm_elf_read_strtab(struct cldm_elfmap const *restrict map, char *restrict buffer, char const *restrict section, size_t bufsize) {
    Elf64_Shdr shdr;
    void *strtab;

    if(strcmp(section, ".strtab") == 0) {
        if(bufsize < map->strtab.size) {
            return -E2BIG;
        }
        memcpy(buffer, map->strtab.addr, map->strtab.size);
        return map->strtab.size;
    }
    else if(strcmp(section, ".dynstr") == 0) {
        if(bufsize < map->dynstr.size) {
            return -E2BIG;
        }
        memcpy(buffer, map->dynstr.addr, map->dynstr.size);
        return map->dynstr.size;
    }

    strtab = cldm_elf_strtab(map, section);
    if(!strtab) {
        return -1;
    }

    memcpy(&shdr, strtab, sizeof(shdr));

    if(bufsize < shdr.sh_size) {
        return -E2BIG;
    }

    memcpy(buffer, (unsigned char *)map->m_un.addr + shdr.sh_offset + 1, shdr.sh_size - 1);
    return shdr.sh_size - 1;
}

ssize_t cldm_elf_read_needed(struct cldm_elfmap const *restrict map, char *restrict buffer, size_t bufsize) {
    Elf64_Shdr shdr;
    Elf64_Dyn dyn;
    void *dynamic;
    ssize_t nbytes;
    ssize_t offset;

    dynamic = cldm_elf_dynamic(map);
    if(!dynamic) {
        return -1;
    }

    memcpy(&shdr, dynamic, sizeof(shdr));

    offset = 0;
    for(Elf64_Xword i = 0; i < shdr.sh_size; i += sizeof(dyn)) {
        memcpy(&dyn, (unsigned char *)map->m_un.addr + shdr.sh_offset + i, sizeof(dyn));
        if(dyn.d_tag == DT_NEEDED) {
            nbytes = cldm_strscpy(buffer + offset, map->dynstr.addr + dyn.d_un.d_val, bufsize - offset);
            if(nbytes < 0 || (size_t)(offset + nbytes + 1) >= bufsize) {
                cldm_err("Adding needed library %s to the list would cause overflow", map->dynstr.addr + dyn.d_un.d_val);
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
        return 0;
    }

    memcpy(&sym, symaddr, sizeof(sym));
    return (void (*)(void))cldm_elf_symaddr(map, &sym);
}

void *cldm_elf_testrec(struct cldm_elfmap const *restrict map, char const *restrict record) {
    void *symaddr;
    Elf64_Sym sym;

    symaddr = cldm_elf_sym(map, record);
    if(!symaddr) {
        return 0;
    }

    memcpy(&sym, symaddr, sizeof(sym));
    return (void *)cldm_elf_symaddr(map, &sym);
}
