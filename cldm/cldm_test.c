#include "cldm_dl.h"
#include "cldm_elf.h"
#include "cldm_log.h"
#include "cldm_mem.h"
#include "cldm_ntbs.h"
#include "cldm_test.h"

#include <dlfcn.h>

static void cldm_empty_local_setup(void) { }
static void cldm_empty_local_teardown(void) { }

#define cldm_load_local_stage(stage)                                                        \
    (void) dlerror();                                                                       \
    *(void **)(&stage) = dlsym(handle, cldm_str_expand(cldm_local_ ## stage ## _ident));    \
    if(dlerror()) {                                                                         \
        stage = cldm_empty_local_ ## stage;                                                 \
    }                                                                                       \
    else {                                                                                  \
        cldm_log("Detected per-test " cldm_str_expand(stage));                              \
    }



static void (*cldm_test_local_setup(void *handle))(void) {
    void (*setup)(void) = { 0 };
    cldm_load_local_stage(setup);
    return setup;
}

static void (*cldm_test_local_teardown(void *handle))(void) {
    void (*teardown)(void) = { 0 };
    cldm_load_local_stage(teardown);
    return teardown;
}

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

    if(!cldm_is_elf64(&map)) {
        cldm_err("%s is not a 64-bit ELF binary", file);
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

int cldm_test_invoke_each(char const *tests, size_t ntests) {
    void (*test)(void);
    void *handle;
    int status;
    char *iter;
    char const *err;
    unsigned testidx;

    void(*setup)(void);
    void(*teardown)(void);

    status = -1;

    cldm_unload();

    handle = dlopen(0, RTLD_LAZY);
    if(!handle) {
        cldm_err("%s", dlerror());
        goto epilogue;
    }

    (void)dlerror();

    setup = cldm_test_local_setup(handle);
    teardown= cldm_test_local_teardown(handle);
    cldm_log("");

    testidx = 0;
    cldm_for_each_word(iter, tests, ';') {
        *(void **)(&test) = dlsym(handle, iter);

        err = dlerror();
        if(err) {
            cldm_err("%s", err);
            goto epilogue;
        }

        cldm_log("[Running %s] (%u/%zu)", iter + sizeof(cldm_str_expand(cldm_test_proc_prefix)) - 1, ++testidx, ntests);
        setup();
        test();
        teardown();
    }

epilogue:
    if(handle) {
        dlclose(handle);
    }

    cldm_preload();

    return status;
}
