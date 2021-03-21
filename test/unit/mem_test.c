#include <cldm/cldm.h>
#include <cldm/cldm_macro.h>
#include <cldm/cldm_mem.h>

TEST(cldm_mmove) {
    unsigned char m[] = { 1, 2, 3, 4, 5, 6 };
    unsigned char n[] = { 1, 2, 3, 4, 5, 6 };

    cldm_mmove(&m[1], m, cldm_arrsize(m) - 1);

    unsigned char *iter;
    cldm_for_each(iter, (m + 1), cldm_arrsize(m) - 1) {
        ASSERT_EQ(*iter, 1);
    }

    cldm_mmove(m, n, cldm_arrsize(n));

    unsigned i = 0;
    cldm_for_each(iter, m) {
        ASSERT_EQ(*iter, n[i++]);
    }
}

TEST(cldm_mcpy) {
    unsigned src[] = {
        1, 2, 3, 4, 5, 6, 7, 8, 1, 2
    };
    unsigned dst[cldm_arrsize(src)];
    cldm_mcpy(dst, src, sizeof(src));

    unsigned *iter;
    unsigned i = 0;
    cldm_for_each(iter, dst) {
        ASSERT_EQ(*iter, src[i++]);
    }
}
