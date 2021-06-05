#include <cldm/cldm.h>
#include <cldm/cldm_cache.h>
#include <cldm/cldm_config.h>


TEST(cldm_cachealign) {
    struct s0 { unsigned char d[CLDM_L1_DCACHE_LINESIZE - 1]; };
    struct s1 { unsigned char d[CLDM_L1_DCACHE_LINESIZE]; };
    struct s2 { unsigned char d[CLDM_L1_DCACHE_LINESIZE + 1]; };
    ASSERT_EQ(sizeof(cldm_cachealign(int)), CLDM_L1_DCACHE_LINESIZE);
    ASSERT_EQ(sizeof(cldm_cachealign(struct s0)), CLDM_L1_DCACHE_LINESIZE);
    ASSERT_EQ(sizeof(cldm_cachealign(struct s1)), CLDM_L1_DCACHE_LINESIZE);
    ASSERT_EQ(sizeof(cldm_cachealign(struct s2)), 2 * CLDM_L1_DCACHE_LINESIZE);
}
