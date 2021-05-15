#include <cldm/cldm.h>
#include <cldm/cldm_macro.h>

#include <string.h>

TEST(cldm_cat_expand) {
    int fb = 3;
    #define foo f
    #define bar b
    ASSERT_EQ(cldm_cat_expand(foo,bar), 3);

    #undef foo
    #undef bar
}

TEST(cldm_str_expand) {
    #define ver 0.1.0
    ASSERT_EQ(strcmp(cldm_str_expand(ver), "0.1.0"), 0);
    #undef ver
}

TEST(cldm_arrsize) {
    ASSERT_EQ(cldm_arrsize((unsigned [10]){ 0 }), 10);
    ASSERT_EQ(cldm_arrsize((char [256]){ 0 }), 256);
}

TEST(cldm_offset) {
    struct ps {
        char pad;
        unsigned long long ull;
    } ps;
    ASSERT_EQ(cldm_offset(struct ps, ull), (size_t)&ps.ull - (size_t)&ps);
}

TEST(cldm_for_each2) {
    unsigned arr[] = { 2, 4, 5, 6, 7 };
    unsigned *iter;
    unsigned i = 0;
    cldm_for_each(iter, arr) {
        ASSERT_EQ(*iter, arr[i++]);
    }
}

TEST(cldm_for_each3) {
    unsigned arr[] = { 2, 4, 5, 6, 7 };
    unsigned *iter;
    unsigned i = 0;
    cldm_for_each(iter, arr, 3) {
        ASSERT_EQ(*iter, arr[i++]);
    }

    ASSERT_EQ(i, 3);
}

TEST(cldm_for_each4) {
    unsigned arr[] = { 2, 4, 5, 6, 7 };
    unsigned *iter;
    unsigned i = 0;
    cldm_for_each(iter, arr, 5, 2) {
        ASSERT_EQ(*iter, arr[i]);
        i += 2;
    }

    ASSERT_EQ(i, 6);
}

TEST(cldm_for_each_zip4) {
    unsigned arr0[] = { 2, 3, 4, 5, 6, 7 };
    char arr1[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
    unsigned *it0;
    char *it1;

    unsigned i = 0;
    cldm_for_each_zip(it0, it1, arr0, arr1) {
        ASSERT_EQ(*it0, arr0[i]);
        ASSERT_EQ(*it1, arr1[i++]);
    }
}

TEST(cldm_for_each_zip5) {
    unsigned arr0[] = { 2, 3, 4, 5, 6, 7 };
    char arr1[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
    unsigned *it0;
    char *it1;

    unsigned i = 0;
    cldm_for_each_zip(it0, it1, arr0, arr1, 3) {
        ASSERT_EQ(*it0, arr0[i]);
        ASSERT_EQ(*it1, arr1[i++]);
    }

    ASSERT_EQ(i, 3u);
}

TEST(cldm_for_each_zip6) {
    unsigned arr0[] = { 2, 3, 4, 5, 6, 7 };
    char arr1[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
    unsigned *it0;
    char *it1;

    unsigned i = 0;
    cldm_for_each_zip(it0, it1, arr0, arr1, cldm_arrsize(arr0), 2) {
        ASSERT_EQ(*it0, arr0[i]);
        ASSERT_EQ(*it1, arr1[i]);
        i += 2;
    }
}

TEST(cldm_for_each_zip_arr0_gt_arr1) {
    unsigned arr0[] = { 2, 3, 4, 5, 6, 7 };
    char arr1[] = { 'a', 'b', 'c' };
    unsigned *it0;
    char *it1;

    unsigned i = 0;
    cldm_for_each_zip(it0, it1, arr0, arr1) {
        ASSERT_EQ(*it0, arr0[i]);
        ASSERT_EQ(*it1, arr1[i++]);
    }

    ASSERT_EQ(i, cldm_arrsize(arr1));
}

TEST(cldm_for_each_zip_arr0_lt_arr1) {
    unsigned arr0[] = { 2, 3, 4, 5 };
    char arr1[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
    unsigned *it0;
    char *it1;

    unsigned i = 0;
    cldm_for_each_zip(it0, it1, arr0, arr1) {
        ASSERT_EQ(*it0, arr0[i]);
        ASSERT_EQ(*it1, arr1[i++]);
    }

    ASSERT_EQ(i, cldm_arrsize(arr0));
}

TEST(cldm_arrindex) {
    unsigned arr[] = { 1, 2, 3, 4, 5, 6, 7 };

    ASSERT_EQ(cldm_arrindex(arr, &arr[0]), 0);
    ASSERT_EQ(cldm_arrindex(arr, &arr[1]), 1);
    ASSERT_EQ(cldm_arrindex(arr, &arr[2]), 2);
    ASSERT_EQ(cldm_arrindex(arr, &arr[3]), 3);
    ASSERT_EQ(cldm_arrindex(arr, &arr[4]), 4);
    ASSERT_EQ(cldm_arrindex(arr, &arr[5]), 5);
    ASSERT_EQ(cldm_arrindex(arr, &arr[6]), 6);
}
