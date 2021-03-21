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
