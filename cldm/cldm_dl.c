#include "cldm_dl.h"
#include "cldm_limits.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_ntbs.h"

#include <dlfcn.h>

static void *cldm_libchandle;

void *cldm_dlsym_next(char const *symname) {
    char *err;
    void *sym;
    (void)dlerror();

    sym = dlsym(RTLD_NEXT, symname);

    err = dlerror();
    if(err) {
        cldm_err("%s", err);
        return 0;
    }

    return sym;
}

int cldm_dlclose(void) {
    return dlclose(cldm_libchandle);
}
