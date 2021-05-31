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

TEST(cldm_map) {
#define m(ctr, v) ASSERT_EQ(ctr, v);
    cldm_map(m, 0, 1, 2, 3, 4, 5, 6, 7, 8);
#undef m
}

TEST(cldm_map_separate) {
#define m(ctr, v) ASSERT_EQ(ctr, v)
    cldm_map_separate(m, ;, 0, 1, 2, 3, 4, 5, 6, 7, 8);
#undef m
}

TEST(cldm_inc) {
    ASSERT_EQ(cldm_inc(0), 1);
    ASSERT_EQ(cldm_inc(1), 2);
    ASSERT_EQ(cldm_inc(2), 3);
    ASSERT_EQ(cldm_inc(3), 4);
    ASSERT_EQ(cldm_inc(4), 5);
    ASSERT_EQ(cldm_inc(5), 6);
    ASSERT_EQ(cldm_inc(6), 7);
    ASSERT_EQ(cldm_inc(7), 8);
    ASSERT_EQ(cldm_inc(8), 9);
    ASSERT_EQ(cldm_inc(9), 10);
    ASSERT_EQ(cldm_inc(10), 11);
    ASSERT_EQ(cldm_inc(11), 12);
    ASSERT_EQ(cldm_inc(12), 13);
    ASSERT_EQ(cldm_inc(13), 14);
    ASSERT_EQ(cldm_inc(14), 15);
    ASSERT_EQ(cldm_inc(15), 16);
    ASSERT_EQ(cldm_inc(16), 17);
    ASSERT_EQ(cldm_inc(17), 18);
    ASSERT_EQ(cldm_inc(18), 19);
    ASSERT_EQ(cldm_inc(19), 20);
    ASSERT_EQ(cldm_inc(20), 21);
    ASSERT_EQ(cldm_inc(21), 22);
    ASSERT_EQ(cldm_inc(22), 23);
    ASSERT_EQ(cldm_inc(23), 24);
    ASSERT_EQ(cldm_inc(24), 25);
    ASSERT_EQ(cldm_inc(25), 26);
    ASSERT_EQ(cldm_inc(26), 27);
    ASSERT_EQ(cldm_inc(27), 28);
    ASSERT_EQ(cldm_inc(28), 29);
    ASSERT_EQ(cldm_inc(29), 30);
    ASSERT_EQ(cldm_inc(30), 31);
    ASSERT_EQ(cldm_inc(31), 32);
    ASSERT_EQ(cldm_inc(32), 33);
    ASSERT_EQ(cldm_inc(33), 34);
    ASSERT_EQ(cldm_inc(34), 35);
    ASSERT_EQ(cldm_inc(35), 36);
    ASSERT_EQ(cldm_inc(36), 37);
    ASSERT_EQ(cldm_inc(37), 38);
    ASSERT_EQ(cldm_inc(38), 39);
    ASSERT_EQ(cldm_inc(39), 40);
    ASSERT_EQ(cldm_inc(40), 41);
    ASSERT_EQ(cldm_inc(41), 42);
    ASSERT_EQ(cldm_inc(42), 43);
    ASSERT_EQ(cldm_inc(43), 44);
    ASSERT_EQ(cldm_inc(44), 45);
    ASSERT_EQ(cldm_inc(45), 46);
    ASSERT_EQ(cldm_inc(46), 47);
    ASSERT_EQ(cldm_inc(47), 48);
    ASSERT_EQ(cldm_inc(48), 49);
    ASSERT_EQ(cldm_inc(49), 50);
    ASSERT_EQ(cldm_inc(50), 51);
    ASSERT_EQ(cldm_inc(51), 52);
    ASSERT_EQ(cldm_inc(52), 53);
    ASSERT_EQ(cldm_inc(53), 54);
    ASSERT_EQ(cldm_inc(54), 55);
    ASSERT_EQ(cldm_inc(55), 56);
    ASSERT_EQ(cldm_inc(56), 57);
    ASSERT_EQ(cldm_inc(57), 58);
    ASSERT_EQ(cldm_inc(58), 59);
    ASSERT_EQ(cldm_inc(59), 60);
    ASSERT_EQ(cldm_inc(60), 61);
    ASSERT_EQ(cldm_inc(61), 62);
    ASSERT_EQ(cldm_inc(62), 63);
    ASSERT_EQ(cldm_inc(63), 64);
    ASSERT_EQ(cldm_inc(64), 65);
    ASSERT_EQ(cldm_inc(65), 66);
    ASSERT_EQ(cldm_inc(66), 67);
    ASSERT_EQ(cldm_inc(67), 68);
    ASSERT_EQ(cldm_inc(68), 69);
    ASSERT_EQ(cldm_inc(69), 70);
    ASSERT_EQ(cldm_inc(70), 71);
    ASSERT_EQ(cldm_inc(71), 72);
    ASSERT_EQ(cldm_inc(72), 73);
    ASSERT_EQ(cldm_inc(73), 74);
    ASSERT_EQ(cldm_inc(74), 75);
    ASSERT_EQ(cldm_inc(75), 76);
    ASSERT_EQ(cldm_inc(76), 77);
    ASSERT_EQ(cldm_inc(77), 78);
    ASSERT_EQ(cldm_inc(78), 79);
    ASSERT_EQ(cldm_inc(79), 80);
    ASSERT_EQ(cldm_inc(80), 81);
    ASSERT_EQ(cldm_inc(81), 82);
    ASSERT_EQ(cldm_inc(82), 83);
    ASSERT_EQ(cldm_inc(83), 84);
    ASSERT_EQ(cldm_inc(84), 85);
    ASSERT_EQ(cldm_inc(85), 86);
    ASSERT_EQ(cldm_inc(86), 87);
    ASSERT_EQ(cldm_inc(87), 88);
    ASSERT_EQ(cldm_inc(88), 89);
    ASSERT_EQ(cldm_inc(89), 90);
    ASSERT_EQ(cldm_inc(90), 91);
    ASSERT_EQ(cldm_inc(91), 92);
    ASSERT_EQ(cldm_inc(92), 93);
    ASSERT_EQ(cldm_inc(93), 94);
    ASSERT_EQ(cldm_inc(94), 95);
    ASSERT_EQ(cldm_inc(95), 96);
    ASSERT_EQ(cldm_inc(96), 97);
    ASSERT_EQ(cldm_inc(97), 98);
    ASSERT_EQ(cldm_inc(98), 99);
    ASSERT_EQ(cldm_inc(99), 100);
    ASSERT_EQ(cldm_inc(100), 101);
    ASSERT_EQ(cldm_inc(101), 102);
    ASSERT_EQ(cldm_inc(102), 103);
    ASSERT_EQ(cldm_inc(103), 104);
    ASSERT_EQ(cldm_inc(104), 105);
    ASSERT_EQ(cldm_inc(105), 106);
    ASSERT_EQ(cldm_inc(106), 107);
    ASSERT_EQ(cldm_inc(107), 108);
    ASSERT_EQ(cldm_inc(108), 109);
    ASSERT_EQ(cldm_inc(109), 110);
    ASSERT_EQ(cldm_inc(110), 111);
    ASSERT_EQ(cldm_inc(111), 112);
    ASSERT_EQ(cldm_inc(112), 113);
    ASSERT_EQ(cldm_inc(113), 114);
    ASSERT_EQ(cldm_inc(114), 115);
    ASSERT_EQ(cldm_inc(115), 116);
    ASSERT_EQ(cldm_inc(116), 117);
    ASSERT_EQ(cldm_inc(117), 118);
    ASSERT_EQ(cldm_inc(118), 119);
    ASSERT_EQ(cldm_inc(119), 120);
    ASSERT_EQ(cldm_inc(120), 121);
    ASSERT_EQ(cldm_inc(121), 122);
    ASSERT_EQ(cldm_inc(122), 123);
    ASSERT_EQ(cldm_inc(123), 124);
    ASSERT_EQ(cldm_inc(124), 125);
    ASSERT_EQ(cldm_inc(125), 126);
    ASSERT_EQ(cldm_inc(126), 127);
}

TEST(cldm_dec) {
    ASSERT_EQ(cldm_dec(1), 0);
    ASSERT_EQ(cldm_dec(2), 1);
    ASSERT_EQ(cldm_dec(3), 2);
    ASSERT_EQ(cldm_dec(4), 3);
    ASSERT_EQ(cldm_dec(5), 4);
    ASSERT_EQ(cldm_dec(6), 5);
    ASSERT_EQ(cldm_dec(7), 6);
    ASSERT_EQ(cldm_dec(8), 7);
    ASSERT_EQ(cldm_dec(9), 8);
    ASSERT_EQ(cldm_dec(10), 9);
    ASSERT_EQ(cldm_dec(11), 10);
    ASSERT_EQ(cldm_dec(12), 11);
    ASSERT_EQ(cldm_dec(13), 12);
    ASSERT_EQ(cldm_dec(14), 13);
    ASSERT_EQ(cldm_dec(15), 14);
    ASSERT_EQ(cldm_dec(16), 15);
    ASSERT_EQ(cldm_dec(17), 16);
    ASSERT_EQ(cldm_dec(18), 17);
    ASSERT_EQ(cldm_dec(19), 18);
    ASSERT_EQ(cldm_dec(20), 19);
    ASSERT_EQ(cldm_dec(21), 20);
    ASSERT_EQ(cldm_dec(22), 21);
    ASSERT_EQ(cldm_dec(23), 22);
    ASSERT_EQ(cldm_dec(24), 23);
    ASSERT_EQ(cldm_dec(25), 24);
    ASSERT_EQ(cldm_dec(26), 25);
    ASSERT_EQ(cldm_dec(27), 26);
    ASSERT_EQ(cldm_dec(28), 27);
    ASSERT_EQ(cldm_dec(29), 28);
    ASSERT_EQ(cldm_dec(30), 29);
    ASSERT_EQ(cldm_dec(31), 30);
    ASSERT_EQ(cldm_dec(32), 31);
    ASSERT_EQ(cldm_dec(33), 32);
    ASSERT_EQ(cldm_dec(34), 33);
    ASSERT_EQ(cldm_dec(35), 34);
    ASSERT_EQ(cldm_dec(36), 35);
    ASSERT_EQ(cldm_dec(37), 36);
    ASSERT_EQ(cldm_dec(38), 37);
    ASSERT_EQ(cldm_dec(39), 38);
    ASSERT_EQ(cldm_dec(40), 39);
    ASSERT_EQ(cldm_dec(41), 40);
    ASSERT_EQ(cldm_dec(42), 41);
    ASSERT_EQ(cldm_dec(43), 42);
    ASSERT_EQ(cldm_dec(44), 43);
    ASSERT_EQ(cldm_dec(45), 44);
    ASSERT_EQ(cldm_dec(46), 45);
    ASSERT_EQ(cldm_dec(47), 46);
    ASSERT_EQ(cldm_dec(48), 47);
    ASSERT_EQ(cldm_dec(49), 48);
    ASSERT_EQ(cldm_dec(50), 49);
    ASSERT_EQ(cldm_dec(51), 50);
    ASSERT_EQ(cldm_dec(52), 51);
    ASSERT_EQ(cldm_dec(53), 52);
    ASSERT_EQ(cldm_dec(54), 53);
    ASSERT_EQ(cldm_dec(55), 54);
    ASSERT_EQ(cldm_dec(56), 55);
    ASSERT_EQ(cldm_dec(57), 56);
    ASSERT_EQ(cldm_dec(58), 57);
    ASSERT_EQ(cldm_dec(59), 58);
    ASSERT_EQ(cldm_dec(60), 59);
    ASSERT_EQ(cldm_dec(61), 60);
    ASSERT_EQ(cldm_dec(62), 61);
    ASSERT_EQ(cldm_dec(63), 62);
    ASSERT_EQ(cldm_dec(64), 63);
    ASSERT_EQ(cldm_dec(65), 64);
    ASSERT_EQ(cldm_dec(66), 65);
    ASSERT_EQ(cldm_dec(67), 66);
    ASSERT_EQ(cldm_dec(68), 67);
    ASSERT_EQ(cldm_dec(69), 68);
    ASSERT_EQ(cldm_dec(70), 69);
    ASSERT_EQ(cldm_dec(71), 70);
    ASSERT_EQ(cldm_dec(72), 71);
    ASSERT_EQ(cldm_dec(73), 72);
    ASSERT_EQ(cldm_dec(74), 73);
    ASSERT_EQ(cldm_dec(75), 74);
    ASSERT_EQ(cldm_dec(76), 75);
    ASSERT_EQ(cldm_dec(77), 76);
    ASSERT_EQ(cldm_dec(78), 77);
    ASSERT_EQ(cldm_dec(79), 78);
    ASSERT_EQ(cldm_dec(80), 79);
    ASSERT_EQ(cldm_dec(81), 80);
    ASSERT_EQ(cldm_dec(82), 81);
    ASSERT_EQ(cldm_dec(83), 82);
    ASSERT_EQ(cldm_dec(84), 83);
    ASSERT_EQ(cldm_dec(85), 84);
    ASSERT_EQ(cldm_dec(86), 85);
    ASSERT_EQ(cldm_dec(87), 86);
    ASSERT_EQ(cldm_dec(88), 87);
    ASSERT_EQ(cldm_dec(89), 88);
    ASSERT_EQ(cldm_dec(90), 89);
    ASSERT_EQ(cldm_dec(91), 90);
    ASSERT_EQ(cldm_dec(92), 91);
    ASSERT_EQ(cldm_dec(93), 92);
    ASSERT_EQ(cldm_dec(94), 93);
    ASSERT_EQ(cldm_dec(95), 94);
    ASSERT_EQ(cldm_dec(96), 95);
    ASSERT_EQ(cldm_dec(97), 96);
    ASSERT_EQ(cldm_dec(98), 97);
    ASSERT_EQ(cldm_dec(99), 98);
    ASSERT_EQ(cldm_dec(100), 99);
    ASSERT_EQ(cldm_dec(101), 100);
    ASSERT_EQ(cldm_dec(102), 101);
    ASSERT_EQ(cldm_dec(103), 102);
    ASSERT_EQ(cldm_dec(104), 103);
    ASSERT_EQ(cldm_dec(105), 104);
    ASSERT_EQ(cldm_dec(106), 105);
    ASSERT_EQ(cldm_dec(107), 106);
    ASSERT_EQ(cldm_dec(108), 107);
    ASSERT_EQ(cldm_dec(109), 108);
    ASSERT_EQ(cldm_dec(110), 109);
    ASSERT_EQ(cldm_dec(111), 110);
    ASSERT_EQ(cldm_dec(112), 111);
    ASSERT_EQ(cldm_dec(113), 112);
    ASSERT_EQ(cldm_dec(114), 113);
    ASSERT_EQ(cldm_dec(115), 114);
    ASSERT_EQ(cldm_dec(116), 115);
    ASSERT_EQ(cldm_dec(117), 116);
    ASSERT_EQ(cldm_dec(118), 117);
    ASSERT_EQ(cldm_dec(119), 118);
    ASSERT_EQ(cldm_dec(120), 119);
    ASSERT_EQ(cldm_dec(121), 120);
    ASSERT_EQ(cldm_dec(122), 121);
    ASSERT_EQ(cldm_dec(123), 122);
    ASSERT_EQ(cldm_dec(124), 123);
    ASSERT_EQ(cldm_dec(125), 124);
    ASSERT_EQ(cldm_dec(126), 125);
    ASSERT_EQ(cldm_dec(127), 126);
}
