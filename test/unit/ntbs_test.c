#include <cldm/cldm.h>
#include <cldm/cldm_log.h>
#include <cldm/cldm_ntbs.h>

#include <string.h>

#include <sys/types.h>

TEST(cldm_strscpy) {
    enum { BUFSIZE = 64 };
    char src[BUFSIZE];
    char dst[BUFSIZE];

    strcpy(src, "a string");

    ASSERT_EQ(cldm_strscpy(dst, src, sizeof(src)), (ssize_t)strlen(src));
    ASSERT_EQ(strcmp(dst, src), 0);

    ASSERT_LT(cldm_strscpy(dst, src, 4), 0);
    ASSERT_EQ(dst[3], 0);
    ASSERT_EQ(strcmp(dst, "a s"), 0);

    ASSERT_EQ(cldm_strscpy(dst, src, 9), (ssize_t)strlen(src));
    ASSERT_EQ(dst[strlen(src)], 0);
}

TEST(cldm_basename) {
    ASSERT_EQ(strcmp("foo", cldm_basename("bar/foo")), 0);
    ASSERT_EQ(strcmp("foo", cldm_basename("foo")), 0);
}
