#ifndef CLDM_ELF_H
#define CLDM_ELF_H

#include <stdbool.h>
#include <stddef.h>

#include <sys/types.h>

struct cldm_strtab {
    char const *addr;
    size_t size;
};

struct cldm_elfmap {
    size_t size;
    void *addr;
    char const *shstrtab;
    struct cldm_strtab strtab;
    struct cldm_strtab dynstr;
    /* Address where executable is loaded */
    void *baseaddr;
};

int cldm_map_elf(struct cldm_elfmap *restrict map, char const *restrict file);
int cldm_unmap_elf(struct cldm_elfmap *map);

bool cldm_is_elf64(struct cldm_elfmap const *map);

ssize_t cldm_elf_read_strtab(struct cldm_elfmap const *restrict map, char *restrict buffer, char const *restrict section, size_t bufsize);
ssize_t cldm_elf_read_needed(struct cldm_elfmap const *restrict map, char *restrict buffer, size_t bufsize);

/* Get pointer to given function */
void (*cldm_elf_func(struct cldm_elfmap const *restrict map, char const *restrict func))(void);

#endif /* CLDM_ELF_H */
