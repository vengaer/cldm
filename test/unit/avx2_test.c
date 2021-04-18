#include <cldm/cldm.h>

#include <stddef.h>
#include <string.h>

extern long long cldm_avx2_strscpy(char *restrict dst, char const *restrict src, size_t dstsize);

TEST(cldm_avx2_strscpy) {
    enum { SIZE = 256 };
    char dst[SIZE];
    char const *src = "Fear Inoculum Pneuma Litanie contre la Peur Invincible Legion Inculant Descending Culling Voices Chocolate Chip Trip 7empest Mockingbeat";
    ASSERT_EQ(cldm_avx2_strscpy(dst, src, sizeof(dst)), (ssize_t)strlen(src));
    ASSERT_STREQ(dst, src);
}

TEST(cldm_avx2_strscpy_termination) {
    enum { SIZE = 16 };
    char dst[SIZE];
    char const *src = "Hear lighting flash, but don't blink. Misleading. Tranquility ruse. You're gonna happen again. That's what I think";

    ASSERT_EQ(cldm_avx2_strscpy(dst, src, sizeof(dst)), -7);
    ASSERT_STRNEQ(src, dst, sizeof(dst) - 1);
    ASSERT_EQ(dst[sizeof(dst) - 1], 0);
}

TEST(cldm_avx2_strscpy_empty) {
    enum { SIZE = 32 };
    char dst[SIZE];
    char const *src = "";
    ASSERT_EQ(cldm_avx2_strscpy(dst, src, sizeof(dst)), 0);
    ASSERT_EQ(dst[0], 0);
}

TEST(cldm_avx2_strscpy_dstsize_zero) {
    enum { SIZE = 32 };
    char dst[SIZE] = { 0x18 };
    char const *src = "asdf";
    ASSERT_EQ(cldm_avx2_strscpy(dst, src, 0), -7);
    ASSERT_EQ(dst[0], 0x18);
}
