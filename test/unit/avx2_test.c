#include <cldm/cldm.h>
#include <cldm/cldm_config.h>

#include <stddef.h>
#include <string.h>

#ifdef CLDM_HAS_AVX2

extern long long cldm_avx2_strscpy(char *restrict dst, char const *restrict src, unsigned long long dstsize);
extern void *cldm_avx2_memset(void *dst, int v, unsigned long long n);
extern void *cldm_avx2_memcpy(void *restrict dst, void const *restrict src, unsigned long long n);
extern int cldm_avx2_memcmp(void const *s0, void const *s1, unsigned long long n);
extern unsigned cldm_avx2_scan_lt(char const *str, int sentinel);
extern unsigned long long cldm_avx2_strlen(char const *str);

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

TEST(cldm_avx2_memcmp) {
    enum { SIZE = 4096 };
    unsigned char s0[SIZE] = { 0 };
    unsigned char s1[SIZE] = { 0 };
    ASSERT_EQ(cldm_avx2_memcmp(s0, s1, sizeof(s0)), 0);
    ASSERT_EQ(cldm_avx2_memcmp(s1, s0, sizeof(s0)), 0);
    memset(s0 + sizeof(s0) / 2, 0xef, sizeof(s0) / 2);
    ASSERT_GT(cldm_avx2_memcmp(s0, s1, sizeof(s0)), 0);
    ASSERT_LT(cldm_avx2_memcmp(s1, s0, sizeof(s0)), 0);
}

TEST(cldm_avx2_memcmp_unaligned) {
    enum { SIZE = 2048 };
    struct {
        unsigned long long p0;
        unsigned char p1;
        unsigned char s0[SIZE];
    } s;
    unsigned char s1[SIZE] = { 0 };
    memset(s.s0, 0, sizeof(s.s0));
    ASSERT_EQ(cldm_avx2_memcmp(s.s0, s1, sizeof(s.s0)), 0);
    ASSERT_EQ(cldm_avx2_memcmp(s1, s.s0, sizeof(s.s0)), 0);
    memset(s.s0 + sizeof(s.s0) / 2, 0x11, sizeof(s.s0) / 2);
    ASSERT_GT(cldm_avx2_memcmp(s.s0, s1, sizeof(s.s0)), 0);
    ASSERT_LT(cldm_avx2_memcmp(s1, s.s0, sizeof(s.s0)), 0);
}

TEST(cldm_avx2_memcmp_odd_size) {
    enum { SIZE = 3999 };
    unsigned char s0[SIZE] = { 0 };
    unsigned char s1[SIZE] = { 0 };
    ASSERT_EQ(cldm_avx2_memcmp(s0, s1, sizeof(s0)), 0);
    ASSERT_EQ(cldm_avx2_memcmp(s1, s0, sizeof(s0)), 0);
    memset(s0 + sizeof(s0) / 2, 0xef, sizeof(s0) / 2);
    ASSERT_GT(cldm_avx2_memcmp(s0, s1, sizeof(s0)), 0);
    ASSERT_LT(cldm_avx2_memcmp(s1, s0, sizeof(s0)), 0);
}

TEST(cldm_avx2_memcpy) {
    enum { SIZE = 4096 };
    unsigned char dst[SIZE];
    unsigned char src[SIZE] = { 0 };

    ASSERT_EQ(cldm_avx2_memcpy(dst, src, sizeof(dst)), dst);
    ASSERT_EQ(memcmp(dst, src, sizeof(dst)), 0);

    memset(src, 0xff, sizeof(src) / 2);
    memset(src + sizeof(src) / 2, 0xaa, sizeof(src) / 2);

    ASSERT_EQ(cldm_avx2_memcpy(dst, src, sizeof(dst)), dst);
    ASSERT_EQ(memcmp(dst, src, sizeof(dst)), 0);
}

TEST(cldm_avx2_memcpy_unaligned) {
    enum { SIZE = 2048 };
    struct {
        unsigned long long p0;
        unsigned char p1;
        unsigned char dst[SIZE];
    } d;
    unsigned char src[SIZE] = { 0 };

    ASSERT_EQ(cldm_avx2_memcpy(d.dst, src, sizeof(d.dst)), d.dst);
    ASSERT_EQ(memcmp(d.dst, src, sizeof(d.dst)), 0);

    memset(src, 0xff, sizeof(src) / 2);
    memset(src + sizeof(src) / 2, 0xaa, sizeof(src) / 2);

    ASSERT_EQ(cldm_avx2_memcpy(d.dst, src, sizeof(d.dst)), d.dst);
    ASSERT_EQ(memcmp(d.dst, src, sizeof(d.dst)), 0);
}

TEST(cldm_avx2_memcpy_odd_size) {
    enum { SIZE = 3999 };
    unsigned char dst[SIZE];
    unsigned char src[SIZE] = { 0 };

    ASSERT_EQ(cldm_avx2_memcpy(dst, src, sizeof(dst)), dst);
    ASSERT_EQ(memcmp(dst, src, sizeof(dst)), 0);

    memset(src, 0xff, sizeof(src) / 2);
    memset(src + sizeof(src) / 2, 0xaa, sizeof(src) / 2);

    ASSERT_EQ(cldm_avx2_memcpy(dst, src, sizeof(dst)), dst);
    ASSERT_EQ(memcmp(dst, src, sizeof(dst)), 0);
}

TEST(cldm_avx2_scan_lt) {
    char const *relapse = "Noises. Noises. Are all I hear when you speak. The point of it all left much faster than it crept to me";
    ASSERT_EQ(cldm_avx2_scan_lt("contort", 'o'), 0);
    ASSERT_EQ(cldm_avx2_scan_lt("packed", 'c'), 1);
    ASSERT_EQ(cldm_avx2_scan_lt("godspeed", 'a'), strlen("godspeed"));
    ASSERT_EQ(cldm_avx2_scan_lt(relapse, 1), strlen(relapse));
}

TEST(cldm_avx2_scan_lt_unaligned) {
    enum { SIZE = 1024 };
    struct {
        unsigned long long p0;
        unsigned char p1;
        char str[SIZE];
    } s;
    memset(s.str, 0, sizeof(s.str));
    char const *sp = "All mixed up, turned around. You're thinking out loud. What were we talking about now? Emotion free, euphoric stat. Too little too late. Let it go";
    strcpy(s.str, sp);
    ASSERT_EQ(cldm_avx2_scan_lt(s.str, 1), strlen(s.str));
}

TEST(cldm_avx2_strlen) {
    char const *str = "Pensive";
    ASSERT_EQ(cldm_avx2_strlen(str), strlen(str));
    str = "I made it up. It's complicated. I gave it up. On giving it a name. Things are implied. Nothing's the same";
    ASSERT_EQ(cldm_avx2_strlen(str), strlen(str));
    str = "If you pray for us. Pray for love. It might just save us from. It just might. Wish you godspeed. Oh, please, whatever it might be. Tell it about the way it is";
    ASSERT_EQ(cldm_avx2_strlen(str), strlen(str));
}

TEST(cldm_avx2_strlen_empty) {
    char const *str = "";
    ASSERT_EQ(cldm_avx2_strlen(str), strlen(str));
}

#endif /* CLDM_HAS_AVX2 */
