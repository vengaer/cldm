#include "cldm_dl.h"
#include "cldm_log.h"
#include "cldm_mem.h"
#include "cldm_ntbs.h"
#include "cldm_test.h"

#include <dlfcn.h>

static void cldm_test_empty_func(void) { }

#define cldm_load_stage(stage, scope)                                                               \
    (void) dlerror();                                                                               \
    *(void **)(&stage) = dlsym(handle, cldm_str_expand(cldm_ ## scope ## _ ## stage ## _ident));    \
    if(dlerror()) {                                                                                 \
        stage = cldm_test_empty_func;                                                               \
    }                                                                                               \
    else {                                                                                          \
        cldm_log("Detected " cldm_str_expand(scope) " " cldm_str_expand(stage));                    \
    }


static void (*cldm_test_local_setup(void *handle))(void) {
    void (*setup)(void) = { 0 };
    cldm_load_stage(setup, local);
    return setup;
}

static void (*cldm_test_local_teardown(void *handle))(void) {
    void (*teardown)(void) = { 0 };
    cldm_load_stage(teardown, local);
    return teardown;
}

static void (*cldm_test_global_setup(void *handle))(void) {
    void (*setup)(void) = { 0 };
    cldm_load_stage(setup, global);
    return setup;
}

static void (*cldm_test_global_teardown(void *handle))(void) {
    void (*teardown)(void) = { 0 };
    cldm_load_stage(teardown, global);
    return teardown;
}

ssize_t cldm_test_collect(char *restrict buffer, struct cldm_elfmap const *restrict map, size_t bufsize) {
    ssize_t ntests = 0;
    ssize_t strtab_size;
    ssize_t symsize;
    char strtab[CLDM_PAGE_SIZE];
    ssize_t offset;

    strtab_size = cldm_elf_read_strtab(map, strtab, ".strtab", sizeof(strtab));
    if(strtab_size < 0) {
        cldm_err("Could not read strtab");
        return -1;
    }

    offset = 0;
    for(ssize_t i = 0; i < strtab_size; i += cldm_ntbslen(strtab + i) + 1) {
        if(cldm_ntbsncmp(cldm_str_expand(cldm_test_proc_prefix), strtab + i, sizeof(cldm_str_expand(cldm_test_proc_prefix)) - 1) == 0) {
            symsize = cldm_ntbscpy(buffer + offset, strtab + i, bufsize - offset);
            if(symsize < 0 || (size_t)(offset + symsize + 1) >= bufsize) {
                cldm_err("Test %lld would cause internal buffer overflow", (long long)i);
                return -E2BIG;
            }
            offset += symsize;
            buffer[offset++] = ';';
            buffer[offset] = '\0';
            ++ntests;
        }
    }

    return ntests;
}

int cldm_test_invoke_each(char const *tests, size_t ntests) {
    void (*test)(void);
    void *handle;
    int status;
    char *iter;
    char const *err;
    unsigned testidx;

    void (*lcl_setup)(void);
    void (*lcl_teardown)(void);
    void (*glob_setup)(void);
    void (*glob_teardown)(void);

    if(!ntests) {
        return 0;
    }

    status = -1;

    handle = dlopen(0, RTLD_LAZY);
    if(!handle) {
        cldm_err("%s", dlerror());
        goto epilogue;
    }

    (void)dlerror();

    lcl_setup = cldm_test_local_setup(handle);
    lcl_teardown= cldm_test_local_teardown(handle);
    glob_setup = cldm_test_global_setup(handle);
    glob_teardown = cldm_test_global_teardown(handle);
    cldm_log("");

    glob_setup();

    testidx = 0;
    cldm_for_each_word(iter, tests, ';') {
        *(void **)(&test) = dlsym(handle, iter);

        err = dlerror();
        if(err) {
            cldm_err("%s", err);
            goto epilogue;
        }

        cldm_log("[Running %s] (%u/%zu)", iter + sizeof(cldm_str_expand(cldm_test_proc_prefix)) - 1, ++testidx, ntests);
        lcl_setup();
        test();
        lcl_teardown();
    }

    glob_teardown();

    status = 0;
epilogue:
    if(handle) {
        dlclose(handle);
    }

    return status;
}
