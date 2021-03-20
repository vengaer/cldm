#ifndef CLDM_ELF_H
#define CLDM_ELF_H

#include <stdbool.h>
#include <stddef.h>

#include <elf.h>
#include <sys/types.h>

struct cldm_strtab {
    char const *addr;
    size_t size;
};

struct cldm_elfmap {
    size_t size;
    union {
        void *addr;
        Elf64_Ehdr *ehdr;
    } m_un;
    char const *shstrtab;
    struct cldm_strtab strtab;
    struct cldm_strtab dynstr;
    /* Address where executable is loaded */
    void *baseaddr;
};

/* Get nth section header */
inline void *cldm_elf_shdr(struct cldm_elfmap const *map, unsigned n) {
    return (unsigned char *)map->m_un.addr + map->m_un.ehdr->e_shoff + n * map->m_un.ehdr->e_shentsize;
}

int cldm_map_elf(struct cldm_elfmap *restrict map, char const *restrict file);
int cldm_unmap_elf(struct cldm_elfmap *map);

bool cldm_is_elf64(struct cldm_elfmap const *map);

ssize_t cldm_elf_read_strtab(struct cldm_elfmap const *restrict map, char *restrict buffer, char const *restrict section, size_t bufsize);
ssize_t cldm_elf_read_needed(struct cldm_elfmap const *restrict map, char *restrict buffer, size_t bufsize);

/* Get pointer to given function */
void (*cldm_elf_func(struct cldm_elfmap const *restrict map, char const *restrict func))(void);

#endif /* CLDM_ELF_H */
