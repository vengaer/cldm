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

static char const *cldm_elf_section_type(Elf64_Word type) {
    switch(type) {
        case SHT_NULL:
            return "SHT_NULL";
        case SHT_PROGBITS:
            return "SHT_PROGBITS";
        case SHT_SYMTAB:
            return "SHT_SYMTAB";
        case SHT_STRTAB:
            return "SHT_STRTAB";
        case SHT_RELA:
            return "SHT_RELA";
        case SHT_HASH:
            return "SHT_HASH";
        case SHT_DYNAMIC:
            return "SHT_DYNAMIC";
        case SHT_NOTE:
            return "SHT_NOTE";
        case SHT_NOBITS:
            return "SHT_NOBITS";
        case SHT_REL:
            return "SHT_REL";
        case SHT_SHLIB:
            return "SHT_SHLIB";
        case SHT_DYNSYM:
            return "SHT_DYNSYM";
        case SHT_LOPROC:
            return "SHT_LOPROC";
        case SHT_HIPROC:
            return "SHT_HIPROC";
        case SHT_LOUSER:
            return "SHT_LOUSER";
        case SHT_HIUSER:
            return "SHT_HIUSER";
        default:
            /* NOP */
            break;
    }
    return "Unknown";
}

static inline void cldm_elf_map_section_fail(struct cldm_elfmap const *map) {
    if(!map->strtab) {
        cldm_log("No .strtab found in binary");
    }
    if(!map->dynstr) {
        cldm_log("No .dynstr found in binary");
    }
}

static inline int cldm_elf_validate_section_map(struct cldm_elfmap const *map, int nmapped) {
    switch(nmapped) {
        case 0:
        case 1:
            cldm_elf_map_section_fail(map);
            return -1;
        case 2:
            /* Debug info not mandatory */
            if(map->debug_info) {
                cldm_elf_map_section_fail(map);
                return -1;
            }
            break;
        case 3:
            break;
        default:
            cldm_rtassert(0);
    }

    return 0;
}

static char const *cldm_elf_shstrtab(struct cldm_elfmap const *map) {
    Elf64_Ehdr *ehdr;
    Elf64_Shdr shdr;

    ehdr = map->addr;
    if(ehdr->e_shstrndx == SHN_UNDEF) {
        return 0;
    }

    cldm_mcpy(&shdr, (unsigned char *)map->addr + ehdr->e_shoff + ehdr->e_shstrndx * sizeof(shdr), sizeof(shdr));
    return (char const *)map->addr + shdr.sh_offset;
}

static int cldm_elf_map_sections(struct cldm_elfmap *map) {
    Elf64_Ehdr *ehdr;
    Elf64_Shdr shdr;
    void **p;
    ehdr = map->addr;
    int nmapped;

    map->strtab = 0;
    map->dynstr = 0;
    map->debug_info = 0;

    nmapped = 0;

    for(Elf64_Half i = 0; i < ehdr->e_shnum; i++) {
        cldm_mcpy(&shdr, (unsigned char *)map->addr + ehdr->e_shoff + i *ehdr->e_shentsize, sizeof(shdr));
        p = 0;

        if(shdr.sh_type == SHT_STRTAB) {
            if(cldm_ntbscmp(map->shstrtab + shdr.sh_name, ".strtab") == 0) {
                p = &map->strtab;
            }
            else if(cldm_ntbscmp(map->shstrtab + shdr.sh_name, ".dynstr") == 0) {
                p = &map->dynstr;
            }
        }
        else if(shdr.sh_type == SHT_PROGBITS && cldm_ntbscmp(map->shstrtab + shdr.sh_name, ".debug_info") == 0) {
            p = &map->debug_info;
        }

        if(p) {
            *p = (unsigned char *)map->addr + ehdr->e_shoff + i * ehdr->e_shentsize;
            ++nmapped;
        }
    }

    return nmapped;
}

static void *cldm_elf_section(struct cldm_elfmap const *restrict map, Elf64_Word type, char const *restrict secname) {
    Elf64_Ehdr *ehdr;
    Elf64_Shdr shdr;

    ehdr = map->addr;

    for(Elf64_Half i = 0; i < ehdr->e_shnum; i++) {
        cldm_mcpy(&shdr, (unsigned char *)map->addr + ehdr->e_shoff + i * ehdr->e_shentsize, sizeof(shdr));

        if(shdr.sh_type == type && cldm_ntbscmp(map->shstrtab + shdr.sh_name, secname) == 0) {
            return (unsigned char *)map->addr + ehdr->e_shoff + i * ehdr->e_shentsize;
        }
    }

    return 0;
}

static inline void *cldm_elf_strtab(struct cldm_elfmap const *restrict map, char const *restrict section) {
    return cldm_elf_section(map, SHT_STRTAB, section);
}

static inline void *cldm_elf_dynamic(struct cldm_elfmap const *map) {
    return cldm_elf_section(map, SHT_DYNAMIC, ".dynamic");
}

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

    map->addr = addr;
    map->size = sb.st_size;
    map->shstrtab = cldm_elf_shstrtab(map);

    if(!map->shstrtab) {
        cldm_err("Found no shstrtab in %s", file);
        cldm_unmap_elf(map);
        goto epilogue;
    }

    nmapped = cldm_elf_map_sections(map);
    if(cldm_elf_validate_section_map(map, nmapped)) {
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

int cldm_elf_dump_strtab(struct cldm_elfmap const *restrict map, char const *restrict section) {
    char buffer[CLDM_PAGE_SIZE];
    ssize_t nbytes;

    nbytes = cldm_elf_read_strtab(map, buffer, section, sizeof(buffer));
    if(nbytes < 0) {
        return nbytes;
    }

    cldm_log("%s", section);
    for(ssize_t i = 0; i < nbytes; i += cldm_ntbslen(buffer + i) + 1) {
        cldm_log("name: %-30s offset: %llu", buffer + i, (unsigned long long)i);
    }

    return 0;
}

int cldm_elf_dump_needed(struct cldm_elfmap const *map) {
    char buffer[CLDM_PAGE_SIZE];
    ssize_t nbytes;

    nbytes = cldm_elf_read_needed(map, buffer, sizeof(buffer));

    if(nbytes < 0) {
        return nbytes;
    }

    cldm_log("needed:");
    for(ssize_t i = 0; i < nbytes; i += cldm_ntbslen(buffer + i) + 1) {
        cldm_log("%s", buffer + i);
    }
    return 0;
}

void cldm_elf_dump_sections(struct cldm_elfmap const *map) {
    Elf64_Ehdr *ehdr;
    Elf64_Shdr shdr;

    ehdr = map->addr;

    for(Elf64_Half i = 0; i < ehdr->e_shnum; i++) {
        cldm_mcpy(&shdr, (unsigned char *)map->addr + ehdr->e_shoff + i * ehdr->e_shentsize, sizeof(shdr));

        cldm_log("name: %-20s type: %s", map->shstrtab + shdr.sh_name, cldm_elf_section_type(shdr.sh_type));
    }
}
