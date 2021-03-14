#ifndef CLDM_ELF_H
#define CLDM_ELF_H

#include <stdbool.h>
#include <stddef.h>

#include <sys/types.h>

struct cldm_elfmap {
    size_t size;
    void *addr;
    char const *shstrtab;
    /* Addresses of Elf64_Shdrs */
    void *strtab;
    void *dynstr;
    void *debug_info;
};

int cldm_map_elf(struct cldm_elfmap *restrict map, char const *restrict file);
int cldm_unmap_elf(struct cldm_elfmap *map);

bool cldm_is_elf64(struct cldm_elfmap const *map);

ssize_t cldm_elf_read_strtab(struct cldm_elfmap const *restrict map, char *restrict buffer, char const *restrict section, size_t bufsize);
ssize_t cldm_elf_read_needed(struct cldm_elfmap const *restrict map, char *restrict buffer, size_t bufsize);

int cldm_elf_dump_strtab(struct cldm_elfmap const *restrict map, char const *restrict section);
int cldm_elf_dump_needed(struct cldm_elfmap const *map);

void cldm_elf_dump_sections(struct cldm_elfmap const *map);

#endif /* CLDM_ELF_H */
