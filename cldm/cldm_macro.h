#ifndef CLDM_MACRO_H
#define CLDM_MACRO_H

#include "cldm_token.h"

#include <stddef.h>

#define cldm_cat(a, b) a ## b
#define cldm_cat_expand(a,b) cldm_cat(a,b)

#define cldm_str(a) #a
#define cldm_str_expand(a) cldm_str(a)

#define cldm_chk_pick(_, x, ...) x
#define cldm_chk(...) cldm_chk_pick(__VA_ARGS__, 0,)

#define cldm_bytediff(p0, p1)   \
    ((unsigned char const *)(p1) - (unsigned char const *)(p0))

#define cldm_arrsize(arr) (sizeof(arr) / sizeof(arr[0]))

#define cldm_arrindex(arr, ptr) \
    ((size_t)(cldm_bytediff(&(arr)[0], (ptr)) / sizeof(*ptr)))

#define cldm_strlitlen(str) (sizeof(str "") - 1u)

#define cldm_offset(type, member)   \
    (size_t)&(((type *)0)->member)

#define cldm_alignof(type)  \
    cldm_offset(struct { char b; type t; }, t)

#define cldm_overload(name, ...)    \
    cldm_cat_expand(name, cldm_count(__VA_ARGS__))(__VA_ARGS__)

#define cldm_container4(addr, type, member, qual)   \
    ((type qual *) ((unsigned char qual *)addr - cldm_offset(type, member)))

#define cldm_container3(addr, type, member) \
    cldm_container4(addr, type, member,)

#define cldm_container(...) \
    cldm_overload(cldm_container, __VA_ARGS__)

#define cldm_first_pick(x, ...) x
#define cldm_first(...) cldm_first_pick(__VA_ARGS__,)

#define cldm_second_pick(_, x, ...) x
#define cldm_second(...) cldm_second_pick(__VA_ARGS__,)

#define cldm_declval(type) (*((type *)0))

#define cldm_count_sequence 128, 127, 126, 125, 124, 123, 122, 121, \
                            120, 119, 118, 117, 116, 115, 114, 113, \
                            112, 111, 110, 109, 108, 107, 106, 105, \
                            104, 103, 102, 101, 100, 99,  98,  97,  \
                            96,  95,  94,  93,  92,  91,  90,  89,  \
                            88,  87,  86,  85,  84,  83,  82,  81,  \
                            80,  79,  78,  77,  76,  75,  74,  73,  \
                            72,  71,  70,  69,  68,  67,  66,  65,  \
                            64,  63,  62,  61,  60,  59,  58,  57,  \
                            56,  55,  54,  53,  52,  51,  50,  49,  \
                            48,  47,  46,  45,  44,  43,  42,  41,  \
                            40,  39,  38,  37,  36,  35,  34,  33,  \
                            32,  31,  30,  29,  28,  27,  26,  25,  \
                            24,  23,  22,  21,  20,  19,  18,  17,  \
                            16,  15,  14,  13,  12,  11,  10,  9,   \
                            8,   7,   6,   5,   4,   3,   2,   1,   \
                            0

#define cldm_count_pick(a0,   a1,   a2,   a3,   a4,   a5,   a6,   a7,      \
                        a8,   a9,   a10,  a11,  a12,  a13,  a14,  a15,     \
                        a16,  a17,  a18,  a19,  a20,  a21,  a22,  a23,     \
                        a24,  a25,  a26,  a27,  a28,  a29,  a30,  a31,     \
                        a32,  a33,  a34,  a35,  a36,  a37,  a38,  a39,     \
                        a40,  a41,  a42,  a43,  a44,  a45,  a46,  a47,     \
                        a48,  a49,  a50,  a51,  a52,  a53,  a54,  a55,     \
                        a56,  a57,  a58,  a59,  a60,  a61,  a62,  a63,     \
                        a64,  a65,  a66,  a67,  a68,  a69,  a70,  a71,     \
                        a72,  a73,  a74,  a75,  a76,  a77,  a78,  a79,     \
                        a80,  a81,  a82,  a83,  a84,  a85,  a86,  a87,     \
                        a88,  a89,  a90,  a91,  a92,  a93,  a94,  a95,     \
                        a96,  a97,  a98,  a99,  a100, a101, a102, a103,    \
                        a104, a105, a106, a107, a108, a109, a110, a111,    \
                        a112, a113, a114, a115, a116, a117, a118, a119,    \
                        a120, a121, a122, a123, a124, a125, a126, a127,    \
                        a128, ...)  a128

#define cldm_count_expand(...) cldm_count_pick(__VA_ARGS__)
#define cldm_count(...) cldm_count_expand(__VA_ARGS__, cldm_count_sequence)

#define cldm_for_each4(iter, array, size, step)                             \
    for(unsigned cldm_cat_expand(cldm_fe,__LINE__) = ((iter) = (array), 0); \
        cldm_cat_expand(cldm_fe,__LINE__) < (unsigned)(size);               \
        cldm_cat_expand(cldm_fe,__LINE__) += (step),                        \
        (iter) = &(array)[cldm_cat_expand(cldm_fe,__LINE__)])

#define cldm_for_each3(iter, array, size)                                   \
    cldm_for_each4(iter, array, size, 1)

#define cldm_for_each2(iter, array)                                         \
    cldm_for_each3(iter, array, cldm_arrsize(array))

#define cldm_for_each(...)                                                  \
    cldm_overload(cldm_for_each,__VA_ARGS__)

#define cldm_for_each_zip6(it0, it1, arr0, arr1, size, step)                                \
    for(unsigned cldm_cat_expand(cldm_fez,__LINE__) = ((it0) = (arr0), (it1) = (arr1), 0);  \
        cldm_cat_expand(cldm_fez,__LINE__) < (unsigned)(size);                              \
        cldm_cat_expand(cldm_fez,__LINE__) += (step),                                       \
        (it0) = &(arr0[cldm_cat_expand(cldm_fez,__LINE__)]),                                \
        (it1) = &(arr1[cldm_cat_expand(cldm_fez,__LINE__)]))

#define cldm_for_each_zip5(it0, it1, arr0, arr1, size)                                      \
    cldm_for_each_zip6(it0, it1, arr0, arr1, size, 1)

#define cldm_for_each_zip4(it0, it1, arr0, arr1)                                            \
    cldm_for_each_zip5(it0, it1, arr0, arr1, cldm_arrsize(arr0) < cldm_arrsize(arr1) ?      \
                                             cldm_arrsize(arr0) : cldm_arrsize(arr1))

#define cldm_for_each_zip(...)                                                              \
    cldm_overload(cldm_for_each_zip,__VA_ARGS__)

#define cldm_nothing()
#define cldm_block(x) x cldm_nothing()

#define cldm_map_recurse1(...)
#define cldm_defer_map_recursion() cldm_map_recurse
#define cldm_map_recurse0(ctr, m, separator, ...)           \
    separator cldm_block(cldm_defer_map_recursion)()(cldm_inc(ctr), m, separator, __VA_ARGS__)

#define cldm_map_recurse(ctr, m, separator, first, ...)     \
    m(ctr, first)                                           \
    cldm_cat_expand(cldm_map_recurse,cldm_token_1(cldm_count(__VA_ARGS__)))(ctr, m, separator, __VA_ARGS__)

#define cldm_map_separate(m, separator, ...)                \
    cldm_cat_expand(cldm_expand, cldm_count(__VA_ARGS__))(cldm_map_recurse(0, m, separator, __VA_ARGS__,))

#define cldm_map_list_recurse1(...)
#define cldm_defer_map_list_recursion() cldm_map_list_recurse
#define cldm_map_list_recurse0(ctr, m, ...)                 \
    ,cldm_block(cldm_defer_map_list_recursion)()(cldm_inc(ctr), m, __VA_ARGS__)

#define cldm_map_list_recurse(ctr, m, first, ...)           \
    m(ctr, first)                                           \
    cldm_cat_expand(cldm_map_list_recurse,cldm_token_1(cldm_count(__VA_ARGS__)))(ctr, m, __VA_ARGS__)

#define cldm_map_list(m, ...)                               \
    cldm_cat_expand(cldm_expand, cldm_count(__VA_ARGS__))(cldm_map_list_recurse(0, m, __VA_ARGS__,))

#define cldm_map(m, ...)                                    \
    cldm_cat_expand(cldm_expand, cldm_count(__VA_ARGS__))(cldm_map_recurse(0, m,, __VA_ARGS__,))

#define cldm_inc(num) cldm_cat(cldm_inc_,num)
#define cldm_dec(num) cldm_cat(cldm_dec_,num)

#define cldm_expand(...)    __VA_ARGS__
#define cldm_expand1(...)   __VA_ARGS__
#define cldm_expand2(...)   cldm_expand(__VA_ARGS__)
#define cldm_expand3(...)   cldm_expand2(__VA_ARGS__)
#define cldm_expand4(...)   cldm_expand3(__VA_ARGS__)
#define cldm_expand5(...)   cldm_expand4(__VA_ARGS__)
#define cldm_expand6(...)   cldm_expand5(__VA_ARGS__)
#define cldm_expand7(...)   cldm_expand6(__VA_ARGS__)
#define cldm_expand8(...)   cldm_expand7(__VA_ARGS__)
#define cldm_expand9(...)   cldm_expand8(__VA_ARGS__)
#define cldm_expand10(...)  cldm_expand9(__VA_ARGS__)
#define cldm_expand11(...)  cldm_expand10(__VA_ARGS__)
#define cldm_expand12(...)  cldm_expand11(__VA_ARGS__)
#define cldm_expand13(...)  cldm_expand12(__VA_ARGS__)
#define cldm_expand14(...)  cldm_expand13(__VA_ARGS__)
#define cldm_expand15(...)  cldm_expand14(__VA_ARGS__)
#define cldm_expand16(...)  cldm_expand15(__VA_ARGS__)
#define cldm_expand17(...)  cldm_expand16(__VA_ARGS__)
#define cldm_expand18(...)  cldm_expand17(__VA_ARGS__)
#define cldm_expand19(...)  cldm_expand18(__VA_ARGS__)
#define cldm_expand20(...)  cldm_expand19(__VA_ARGS__)
#define cldm_expand21(...)  cldm_expand20(__VA_ARGS__)
#define cldm_expand22(...)  cldm_expand21(__VA_ARGS__)
#define cldm_expand23(...)  cldm_expand22(__VA_ARGS__)
#define cldm_expand24(...)  cldm_expand23(__VA_ARGS__)
#define cldm_expand25(...)  cldm_expand24(__VA_ARGS__)
#define cldm_expand26(...)  cldm_expand25(__VA_ARGS__)
#define cldm_expand27(...)  cldm_expand26(__VA_ARGS__)
#define cldm_expand28(...)  cldm_expand27(__VA_ARGS__)
#define cldm_expand29(...)  cldm_expand28(__VA_ARGS__)
#define cldm_expand30(...)  cldm_expand29(__VA_ARGS__)
#define cldm_expand31(...)  cldm_expand30(__VA_ARGS__)
#define cldm_expand32(...)  cldm_expand31(__VA_ARGS__)
#define cldm_expand33(...)  cldm_expand32(__VA_ARGS__)
#define cldm_expand34(...)  cldm_expand33(__VA_ARGS__)
#define cldm_expand35(...)  cldm_expand34(__VA_ARGS__)
#define cldm_expand36(...)  cldm_expand35(__VA_ARGS__)
#define cldm_expand37(...)  cldm_expand36(__VA_ARGS__)
#define cldm_expand38(...)  cldm_expand37(__VA_ARGS__)
#define cldm_expand39(...)  cldm_expand38(__VA_ARGS__)
#define cldm_expand40(...)  cldm_expand39(__VA_ARGS__)
#define cldm_expand41(...)  cldm_expand40(__VA_ARGS__)
#define cldm_expand42(...)  cldm_expand41(__VA_ARGS__)
#define cldm_expand43(...)  cldm_expand42(__VA_ARGS__)
#define cldm_expand44(...)  cldm_expand43(__VA_ARGS__)
#define cldm_expand45(...)  cldm_expand44(__VA_ARGS__)
#define cldm_expand46(...)  cldm_expand45(__VA_ARGS__)
#define cldm_expand47(...)  cldm_expand46(__VA_ARGS__)
#define cldm_expand48(...)  cldm_expand47(__VA_ARGS__)
#define cldm_expand49(...)  cldm_expand48(__VA_ARGS__)
#define cldm_expand50(...)  cldm_expand49(__VA_ARGS__)
#define cldm_expand51(...)  cldm_expand50(__VA_ARGS__)
#define cldm_expand52(...)  cldm_expand51(__VA_ARGS__)
#define cldm_expand53(...)  cldm_expand52(__VA_ARGS__)
#define cldm_expand54(...)  cldm_expand53(__VA_ARGS__)
#define cldm_expand55(...)  cldm_expand54(__VA_ARGS__)
#define cldm_expand56(...)  cldm_expand55(__VA_ARGS__)
#define cldm_expand57(...)  cldm_expand56(__VA_ARGS__)
#define cldm_expand58(...)  cldm_expand57(__VA_ARGS__)
#define cldm_expand59(...)  cldm_expand58(__VA_ARGS__)
#define cldm_expand60(...)  cldm_expand59(__VA_ARGS__)
#define cldm_expand61(...)  cldm_expand60(__VA_ARGS__)
#define cldm_expand62(...)  cldm_expand61(__VA_ARGS__)
#define cldm_expand63(...)  cldm_expand62(__VA_ARGS__)
#define cldm_expand64(...)  cldm_expand63(__VA_ARGS__)
#define cldm_expand65(...)  cldm_expand64(__VA_ARGS__)
#define cldm_expand66(...)  cldm_expand65(__VA_ARGS__)
#define cldm_expand67(...)  cldm_expand66(__VA_ARGS__)
#define cldm_expand68(...)  cldm_expand67(__VA_ARGS__)
#define cldm_expand69(...)  cldm_expand68(__VA_ARGS__)
#define cldm_expand70(...)  cldm_expand69(__VA_ARGS__)
#define cldm_expand71(...)  cldm_expand70(__VA_ARGS__)
#define cldm_expand72(...)  cldm_expand71(__VA_ARGS__)
#define cldm_expand73(...)  cldm_expand72(__VA_ARGS__)
#define cldm_expand74(...)  cldm_expand73(__VA_ARGS__)
#define cldm_expand75(...)  cldm_expand74(__VA_ARGS__)
#define cldm_expand76(...)  cldm_expand75(__VA_ARGS__)
#define cldm_expand77(...)  cldm_expand76(__VA_ARGS__)
#define cldm_expand78(...)  cldm_expand77(__VA_ARGS__)
#define cldm_expand79(...)  cldm_expand78(__VA_ARGS__)
#define cldm_expand80(...)  cldm_expand79(__VA_ARGS__)
#define cldm_expand81(...)  cldm_expand80(__VA_ARGS__)
#define cldm_expand82(...)  cldm_expand81(__VA_ARGS__)
#define cldm_expand83(...)  cldm_expand82(__VA_ARGS__)
#define cldm_expand84(...)  cldm_expand83(__VA_ARGS__)
#define cldm_expand85(...)  cldm_expand84(__VA_ARGS__)
#define cldm_expand86(...)  cldm_expand85(__VA_ARGS__)
#define cldm_expand87(...)  cldm_expand86(__VA_ARGS__)
#define cldm_expand88(...)  cldm_expand87(__VA_ARGS__)
#define cldm_expand89(...)  cldm_expand88(__VA_ARGS__)
#define cldm_expand90(...)  cldm_expand89(__VA_ARGS__)
#define cldm_expand91(...)  cldm_expand90(__VA_ARGS__)
#define cldm_expand92(...)  cldm_expand91(__VA_ARGS__)
#define cldm_expand93(...)  cldm_expand92(__VA_ARGS__)
#define cldm_expand94(...)  cldm_expand93(__VA_ARGS__)
#define cldm_expand95(...)  cldm_expand94(__VA_ARGS__)
#define cldm_expand96(...)  cldm_expand95(__VA_ARGS__)
#define cldm_expand97(...)  cldm_expand96(__VA_ARGS__)
#define cldm_expand98(...)  cldm_expand97(__VA_ARGS__)
#define cldm_expand99(...)  cldm_expand98(__VA_ARGS__)
#define cldm_expand100(...) cldm_expand99(__VA_ARGS__)
#define cldm_expand101(...) cldm_expand100(__VA_ARGS__)
#define cldm_expand102(...) cldm_expand101(__VA_ARGS__)
#define cldm_expand103(...) cldm_expand102(__VA_ARGS__)
#define cldm_expand104(...) cldm_expand103(__VA_ARGS__)
#define cldm_expand105(...) cldm_expand104(__VA_ARGS__)
#define cldm_expand106(...) cldm_expand105(__VA_ARGS__)
#define cldm_expand107(...) cldm_expand106(__VA_ARGS__)
#define cldm_expand108(...) cldm_expand107(__VA_ARGS__)
#define cldm_expand109(...) cldm_expand108(__VA_ARGS__)
#define cldm_expand110(...) cldm_expand109(__VA_ARGS__)
#define cldm_expand111(...) cldm_expand110(__VA_ARGS__)
#define cldm_expand112(...) cldm_expand111(__VA_ARGS__)
#define cldm_expand113(...) cldm_expand112(__VA_ARGS__)
#define cldm_expand114(...) cldm_expand113(__VA_ARGS__)
#define cldm_expand115(...) cldm_expand114(__VA_ARGS__)
#define cldm_expand116(...) cldm_expand115(__VA_ARGS__)
#define cldm_expand117(...) cldm_expand116(__VA_ARGS__)
#define cldm_expand118(...) cldm_expand117(__VA_ARGS__)
#define cldm_expand119(...) cldm_expand118(__VA_ARGS__)
#define cldm_expand120(...) cldm_expand119(__VA_ARGS__)
#define cldm_expand121(...) cldm_expand120(__VA_ARGS__)
#define cldm_expand122(...) cldm_expand121(__VA_ARGS__)
#define cldm_expand123(...) cldm_expand122(__VA_ARGS__)
#define cldm_expand124(...) cldm_expand123(__VA_ARGS__)
#define cldm_expand125(...) cldm_expand124(__VA_ARGS__)
#define cldm_expand126(...) cldm_expand125(__VA_ARGS__)
#define cldm_expand127(...) cldm_expand126(__VA_ARGS__)
#define cldm_expand128(...) cldm_expand127(__VA_ARGS__)

#define cldm_inc_0   1
#define cldm_inc_1   2
#define cldm_inc_2   3
#define cldm_inc_3   4
#define cldm_inc_4   5
#define cldm_inc_5   6
#define cldm_inc_6   7
#define cldm_inc_7   8
#define cldm_inc_8   9
#define cldm_inc_9   10
#define cldm_inc_10  11
#define cldm_inc_11  12
#define cldm_inc_12  13
#define cldm_inc_13  14
#define cldm_inc_14  15
#define cldm_inc_15  16
#define cldm_inc_16  17
#define cldm_inc_17  18
#define cldm_inc_18  19
#define cldm_inc_19  20
#define cldm_inc_20  21
#define cldm_inc_21  22
#define cldm_inc_22  23
#define cldm_inc_23  24
#define cldm_inc_24  25
#define cldm_inc_25  26
#define cldm_inc_26  27
#define cldm_inc_27  28
#define cldm_inc_28  29
#define cldm_inc_29  30
#define cldm_inc_30  31
#define cldm_inc_31  32
#define cldm_inc_32  33
#define cldm_inc_33  34
#define cldm_inc_34  35
#define cldm_inc_35  36
#define cldm_inc_36  37
#define cldm_inc_37  38
#define cldm_inc_38  39
#define cldm_inc_39  40
#define cldm_inc_40  41
#define cldm_inc_41  42
#define cldm_inc_42  43
#define cldm_inc_43  44
#define cldm_inc_44  45
#define cldm_inc_45  46
#define cldm_inc_46  47
#define cldm_inc_47  48
#define cldm_inc_48  49
#define cldm_inc_49  50
#define cldm_inc_50  51
#define cldm_inc_51  52
#define cldm_inc_52  53
#define cldm_inc_53  54
#define cldm_inc_54  55
#define cldm_inc_55  56
#define cldm_inc_56  57
#define cldm_inc_57  58
#define cldm_inc_58  59
#define cldm_inc_59  60
#define cldm_inc_60  61
#define cldm_inc_61  62
#define cldm_inc_62  63
#define cldm_inc_63  64
#define cldm_inc_64  65
#define cldm_inc_65  66
#define cldm_inc_66  67
#define cldm_inc_67  68
#define cldm_inc_68  69
#define cldm_inc_69  70
#define cldm_inc_70  71
#define cldm_inc_71  72
#define cldm_inc_72  73
#define cldm_inc_73  74
#define cldm_inc_74  75
#define cldm_inc_75  76
#define cldm_inc_76  77
#define cldm_inc_77  78
#define cldm_inc_78  79
#define cldm_inc_79  80
#define cldm_inc_80  81
#define cldm_inc_81  82
#define cldm_inc_82  83
#define cldm_inc_83  84
#define cldm_inc_84  85
#define cldm_inc_85  86
#define cldm_inc_86  87
#define cldm_inc_87  88
#define cldm_inc_88  89
#define cldm_inc_89  90
#define cldm_inc_90  91
#define cldm_inc_91  92
#define cldm_inc_92  93
#define cldm_inc_93  94
#define cldm_inc_94  95
#define cldm_inc_95  96
#define cldm_inc_96  97
#define cldm_inc_97  98
#define cldm_inc_98  99
#define cldm_inc_99  100
#define cldm_inc_100 101
#define cldm_inc_101 102
#define cldm_inc_102 103
#define cldm_inc_103 104
#define cldm_inc_104 105
#define cldm_inc_105 106
#define cldm_inc_106 107
#define cldm_inc_107 108
#define cldm_inc_108 109
#define cldm_inc_109 110
#define cldm_inc_110 111
#define cldm_inc_111 112
#define cldm_inc_112 113
#define cldm_inc_113 114
#define cldm_inc_114 115
#define cldm_inc_115 116
#define cldm_inc_116 117
#define cldm_inc_117 118
#define cldm_inc_118 119
#define cldm_inc_119 120
#define cldm_inc_120 121
#define cldm_inc_121 122
#define cldm_inc_122 123
#define cldm_inc_123 124
#define cldm_inc_124 125
#define cldm_inc_125 126
#define cldm_inc_126 127

#define cldm_dec_1   0
#define cldm_dec_2   1
#define cldm_dec_3   2
#define cldm_dec_4   3
#define cldm_dec_5   4
#define cldm_dec_6   5
#define cldm_dec_7   6
#define cldm_dec_8   7
#define cldm_dec_9   8
#define cldm_dec_10  9
#define cldm_dec_11  10
#define cldm_dec_12  11
#define cldm_dec_13  12
#define cldm_dec_14  13
#define cldm_dec_15  14
#define cldm_dec_16  15
#define cldm_dec_17  16
#define cldm_dec_18  17
#define cldm_dec_19  18
#define cldm_dec_20  19
#define cldm_dec_21  20
#define cldm_dec_22  21
#define cldm_dec_23  22
#define cldm_dec_24  23
#define cldm_dec_25  24
#define cldm_dec_26  25
#define cldm_dec_27  26
#define cldm_dec_28  27
#define cldm_dec_29  28
#define cldm_dec_30  29
#define cldm_dec_31  30
#define cldm_dec_32  31
#define cldm_dec_33  32
#define cldm_dec_34  33
#define cldm_dec_35  34
#define cldm_dec_36  35
#define cldm_dec_37  36
#define cldm_dec_38  37
#define cldm_dec_39  38
#define cldm_dec_40  39
#define cldm_dec_41  40
#define cldm_dec_42  41
#define cldm_dec_43  42
#define cldm_dec_44  43
#define cldm_dec_45  44
#define cldm_dec_46  45
#define cldm_dec_47  46
#define cldm_dec_48  47
#define cldm_dec_49  48
#define cldm_dec_50  49
#define cldm_dec_51  50
#define cldm_dec_52  51
#define cldm_dec_53  52
#define cldm_dec_54  53
#define cldm_dec_55  54
#define cldm_dec_56  55
#define cldm_dec_57  56
#define cldm_dec_58  57
#define cldm_dec_59  58
#define cldm_dec_60  59
#define cldm_dec_61  60
#define cldm_dec_62  61
#define cldm_dec_63  62
#define cldm_dec_64  63
#define cldm_dec_65  64
#define cldm_dec_66  65
#define cldm_dec_67  66
#define cldm_dec_68  67
#define cldm_dec_69  68
#define cldm_dec_70  69
#define cldm_dec_71  70
#define cldm_dec_72  71
#define cldm_dec_73  72
#define cldm_dec_74  73
#define cldm_dec_75  74
#define cldm_dec_76  75
#define cldm_dec_77  76
#define cldm_dec_78  77
#define cldm_dec_79  78
#define cldm_dec_80  79
#define cldm_dec_81  80
#define cldm_dec_82  81
#define cldm_dec_83  82
#define cldm_dec_84  83
#define cldm_dec_85  84
#define cldm_dec_86  85
#define cldm_dec_87  86
#define cldm_dec_88  87
#define cldm_dec_89  88
#define cldm_dec_90  89
#define cldm_dec_91  90
#define cldm_dec_92  91
#define cldm_dec_93  92
#define cldm_dec_94  93
#define cldm_dec_95  94
#define cldm_dec_96  95
#define cldm_dec_97  96
#define cldm_dec_98  97
#define cldm_dec_99  98
#define cldm_dec_100 99
#define cldm_dec_101 100
#define cldm_dec_102 101
#define cldm_dec_103 102
#define cldm_dec_104 103
#define cldm_dec_105 104
#define cldm_dec_106 105
#define cldm_dec_107 106
#define cldm_dec_108 107
#define cldm_dec_109 108
#define cldm_dec_110 109
#define cldm_dec_111 110
#define cldm_dec_112 111
#define cldm_dec_113 112
#define cldm_dec_114 113
#define cldm_dec_115 114
#define cldm_dec_116 115
#define cldm_dec_117 116
#define cldm_dec_118 117
#define cldm_dec_119 118
#define cldm_dec_120 119
#define cldm_dec_121 120
#define cldm_dec_122 121
#define cldm_dec_123 122
#define cldm_dec_124 123
#define cldm_dec_125 124
#define cldm_dec_126 125
#define cldm_dec_127 126

#endif /* CLDM_MACRO_H */
