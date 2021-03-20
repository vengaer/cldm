#include <cldm/cldm.h>
#include <cldm/cldm_log.h>
#include <cldm/cldm_ntbs.h>

TEST(cldm_ntbschr) {
    char const *str = "a string with spaces";
    ASSERT_EQ(cldm_ntbschr((char *)str, ' '), str + 1);
    str = "some/path/to/a/file";
    ASSERT_EQ(cldm_ntbschr((char *)str, '/'), str + 4);
    str = "a string without slashes";
    ASSERT_EQ(cldm_ntbschr((char *)str, '/'), 0);
}

TEST(cldm_ntbscrchr) {
    char const *str = "a string with spaces";
    ASSERT_EQ(cldm_ntbscrchr(str, ' '), str + 13);
    str = "/some/path/to/a/file";
    ASSERT_EQ(cldm_ntbscrchr(str, '/'), str + 15);
    str = "a_string";
    ASSERT_EQ(cldm_ntbscrchr(str, ' '), 0);
}
