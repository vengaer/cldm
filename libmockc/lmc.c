#define LMC_GENERATE_SYMBOLS
#include "lmc.h"

static void *dlhandle;
struct lmc_mock_info *mockinfo;

int lmc_init1(char const *binary_path) {
    dlhandle = dlopen(binary_path, RTLD_LAZY);
    if(!dlhandle) {
        char const *err = dlerror();
        lmc_assert(!err, err);
    }
    return 0;
}

int lmc_init2(int argc, char *const *argv) {
    (void)argc;
    return lmc_init1(argv[1]);
}

void lmc_close(void) {
    if(dlhandle) {
        dlclose(dlhandle);
    }
}

_Bool lmc_is_open(void) {
    return dlhandle;
}

void *lmc_symbol(char const *symname) {
    return dlsym(dlhandle, symname);
}
