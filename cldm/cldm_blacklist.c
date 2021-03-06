#include "cldm_blacklist.h"
#include "cldm_macro.h"
#include "cldm_ntbs.h"

static char const *blacklist[] = {
    "dlopen",
    "dlclose",
    "dlsym",
    "getenv",
    "setenv"
};

bool cldm_is_blacklisted(char const *function) {
    char const **iter;

    cldm_for_each(iter, blacklist) {
        if (cldm_ntbscmp(*iter, function) == 0) {
            return true;
        }
    }

    return false;
}
