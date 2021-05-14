#include <cldm/cldm.h>
#include <cldm/cldm_cache.h>


TEST(cldm_cachealign) {
    struct s0 { unsigned char d[CLDM_L1_CACHE_LINE_SIZE - 1]; };
    struct s1 { unsigned char d[CLDM_L1_CACHE_LINE_SIZE]; };
    struct s2 { unsigned char d[CLDM_L1_CACHE_LINE_SIZE + 1]; };
    ASSERT_EQ(sizeof(cldm_cachealign(int)), CLDM_L1_CACHE_LINE_SIZE);
    ASSERT_EQ(sizeof(cldm_cachealign(struct s0)), CLDM_L1_CACHE_LINE_SIZE);
    ASSERT_EQ(sizeof(cldm_cachealign(struct s1)), CLDM_L1_CACHE_LINE_SIZE);
    ASSERT_EQ(sizeof(cldm_cachealign(struct s2)), 2 * CLDM_L1_CACHE_LINE_SIZE);
}
