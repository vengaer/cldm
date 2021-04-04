#include <cldm/cldm.h>
#include <cldm/cldm_algo.h>
#include <cldm/cldm_macro.h>

#include <stdbool.h>

static bool is_even(void const *d) {
    return !((*(unsigned const *)d) % 2);
}

static bool gt_5(void const *d) {
    return (*(unsigned const*)d) > 5;
}

TEST(cldm_stable_partition) {
    unsigned arr[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    unsigned partitioned0[] = { 2, 4, 6, 8, 1, 3, 5, 7 };
    unsigned partitioned1[] = { 6, 8, 7, 2, 4, 1, 3, 5 };

    cldm_stable_partition(arr, sizeof(unsigned), cldm_arrsize(arr), is_even);

    for(unsigned i = 0; i < cldm_arrsize(arr); i++) {
        ASSERT_EQ(arr[i], partitioned0[i]);
    }

    cldm_stable_partition(arr, sizeof(unsigned), cldm_arrsize(arr), gt_5);

    for(unsigned i = 0; i < cldm_arrsize(arr); i++) {
        ASSERT_EQ(arr[i], partitioned1[i]);
    }
}
