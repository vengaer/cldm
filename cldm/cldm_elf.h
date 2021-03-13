#ifndef CLDM_ELF_H
#define CLDM_ELF_H

#include <stdbool.h>
#include <stddef.h>

#include <sys/types.h>

struct cldm_elfmap {
    void *addr;
    size_t size;
};

int cldm_map_elf(struct cldm_elfmap *restrict map, char const *restrict file);
int cldm_unmap_elf(struct cldm_elfmap *map);

bool cldm_is_elf64(struct cldm_elfmap const *map);
ssize_t cldm_get_strtab(struct cldm_elfmap const *restrict map, char *restrict buffer, size_t bufsize);
ssize_t cldm_dump_strtab(struct cldm_elfmap const *map);

#endif /* CLDM_ELF_H */
