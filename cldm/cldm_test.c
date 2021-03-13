#include "cldm_elf.h"
#include "cldm_log.h"
#include "cldm_mem.h"
#include "cldm_ntbs.h"
#include "cldm_test.h"


ssize_t cldm_test_collect(char *restrict buffer, char const *restrict file, size_t bufsize) {
    ssize_t status = -1;
    ssize_t ntests = 0;
    ssize_t strtab_size;
    ssize_t symsize;
    struct cldm_elfmap map = { 0 };
    char strtab[CLDM_PAGE_SIZE];
    char *addr = buffer;

    if(cldm_map_elf(&map, file)) {
        cldm_err("Mapping %s failed\n", file);
        goto epilogue;
    }

    strtab_size = cldm_read_strtab(&map, strtab, sizeof(strtab));
    if(strtab_size < 0) {
        cldm_err("Could not read strtab of %s", file);
        goto epilogue;
    }

    for(ssize_t i = 0; i < strtab_size; i += cldm_ntbslen(strtab + i) + 1) {
        if(cldm_ntbsncmp(cldm_str_expand(cldm_test_proc_prefix), strtab + i, sizeof(cldm_str_expand(cldm_test_proc_prefix)) - 1) == 0) {
            symsize = cldm_ntbscpy(addr, strtab + i, bufsize - (addr - buffer));
            if(symsize < 0) {
                cldm_err("Test %lld would cause internal buffer overflow", (long long)i);
                status = symsize;
                goto epilogue;
            }
            addr += symsize;
            *addr++ = ';';
            *addr = '\0';
            ++ntests;
        }
    }

    status = ntests;
epilogue:
    if(map.addr) {
        if(cldm_unmap_elf(&map)) {
            cldm_warn("Unmapping of %s failed", file);
        }
    }

    return status;
}
