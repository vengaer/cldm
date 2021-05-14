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

static int ucmp(void const *v0, void const *v1) {
    return *(int const *)v1 - *(int const *)v0;
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

TEST(cldm_uniq) {
    unsigned arr[] = { 1, 2, 3, 4, 4, 5, 6, 7, 7, 7, 8 };
    unsigned res[] = { 1, 2 ,3, 4, 5, 6, 7, 8 };
    unsigned *it0;
    unsigned *it1;

    ASSERT_EQ(cldm_uniq(arr, arr, sizeof(arr[0]), cldm_arrsize(arr), ucmp), cldm_arrsize(res));

    cldm_for_each_zip(it0, it1, arr, res) {
        ASSERT_EQ(*it0, *it1);
    }

    memcpy(arr, (unsigned[cldm_arrsize(arr)]) { 1, 2, 3, 4, 4, 2, 6, 7, 7, 7, 8 }, sizeof(arr));
    memcpy(res, (unsigned[cldm_arrsize(res)]) { 1, 2, 3, 4, 2, 6, 7, 8 }, sizeof(res));

    ASSERT_EQ(cldm_uniq(arr, arr, sizeof(arr[0]), cldm_arrsize(arr), ucmp), cldm_arrsize(res));

    cldm_for_each_zip(it0, it1, arr, res) {
        ASSERT_EQ(*it0, *it1);
    }
}

TEST(cldm_ndigits) {
    ASSERT_EQ(cldm_ndigits(1u), 1);
    ASSERT_EQ(cldm_ndigits(18u), 2);
    ASSERT_EQ(cldm_ndigits(122u), 3);
    ASSERT_EQ(cldm_ndigits(1221u), 4);
    ASSERT_EQ(cldm_ndigits(12218u), 5);
}
