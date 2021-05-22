#include <cldm/cldm.h>
#include <cldm/cldm_config.h>

#include <stddef.h>
#include <string.h>

#ifdef CLDM_HAS_AVX2

extern long long cldm_avx2_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize);
extern void *cldm_avx2_memset(void *dst, int v, unsigned long long n);

TEST(cldm_avx2_strscpy) {
    enum { SIZE = 256 };
    char dst[SIZE];
    char const *src = "Fear Inoculum Pneuma Litanie contre la Peur Invincible Legion Inculant Descending Culling Voices Chocolate Chip Trip 7empest Mockingbeat";
    ASSERT_EQ(cldm_avx2_strscpy(dst, src, sizeof(dst)), (long long)strlen(src));
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

TEST(cldm_avx2_memset) {
    enum { SIZE = 4096 };
    char dst[SIZE] = { 0 };
    char ref[SIZE];
    memset(ref, 0xff, sizeof(ref));
    ASSERT_EQ(cldm_avx2_memset(dst, 0xff, sizeof(dst)), dst);
    ASSERT_EQ(memcmp(dst, ref, sizeof(dst)), 0);
}

TEST(cldm_avx2_memset_unaligned) {
    enum { SIZE = 2048 };
    struct {
        unsigned long long p0;
        unsigned char p1;
        char dst[SIZE];
    } d;
    char ref[SIZE];

    memset(d.dst, 0, sizeof(d.dst));
    memset(ref, 0xff, sizeof(ref));

    ASSERT_EQ(cldm_avx2_memset(d.dst, 0xff, sizeof(d.dst)), d.dst);
    ASSERT_EQ(memcmp(d.dst, ref, sizeof(d.dst)), 0);
}

TEST(cldm_avx2_memset_odd_size) {
    enum { SIZE = 3999 };
    char dst[SIZE] = { 0 };
    char ref[SIZE];

    memset(ref, 0xff, sizeof(ref));

    ASSERT_EQ(cldm_avx2_memset(dst, 0xff, sizeof(dst)), dst);
    ASSERT_EQ(memcmp(dst, ref, sizeof(dst)), 0);
}

#endif /* CLDM_HAS_AVX2 */
