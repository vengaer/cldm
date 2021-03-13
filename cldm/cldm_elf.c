#include "cldm_assert.h"
#include "cldm_elf.h"
#include "cldm_log.h"
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

int cldm_map_elf(struct cldm_elfmap *restrict map, char const *restrict file) {
    int fd;
    struct stat sb;
    void *addr;
    int status;

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

ssize_t cldm_read_strtab(struct cldm_elfmap const *restrict map, char *restrict buffer, size_t bufsize) {
    Elf64_Ehdr ehdr;
    Elf64_Shdr shdr;
    char const *shstrtab;

    cldm_mcpy(&ehdr, map->addr, sizeof(ehdr));
    if(ehdr.e_shstrndx == SHN_UNDEF) {
        return -1;
    }

    cldm_mcpy(&shdr, (char const *)map->addr + ehdr.e_shoff + ehdr.e_shstrndx * sizeof(shdr), sizeof(shdr));
    shstrtab = (char const *)map->addr + shdr.sh_offset;

    for(unsigned i = 0; i < ehdr.e_shnum; i++) {
        cldm_mcpy(&shdr, (unsigned char *)map->addr + ehdr.e_shoff + i * ehdr.e_shentsize, sizeof(shdr));

        if(shdr.sh_type == SHT_STRTAB && cldm_ntbscmp(shstrtab + shdr.sh_name, ".strtab") == 0) {
            if(bufsize < shdr.sh_size) {
                return -E2BIG;
            }
            cldm_mcpy(buffer, (char const *)map->addr + shdr.sh_offset + 1, shdr.sh_size - 1);
            return shdr.sh_size - 1;
        }
    }
    return -1;
}

ssize_t cldm_dump_strtab(struct cldm_elfmap const *map) {
    char buffer[CLDM_PAGE_SIZE];
    ssize_t nbytes;

    nbytes = cldm_read_strtab(map, buffer, sizeof(buffer));
    if(nbytes < 0) {
        return nbytes;
    }

    puts(".strtab:");
    for(int i = 0; i < nbytes; i += cldm_ntbslen(buffer + i) + 1) {
        printf("%s\n", buffer + i);
    }

    return 0;
}
