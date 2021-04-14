#include "cldm_dl.h"
#include "cldm_log.h"

#include <dlfcn.h>

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
