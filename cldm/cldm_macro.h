#ifndef CLDM_MACRO_H
#define CLDM_MACRO_H

#define cldm_cat(a, b) a ## b
#define cldm_cat_expand(a,b) cldm_cat(a,b)

#define cldm_str(a) #a
#define cldm_str_expand(a) cldm_str(a)

#define cldm_expand(a) a

#define cldm_arrsize(arr) (sizeof(arr) / sizeof(arr[0]))

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
        cldm_cat_expand(cldm_fe,__LINE__) < (size);                         \
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
        cldm_cat_expand(cldm_fez,__LINE__) < (size);                                        \
        cldm_cat_expand(cldm_fez,__LINE__) += (step),                                       \
        (it0) = &(arr0[cldm_cat_expand(cldm_fez,__LINE__)]),                                \
        (it1) = &(arr1[cldm_cat_expand(cldm_fez,__LINE__)]))

#define cldm_for_each_zip5(it0, it1, arr0, arr1, size)                                      \
    cldm_for_each_zip6(it0, it1, arr0, arr1, size, 1)

#define cldm_for_each_zip4(it0, it1, arr0, arr1)                                            \
    cldm_for_each_zip5(it0, it1, arr0, arr1, cldm_arrsize(arr0))

#define cldm_for_each_zip(...)                                                              \
    cldm_overload(cldm_for_each_zip,__VA_ARGS__)

#endif /* CLDM_MACRO_H */
