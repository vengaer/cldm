#include "cldm_elfdump.h"
#include "cldm_limits.h"
#include "cldm_log.h"
#include "cldm_ntbs.h"

#include <elf.h>
#include <string.h>

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


int cldm_elfdump_strtab(struct cldm_elfmap const *restrict map, char const *restrict section) {
    char buffer[CLDM_PGSIZE];
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

int cldm_elfdump_needed(struct cldm_elfmap const *map) {
    char buffer[CLDM_PGSIZE];
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

void cldm_elfdump_sections(struct cldm_elfmap const *map) {
    Elf64_Shdr shdr;

    cldm_log("sections:");
    for(Elf64_Half i = 0; i < map->m_un.ehdr->e_shnum; i++) {
        memcpy(&shdr, cldm_elf_shdr(map, i), sizeof(shdr));

        cldm_log("name: %-20s type: %-20s offset: %#lx", map->shstrtab + shdr.sh_name, cldm_elf_section_type(shdr.sh_type), shdr.sh_offset);
    }
}
