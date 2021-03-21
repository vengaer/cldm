#include <cldm/cldm.h>
#include <cldm/cldm_blacklist.h>
#include <cldm/cldm_macro.h>

TEST(blacklist) {
    char const *blist[] = {
        "dlopen",
        "dlclose",
        "dlerror",
        "dlsym",
        "mmap",
        "munmap",
        "open",
        "close",
        "fstat"
    };
    char const **iter;

    cldm_for_each(iter, blist) {
        ASSERT_TRUE(cldm_is_blacklisted(*iter));
    }

    ASSERT_FALSE(cldm_is_blacklisted("gets"));
}
