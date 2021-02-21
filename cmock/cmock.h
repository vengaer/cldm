#ifndef CMOCK_H
#define CMOCK_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dlfcn.h>

#define cmock_count_sequence 128, 127, 126, 125, 124, 123, 122, 121,    \
                             120, 119, 118, 117, 116, 115, 114, 113,    \
                             112, 111, 110, 109, 108, 107, 106, 105,    \
                             104, 103, 102, 101, 100, 99,  98,  97,     \
                             96,  95,  94,  93,  92,  91,  90,  89,     \
                             88,  87,  86,  85,  84,  83,  82,  81,     \
                             80,  79,  78,  77,  76,  75,  74,  73,     \
                             72,  71,  70,  69,  68,  67,  66,  65,     \
                             64,  63,  62,  61,  60,  59,  58,  57,     \
                             56,  55,  54,  53,  52,  51,  50,  49,     \
                             48,  47,  46,  45,  44,  43,  42,  41,     \
                             40,  39,  38,  37,  36,  35,  34,  33,     \
                             32,  31,  30,  29,  28,  27,  26,  25,     \
                             24,  23,  22,  21,  20,  19,  18,  17,     \
                             16,  15,  14,  13,  12,  11,  10,  9,      \
                             8,   7,   6,   5,   4,   3,   2,   1,      \
                             0

#define cmock_count_pick(a0,   a1,   a2,   a3,   a4,   a5,   a6,   a7,      \
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

#define cmock_count_expand(...) cmock_count_pick(__VA_ARGS__)
#define cmock_count(...) cmock_count_expand(__VA_ARGS__, cmock_count_sequence)

#define cmock_cat(a, b) a ## b
#define cmock_cat_expand(a,b) cmock_cat(a,b)

#define cmock_str(a) #a
#define cmock_str_expand(a) cmock_str(a)

#define cmock_genparams_1(type)        type a1
#define cmock_genparams_2(type, ...)   type a2,   cmock_genparams_1(__VA_ARGS__)
#define cmock_genparams_3(type, ...)   type a3,   cmock_genparams_2(__VA_ARGS__)
#define cmock_genparams_4(type, ...)   type a4,   cmock_genparams_3(__VA_ARGS__)
#define cmock_genparams_5(type, ...)   type a5,   cmock_genparams_4(__VA_ARGS__)
#define cmock_genparams_6(type, ...)   type a6,   cmock_genparams_5(__VA_ARGS__)
#define cmock_genparams_7(type, ...)   type a7,   cmock_genparams_6(__VA_ARGS__)
#define cmock_genparams_8(type, ...)   type a8,   cmock_genparams_7(__VA_ARGS__)
#define cmock_genparams_9(type, ...)   type a9,   cmock_genparams_8(__VA_ARGS__)
#define cmock_genparams_10(type, ...)  type a10,  cmock_genparams_9(__VA_ARGS__)
#define cmock_genparams_11(type, ...)  type a11,  cmock_genparams_10(__VA_ARGS__)
#define cmock_genparams_12(type, ...)  type a12,  cmock_genparams_11(__VA_ARGS__)
#define cmock_genparams_13(type, ...)  type a13,  cmock_genparams_12(__VA_ARGS__)
#define cmock_genparams_14(type, ...)  type a14,  cmock_genparams_13(__VA_ARGS__)
#define cmock_genparams_15(type, ...)  type a15,  cmock_genparams_14(__VA_ARGS__)
#define cmock_genparams_16(type, ...)  type a16,  cmock_genparams_15(__VA_ARGS__)
#define cmock_genparams_17(type, ...)  type a17,  cmock_genparams_16(__VA_ARGS__)
#define cmock_genparams_18(type, ...)  type a18,  cmock_genparams_17(__VA_ARGS__)
#define cmock_genparams_19(type, ...)  type a19,  cmock_genparams_18(__VA_ARGS__)
#define cmock_genparams_20(type, ...)  type a20,  cmock_genparams_19(__VA_ARGS__)
#define cmock_genparams_21(type, ...)  type a21,  cmock_genparams_20(__VA_ARGS__)
#define cmock_genparams_22(type, ...)  type a22,  cmock_genparams_21(__VA_ARGS__)
#define cmock_genparams_23(type, ...)  type a23,  cmock_genparams_22(__VA_ARGS__)
#define cmock_genparams_24(type, ...)  type a24,  cmock_genparams_23(__VA_ARGS__)
#define cmock_genparams_25(type, ...)  type a25,  cmock_genparams_24(__VA_ARGS__)
#define cmock_genparams_26(type, ...)  type a26,  cmock_genparams_25(__VA_ARGS__)
#define cmock_genparams_27(type, ...)  type a27,  cmock_genparams_26(__VA_ARGS__)
#define cmock_genparams_28(type, ...)  type a28,  cmock_genparams_27(__VA_ARGS__)
#define cmock_genparams_29(type, ...)  type a29,  cmock_genparams_28(__VA_ARGS__)
#define cmock_genparams_30(type, ...)  type a30,  cmock_genparams_29(__VA_ARGS__)
#define cmock_genparams_31(type, ...)  type a31,  cmock_genparams_30(__VA_ARGS__)
#define cmock_genparams_32(type, ...)  type a32,  cmock_genparams_31(__VA_ARGS__)
#define cmock_genparams_33(type, ...)  type a33,  cmock_genparams_32(__VA_ARGS__)
#define cmock_genparams_34(type, ...)  type a34,  cmock_genparams_33(__VA_ARGS__)
#define cmock_genparams_35(type, ...)  type a35,  cmock_genparams_34(__VA_ARGS__)
#define cmock_genparams_36(type, ...)  type a36,  cmock_genparams_35(__VA_ARGS__)
#define cmock_genparams_37(type, ...)  type a37,  cmock_genparams_36(__VA_ARGS__)
#define cmock_genparams_38(type, ...)  type a38,  cmock_genparams_37(__VA_ARGS__)
#define cmock_genparams_39(type, ...)  type a39,  cmock_genparams_38(__VA_ARGS__)
#define cmock_genparams_40(type, ...)  type a40,  cmock_genparams_39(__VA_ARGS__)
#define cmock_genparams_41(type, ...)  type a41,  cmock_genparams_40(__VA_ARGS__)
#define cmock_genparams_42(type, ...)  type a42,  cmock_genparams_41(__VA_ARGS__)
#define cmock_genparams_43(type, ...)  type a43,  cmock_genparams_42(__VA_ARGS__)
#define cmock_genparams_44(type, ...)  type a44,  cmock_genparams_43(__VA_ARGS__)
#define cmock_genparams_45(type, ...)  type a45,  cmock_genparams_44(__VA_ARGS__)
#define cmock_genparams_46(type, ...)  type a46,  cmock_genparams_45(__VA_ARGS__)
#define cmock_genparams_47(type, ...)  type a47,  cmock_genparams_46(__VA_ARGS__)
#define cmock_genparams_48(type, ...)  type a48,  cmock_genparams_47(__VA_ARGS__)
#define cmock_genparams_49(type, ...)  type a49,  cmock_genparams_48(__VA_ARGS__)
#define cmock_genparams_50(type, ...)  type a50,  cmock_genparams_49(__VA_ARGS__)
#define cmock_genparams_51(type, ...)  type a51,  cmock_genparams_50(__VA_ARGS__)
#define cmock_genparams_52(type, ...)  type a52,  cmock_genparams_51(__VA_ARGS__)
#define cmock_genparams_53(type, ...)  type a53,  cmock_genparams_52(__VA_ARGS__)
#define cmock_genparams_54(type, ...)  type a54,  cmock_genparams_53(__VA_ARGS__)
#define cmock_genparams_55(type, ...)  type a55,  cmock_genparams_54(__VA_ARGS__)
#define cmock_genparams_56(type, ...)  type a56,  cmock_genparams_55(__VA_ARGS__)
#define cmock_genparams_57(type, ...)  type a57,  cmock_genparams_56(__VA_ARGS__)
#define cmock_genparams_58(type, ...)  type a58,  cmock_genparams_57(__VA_ARGS__)
#define cmock_genparams_59(type, ...)  type a59,  cmock_genparams_58(__VA_ARGS__)
#define cmock_genparams_60(type, ...)  type a60,  cmock_genparams_59(__VA_ARGS__)
#define cmock_genparams_61(type, ...)  type a61,  cmock_genparams_60(__VA_ARGS__)
#define cmock_genparams_62(type, ...)  type a62,  cmock_genparams_61(__VA_ARGS__)
#define cmock_genparams_63(type, ...)  type a63,  cmock_genparams_62(__VA_ARGS__)
#define cmock_genparams_64(type, ...)  type a64,  cmock_genparams_63(__VA_ARGS__)
#define cmock_genparams_65(type, ...)  type a65,  cmock_genparams_64(__VA_ARGS__)
#define cmock_genparams_66(type, ...)  type a66,  cmock_genparams_65(__VA_ARGS__)
#define cmock_genparams_67(type, ...)  type a67,  cmock_genparams_66(__VA_ARGS__)
#define cmock_genparams_68(type, ...)  type a68,  cmock_genparams_67(__VA_ARGS__)
#define cmock_genparams_69(type, ...)  type a69,  cmock_genparams_68(__VA_ARGS__)
#define cmock_genparams_70(type, ...)  type a70,  cmock_genparams_69(__VA_ARGS__)
#define cmock_genparams_71(type, ...)  type a71,  cmock_genparams_70(__VA_ARGS__)
#define cmock_genparams_72(type, ...)  type a72,  cmock_genparams_71(__VA_ARGS__)
#define cmock_genparams_73(type, ...)  type a73,  cmock_genparams_72(__VA_ARGS__)
#define cmock_genparams_74(type, ...)  type a74,  cmock_genparams_73(__VA_ARGS__)
#define cmock_genparams_75(type, ...)  type a75,  cmock_genparams_74(__VA_ARGS__)
#define cmock_genparams_76(type, ...)  type a76,  cmock_genparams_75(__VA_ARGS__)
#define cmock_genparams_77(type, ...)  type a77,  cmock_genparams_76(__VA_ARGS__)
#define cmock_genparams_78(type, ...)  type a78,  cmock_genparams_77(__VA_ARGS__)
#define cmock_genparams_79(type, ...)  type a79,  cmock_genparams_78(__VA_ARGS__)
#define cmock_genparams_80(type, ...)  type a80,  cmock_genparams_79(__VA_ARGS__)
#define cmock_genparams_81(type, ...)  type a81,  cmock_genparams_80(__VA_ARGS__)
#define cmock_genparams_82(type, ...)  type a82,  cmock_genparams_81(__VA_ARGS__)
#define cmock_genparams_83(type, ...)  type a83,  cmock_genparams_82(__VA_ARGS__)
#define cmock_genparams_84(type, ...)  type a84,  cmock_genparams_83(__VA_ARGS__)
#define cmock_genparams_85(type, ...)  type a85,  cmock_genparams_84(__VA_ARGS__)
#define cmock_genparams_86(type, ...)  type a86,  cmock_genparams_85(__VA_ARGS__)
#define cmock_genparams_87(type, ...)  type a87,  cmock_genparams_86(__VA_ARGS__)
#define cmock_genparams_88(type, ...)  type a88,  cmock_genparams_87(__VA_ARGS__)
#define cmock_genparams_89(type, ...)  type a89,  cmock_genparams_88(__VA_ARGS__)
#define cmock_genparams_90(type, ...)  type a90,  cmock_genparams_89(__VA_ARGS__)
#define cmock_genparams_91(type, ...)  type a91,  cmock_genparams_90(__VA_ARGS__)
#define cmock_genparams_92(type, ...)  type a92,  cmock_genparams_91(__VA_ARGS__)
#define cmock_genparams_93(type, ...)  type a93,  cmock_genparams_92(__VA_ARGS__)
#define cmock_genparams_94(type, ...)  type a94,  cmock_genparams_93(__VA_ARGS__)
#define cmock_genparams_95(type, ...)  type a95,  cmock_genparams_94(__VA_ARGS__)
#define cmock_genparams_96(type, ...)  type a96,  cmock_genparams_95(__VA_ARGS__)
#define cmock_genparams_97(type, ...)  type a97,  cmock_genparams_96(__VA_ARGS__)
#define cmock_genparams_98(type, ...)  type a98,  cmock_genparams_97(__VA_ARGS__)
#define cmock_genparams_99(type, ...)  type a99,  cmock_genparams_98(__VA_ARGS__)
#define cmock_genparams_100(type, ...) type a100, cmock_genparams_99(__VA_ARGS__)
#define cmock_genparams_101(type, ...) type a101, cmock_genparams_100(__VA_ARGS__)
#define cmock_genparams_102(type, ...) type a102, cmock_genparams_101(__VA_ARGS__)
#define cmock_genparams_103(type, ...) type a103, cmock_genparams_102(__VA_ARGS__)
#define cmock_genparams_104(type, ...) type a104, cmock_genparams_103(__VA_ARGS__)
#define cmock_genparams_105(type, ...) type a105, cmock_genparams_104(__VA_ARGS__)
#define cmock_genparams_106(type, ...) type a106, cmock_genparams_105(__VA_ARGS__)
#define cmock_genparams_107(type, ...) type a107, cmock_genparams_106(__VA_ARGS__)
#define cmock_genparams_108(type, ...) type a108, cmock_genparams_107(__VA_ARGS__)
#define cmock_genparams_109(type, ...) type a109, cmock_genparams_108(__VA_ARGS__)
#define cmock_genparams_110(type, ...) type a110, cmock_genparams_109(__VA_ARGS__)
#define cmock_genparams_111(type, ...) type a111, cmock_genparams_110(__VA_ARGS__)
#define cmock_genparams_112(type, ...) type a112, cmock_genparams_111(__VA_ARGS__)
#define cmock_genparams_113(type, ...) type a113, cmock_genparams_112(__VA_ARGS__)
#define cmock_genparams_114(type, ...) type a114, cmock_genparams_113(__VA_ARGS__)
#define cmock_genparams_115(type, ...) type a115, cmock_genparams_114(__VA_ARGS__)
#define cmock_genparams_116(type, ...) type a116, cmock_genparams_115(__VA_ARGS__)
#define cmock_genparams_117(type, ...) type a117, cmock_genparams_116(__VA_ARGS__)
#define cmock_genparams_118(type, ...) type a118, cmock_genparams_117(__VA_ARGS__)
#define cmock_genparams_119(type, ...) type a119, cmock_genparams_118(__VA_ARGS__)
#define cmock_genparams_120(type, ...) type a120, cmock_genparams_119(__VA_ARGS__)
#define cmock_genparams_121(type, ...) type a121, cmock_genparams_120(__VA_ARGS__)
#define cmock_genparams_122(type, ...) type a122, cmock_genparams_121(__VA_ARGS__)
#define cmock_genparams_123(type, ...) type a123, cmock_genparams_122(__VA_ARGS__)
#define cmock_genparams_124(type, ...) type a124, cmock_genparams_123(__VA_ARGS__)
#define cmock_genparams_125(type, ...) type a125, cmock_genparams_124(__VA_ARGS__)
#define cmock_genparams_126(type, ...) type a126, cmock_genparams_125(__VA_ARGS__)
#define cmock_genparams_127(type, ...) type a127, cmock_genparams_126(__VA_ARGS__)

#define cmock_genparams(...) cmock_cat_expand(cmock_genparams_,cmock_count(__VA_ARGS__))(__VA_ARGS__)

#define cmock_arglist_1   a1
#define cmock_arglist_2   a2,   cmock_arglist_1
#define cmock_arglist_3   a3,   cmock_arglist_2
#define cmock_arglist_4   a4,   cmock_arglist_3
#define cmock_arglist_5   a5,   cmock_arglist_4
#define cmock_arglist_6   a6,   cmock_arglist_5
#define cmock_arglist_7   a7,   cmock_arglist_6
#define cmock_arglist_8   a8,   cmock_arglist_7
#define cmock_arglist_9   a9,   cmock_arglist_8
#define cmock_arglist_10  a10,  cmock_arglist_9
#define cmock_arglist_11  a11,  cmock_arglist_10
#define cmock_arglist_12  a12,  cmock_arglist_11
#define cmock_arglist_13  a13,  cmock_arglist_12
#define cmock_arglist_14  a14,  cmock_arglist_13
#define cmock_arglist_15  a15,  cmock_arglist_14
#define cmock_arglist_16  a16,  cmock_arglist_15
#define cmock_arglist_17  a17,  cmock_arglist_16
#define cmock_arglist_18  a18,  cmock_arglist_17
#define cmock_arglist_19  a19,  cmock_arglist_18
#define cmock_arglist_20  a20,  cmock_arglist_19
#define cmock_arglist_21  a21,  cmock_arglist_20
#define cmock_arglist_22  a22,  cmock_arglist_21
#define cmock_arglist_23  a23,  cmock_arglist_22
#define cmock_arglist_24  a24,  cmock_arglist_23
#define cmock_arglist_25  a25,  cmock_arglist_24
#define cmock_arglist_26  a26,  cmock_arglist_25
#define cmock_arglist_27  a27,  cmock_arglist_26
#define cmock_arglist_28  a28,  cmock_arglist_27
#define cmock_arglist_29  a29,  cmock_arglist_28
#define cmock_arglist_30  a30,  cmock_arglist_29
#define cmock_arglist_31  a31,  cmock_arglist_30
#define cmock_arglist_32  a32,  cmock_arglist_31
#define cmock_arglist_33  a33,  cmock_arglist_32
#define cmock_arglist_34  a34,  cmock_arglist_33
#define cmock_arglist_35  a35,  cmock_arglist_34
#define cmock_arglist_36  a36,  cmock_arglist_35
#define cmock_arglist_37  a37,  cmock_arglist_36
#define cmock_arglist_38  a38,  cmock_arglist_37
#define cmock_arglist_39  a39,  cmock_arglist_38
#define cmock_arglist_40  a40,  cmock_arglist_39
#define cmock_arglist_41  a41,  cmock_arglist_40
#define cmock_arglist_42  a42,  cmock_arglist_41
#define cmock_arglist_43  a43,  cmock_arglist_42
#define cmock_arglist_44  a44,  cmock_arglist_43
#define cmock_arglist_45  a45,  cmock_arglist_44
#define cmock_arglist_46  a46,  cmock_arglist_45
#define cmock_arglist_47  a47,  cmock_arglist_46
#define cmock_arglist_48  a48,  cmock_arglist_47
#define cmock_arglist_49  a49,  cmock_arglist_48
#define cmock_arglist_50  a50,  cmock_arglist_49
#define cmock_arglist_51  a51,  cmock_arglist_50
#define cmock_arglist_52  a52,  cmock_arglist_51
#define cmock_arglist_53  a53,  cmock_arglist_52
#define cmock_arglist_54  a54,  cmock_arglist_53
#define cmock_arglist_55  a55,  cmock_arglist_54
#define cmock_arglist_56  a56,  cmock_arglist_55
#define cmock_arglist_57  a57,  cmock_arglist_56
#define cmock_arglist_58  a58,  cmock_arglist_57
#define cmock_arglist_59  a59,  cmock_arglist_58
#define cmock_arglist_60  a60,  cmock_arglist_59
#define cmock_arglist_61  a61,  cmock_arglist_60
#define cmock_arglist_62  a62,  cmock_arglist_61
#define cmock_arglist_63  a63,  cmock_arglist_62
#define cmock_arglist_64  a64,  cmock_arglist_63
#define cmock_arglist_65  a65,  cmock_arglist_64
#define cmock_arglist_66  a66,  cmock_arglist_65
#define cmock_arglist_67  a67,  cmock_arglist_66
#define cmock_arglist_68  a68,  cmock_arglist_67
#define cmock_arglist_69  a69,  cmock_arglist_68
#define cmock_arglist_70  a70,  cmock_arglist_69
#define cmock_arglist_71  a71,  cmock_arglist_70
#define cmock_arglist_72  a72,  cmock_arglist_71
#define cmock_arglist_73  a73,  cmock_arglist_72
#define cmock_arglist_74  a74,  cmock_arglist_73
#define cmock_arglist_75  a75,  cmock_arglist_74
#define cmock_arglist_76  a76,  cmock_arglist_75
#define cmock_arglist_77  a77,  cmock_arglist_76
#define cmock_arglist_78  a78,  cmock_arglist_77
#define cmock_arglist_79  a79,  cmock_arglist_78
#define cmock_arglist_80  a80,  cmock_arglist_79
#define cmock_arglist_81  a81,  cmock_arglist_80
#define cmock_arglist_82  a82,  cmock_arglist_81
#define cmock_arglist_83  a83,  cmock_arglist_82
#define cmock_arglist_84  a84,  cmock_arglist_83
#define cmock_arglist_85  a85,  cmock_arglist_84
#define cmock_arglist_86  a86,  cmock_arglist_85
#define cmock_arglist_87  a87,  cmock_arglist_86
#define cmock_arglist_88  a88,  cmock_arglist_87
#define cmock_arglist_89  a89,  cmock_arglist_88
#define cmock_arglist_90  a90,  cmock_arglist_89
#define cmock_arglist_91  a91,  cmock_arglist_90
#define cmock_arglist_92  a92,  cmock_arglist_91
#define cmock_arglist_93  a93,  cmock_arglist_92
#define cmock_arglist_94  a94,  cmock_arglist_93
#define cmock_arglist_95  a95,  cmock_arglist_94
#define cmock_arglist_96  a96,  cmock_arglist_95
#define cmock_arglist_97  a97,  cmock_arglist_96
#define cmock_arglist_98  a98,  cmock_arglist_97
#define cmock_arglist_99  a99,  cmock_arglist_98
#define cmock_arglist_100 a100, cmock_arglist_99
#define cmock_arglist_101 a101, cmock_arglist_100
#define cmock_arglist_102 a102, cmock_arglist_101
#define cmock_arglist_103 a103, cmock_arglist_102
#define cmock_arglist_104 a104, cmock_arglist_103
#define cmock_arglist_105 a105, cmock_arglist_104
#define cmock_arglist_106 a106, cmock_arglist_105
#define cmock_arglist_107 a107, cmock_arglist_106
#define cmock_arglist_108 a108, cmock_arglist_107
#define cmock_arglist_109 a109, cmock_arglist_108
#define cmock_arglist_110 a110, cmock_arglist_109
#define cmock_arglist_111 a111, cmock_arglist_110
#define cmock_arglist_112 a112, cmock_arglist_111
#define cmock_arglist_113 a113, cmock_arglist_112
#define cmock_arglist_114 a114, cmock_arglist_113
#define cmock_arglist_115 a115, cmock_arglist_114
#define cmock_arglist_116 a116, cmock_arglist_115
#define cmock_arglist_117 a117, cmock_arglist_116
#define cmock_arglist_118 a118, cmock_arglist_117
#define cmock_arglist_119 a119, cmock_arglist_118
#define cmock_arglist_120 a120, cmock_arglist_119
#define cmock_arglist_121 a121, cmock_arglist_120
#define cmock_arglist_122 a122, cmock_arglist_121
#define cmock_arglist_123 a123, cmock_arglist_122
#define cmock_arglist_124 a124, cmock_arglist_123
#define cmock_arglist_125 a125, cmock_arglist_124
#define cmock_arglist_126 a126, cmock_arglist_125
#define cmock_arglist_127 a127, cmock_arglist_126

#define cmock_arglist(size) cmock_cat(cmock_arglist_, size)

#define cmock_mock_function2(...)   cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function3(...)   cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function4(...)   cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function5(...)   cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function6(...)   cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function7(...)   cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function8(...)   cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function9(...)   cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function10(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function11(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function12(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function13(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function14(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function15(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function16(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function17(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function18(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function19(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function20(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function21(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function22(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function23(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function24(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function25(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function26(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function27(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function28(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function29(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function30(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function31(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function32(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function33(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function34(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function35(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function36(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function37(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function38(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function39(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function40(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function41(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function42(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function43(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function44(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function45(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function46(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function47(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function48(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function49(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function50(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function51(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function52(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function53(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function54(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function55(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function56(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function57(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function58(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function59(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function60(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function61(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function62(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function63(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function64(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function65(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function66(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function67(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function68(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function69(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function70(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function71(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function72(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function73(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function74(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function75(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function76(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function77(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function78(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function79(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function80(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function81(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function82(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function83(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function84(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function85(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function86(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function87(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function88(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function89(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function90(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function91(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function92(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function93(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function94(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function95(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function96(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function97(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function98(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function99(...)  cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function100(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function101(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function102(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function103(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function104(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function105(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function106(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function107(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function108(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function109(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function110(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function111(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function112(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function113(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function114(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function115(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function116(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function117(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function118(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function119(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function120(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function121(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function122(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function123(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function124(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function125(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function126(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function127(...) cmock_mock_function(__VA_ARGS__)
#define cmock_mock_function128(...) cmock_mock_function(__VA_ARGS__)

#define cmock_assert1(...)   cmock_assert_no_msg(__VA_ARGS__)
#define cmock_assert2(...)   cmock_assert_msg(__VA_ARGS__)
#define cmock_assert3(...)   cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert4(...)   cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert5(...)   cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert6(...)   cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert7(...)   cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert8(...)   cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert9(...)   cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert10(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert11(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert12(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert13(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert14(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert15(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert16(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert17(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert18(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert19(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert20(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert21(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert22(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert23(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert24(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert25(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert26(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert27(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert28(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert29(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert30(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert31(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert32(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert33(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert34(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert35(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert36(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert37(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert38(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert39(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert40(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert41(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert42(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert43(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert44(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert45(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert46(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert47(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert48(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert49(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert50(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert51(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert52(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert53(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert54(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert55(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert56(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert57(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert58(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert59(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert60(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert61(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert62(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert63(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert64(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert65(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert66(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert67(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert68(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert69(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert70(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert71(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert72(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert73(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert74(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert75(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert76(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert77(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert78(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert79(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert80(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert81(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert82(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert83(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert84(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert85(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert86(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert87(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert88(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert89(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert90(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert91(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert92(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert93(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert94(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert95(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert96(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert97(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert98(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert99(...)  cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert100(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert101(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert102(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert103(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert104(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert105(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert106(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert107(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert108(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert109(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert110(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert111(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert112(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert113(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert114(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert115(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert116(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert117(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert118(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert119(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert120(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert121(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert122(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert123(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert124(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert125(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert126(...) cmock_assert_variadic(__VA_ARGS__)
#define cmock_assert127(...) cmock_assert_variadic(__VA_ARGS__)

#define cmock_mock_function0_1(rettype, name) CMOCK_MOCK_FUNCTION(rettype, name)
#define cmock_mock_function0_2(rettype, name, _) CMOCK_MOCK_FUNCTION(rettype, name)

#define cmock_mock_function_void0_1(name) CMOCK_MOCK_FUNCTION_VOID(name)
#define cmock_mock_function_void0_2(name, _) CMOCK_MOCK_FUNCTION_VOID(name)


#define cmock_assert_trigger(condition, ...)    \
    do {                                        \
        if(!(condition)) {                      \
            fprintf(stderr, __VA_ARGS__);       \
            _Exit(1);                           \
        }                                       \
    } while (0)

#define cmock_assert_no_msg(condition)    \
    cmock_assert_trigger(condition, "%s:%d assertion '%s' failed\n", __FILE__, __LINE__, #condition)

#define cmock_assert_msg(condition, msg)  \
    cmock_assert_trigger(condition, "%s:%d assertion '%s' failed " msg "\n", __FILE__, __LINE__, #condition)

#define cmock_assert_variadic(condition, fmt, ...) \
    cmock_assert_trigger(condition, "%s:%d assertion '%s' failed " fmt "\n", __FILE__, __LINE__, #condition, __VA_ARGS__)

#define cmock_assert(...) \
    cmock_overload(cmock_assert, __VA_ARGS__)

#define cmock_overload(name, ...) \
    cmock_cat_expand(name, cmock_count(__VA_ARGS__))(__VA_ARGS__)

#define cmock_will(invocations, ...)                                                                    \
        __VA_ARGS__;                                                                                    \
        *(int *)((unsigned char *)mockinfo->addr + mockinfo->invocations_offset) = invocations;         \
        cmock_assert((*(int *)((unsigned char *)mockinfo->addr + mockinfo->invocations_offset)) > -2);  \
    } while (0)

#define cmock_set_opmode(mode)                                                              \
    enum cmock_opmode *cmock_cat_expand(cmock_opdata, __LINE__) =                           \
        (enum cmock_opmode *)((unsigned char*)mockinfo->addr + mockinfo->opmode_offset);    \
    *cmock_cat_expand(cmock_opdata, __LINE__) = mode

#define cmock_setop(field, value, mode)     \
    opdata.field = value;                   \
    cmock_set_opmode(mode)

struct cmock_mock_info {
    void *addr;
    unsigned invocations_offset;
    unsigned opmode_offset;
};

enum cmock_opmode {
    CMOCK_OP_INVOKE,
    CMOCK_OP_RETURN,
    CMOCK_OP_INCREMENT,
};

#define cmock_generate_mock_ctx(utype, rettype, name, ...)  \
    struct cmock_mock_ ## name ## _ctx {                    \
        struct cmock_mock_info info;                        \
        int invocations;                                    \
        struct cmock_mock_ ## name ## _opdata{              \
            enum cmock_opmode mode;                         \
            union {                                         \
                rettype(*invoke)(__VA_ARGS__);              \
                utype retval;                               \
                utype counter;                              \
            };                                              \
        } opdata;                                           \
    }

#ifdef CMOCK_GENERATE_SYMBOLS
#define cmock_mock_function(rvinit, call_prefix, retstatement, utype, rettype, name, ...)                   \
    cmock_generate_mock_ctx(utype, rettype, name, __VA_ARGS__);                                             \
    struct cmock_mock_ ## name ## _ctx cmock_mock_ ## name = {                                              \
        .info = {                                                                                           \
            .addr = &cmock_mock_ ## name,                                                                   \
            .invocations_offset = offsetof(struct cmock_mock_ ## name ## _ctx, invocations),                \
            .opmode_offset = offsetof(struct cmock_mock_ ## name ## _ctx, opdata) +                         \
                             offsetof(struct cmock_mock_ ## name ## _opdata, mode)                          \
        },                                                                                                  \
        .invocations = 0,                                                                                   \
        .opdata = {                                                                                         \
            .mode = CMOCK_OP_INVOKE,                                                                        \
            .invoke = 0                                                                                     \
        }                                                                                                   \
    };                                                                                                      \
    rettype name(cmock_genparams(__VA_ARGS__)) {                                                            \
        rvinit;                                                                                             \
        if(cmock_mock_ ## name.invocations) {                                                               \
            if(cmock_mock_ ## name.invocations != -1) {                                                     \
                --cmock_mock_ ## name.invocations;                                                          \
            }                                                                                               \
            switch(cmock_mock_ ## name.opdata.mode) {                                                       \
                case CMOCK_OP_INVOKE:                                                                       \
                    call_prefix cmock_mock_ ## name.opdata.invoke(cmock_arglist(cmock_count(__VA_ARGS__))); \
                    retstatement;                                                                           \
                    break;                                                                                  \
                case CMOCK_OP_RETURN:                                                                       \
                    call_prefix cmock_mock_ ## name.opdata.retval;                                          \
                    retstatement;                                                                           \
                    break;                                                                                  \
                case CMOCK_OP_INCREMENT:                                                                    \
                    call_prefix ++cmock_mock_ ## name.opdata.counter;                                       \
                    retstatement;                                                                           \
                    break;                                                                                  \
                default:                                                                                    \
                    cmock_assert(0, "Invalid opmode %d", cmock_mock_ ## name.opdata.mode);                  \
            }                                                                                               \
        }                                                                                                   \
        extern void *cmock_symbol(char const*);                                                             \
        extern void cmock_close_dlhandle(void);                                                             \
        rettype(* cmock_handle_ ## name)(__VA_ARGS__);                                                      \
        dlerror();                                                                                          \
        *(void **) (& cmock_handle_ ## name) = cmock_symbol(#name);                                         \
        char const *cmock_error_ ## name = dlerror();                                                       \
        cmock_assert(!cmock_error_ ## name, "%s", cmock_error_ ## name);                                    \
        call_prefix cmock_handle_ ## name(cmock_arglist(cmock_count(__VA_ARGS__)));                         \
        cmock_close_dlhandle();                                                                             \
        retstatement;                                                                                       \
    }                                                                                                       \
    void cmock_trailing_ ## name (void)

#define cmock_mock_function1(rvinit, call_prefix, retstatement, utype, rettype, name)           \
    cmock_generate_mock_ctx(utype, rettype, name, void);                                        \
    struct cmock_mock_ ## name ## _ctx cmock_mock_ ## name = {                                  \
        .info = {                                                                               \
            .addr = &cmock_mock_ ## name,                                                       \
            .invocations_offset = offsetof(struct cmock_mock_ ## name ## _ctx, invocations),    \
            .opmode_offset = offsetof(struct cmock_mock_ ## name ## _ctx, opdata) +             \
                             offsetof(struct cmock_mock_ ## name ## _opdata, mode)              \
        },                                                                                      \
        .invocations = 0,                                                                       \
        .opdata = {                                                                             \
            .mode = CMOCK_OP_INVOKE,                                                            \
            .invoke = 0                                                                         \
        }                                                                                       \
    };                                                                                          \
    rettype name(void) {                                                                        \
        rvinit;                                                                                 \
        if(cmock_mock_ ## name.invocations) {                                                   \
            if(cmock_mock_ ## name.invocations != -1) {                                         \
                --cmock_mock_ ## name.invocations;                                              \
            }                                                                                   \
            switch(cmock_mock_ ## name.opdata.mode) {                                           \
                case CMOCK_OP_INVOKE:                                                           \
                    call_prefix cmock_mock_ ## name.opdata.invoke();                            \
                    retstatement;                                                               \
                    break;                                                                      \
                case CMOCK_OP_RETURN:                                                           \
                    call_prefix cmock_mock_ ## name.opdata.retval;                              \
                    retstatement;                                                               \
                    break;                                                                      \
                case CMOCK_OP_INCREMENT:                                                        \
                    call_prefix ++cmock_mock_ ## name.opdata.counter;                           \
                    retstatement;                                                               \
                    break;                                                                      \
                default:                                                                        \
                    cmock_assert(0, "Invalid opmode %d", cmock_mock_ ## name.opdata.mode);      \
            }                                                                                   \
        }                                                                                       \
        extern void *cmock_symbol(char const*);                                                 \
        extern void cmock_close_dlhandle(void);                                                 \
        rettype(* cmock_handle_ ## name)(void);                                                 \
        dlerror();                                                                              \
        *(void **) (& cmock_handle_ ## name) = cmock_symbol(#name);                             \
        char const *cmock_error_ ## name = dlerror();                                           \
        cmock_assert(!cmock_error_ ## name, "%s", cmock_error_ ## name);                        \
        call_prefix cmock_handle_ ## name ();                                                   \
        cmock_close_dlhandle();                                                                 \
        retstatement;                                                                           \
    }                                                                                           \
    void cmock_trailing_ ## name (void)
#else

#define cmock_mock_function(rvinit, call_prefix, retstatement, utype, rettype, name, ...)   \
    cmock_generate_mock_ctx(utype, rettype, name, __VA_ARGS__)

#define cmock_mock_function1(rvinit, call_prefix, retstatement, utype, rettype, name)       \
    cmock_generate_mock_ctx(utype, rettype, name, void)
#endif

#ifdef CMOCK_GENERATE_SYMBOLS
#define cmock_for_each_word(iter, str)                                    \
    for(char *end = (iter = str, strchr(str, ' '));                       \
        (end ? *end = '\0' : 0, iter);                                    \
        iter = end ? end + 1 :  0, end ? end = strchr(end + 1, ' ') : 0)
#endif

#define CMOCK_MOCK_FUNCTION(rettype, ...)                                                                           \
    cmock_cat_expand(cmock_mock_function, cmock_count(__VA_ARGS__))(rettype cmock_cat_expand(cmock_rv_, __LINE__),  \
                                                                    cmock_cat_expand(cmock_rv_, __LINE__) =,        \
                                                                    return cmock_cat_expand(cmock_rv_, __LINE__),   \
                                                                    rettype, rettype, __VA_ARGS__)

#define CMOCK_MOCK_FUNCTION0(rettype, ...) \
    cmock_cat_expand(cmock_mock_function0_, cmock_count(__VA_ARGS__))(rettype, __VA_ARGS__)


#define CMOCK_MOCK_FUNCTION_VOID(...)     \
    cmock_cat_expand(cmock_mock_function, cmock_count(__VA_ARGS__))((void)0, (void), return, int, void, __VA_ARGS__)

#define CMOCK_MOCK_FUNCTION_VOID0(...)    \
    cmock_cat_expand(cmock_mock_function_void0_, cmock_count(__VA_ARGS__))(__VA_ARGS__)

#define CMOCK_EXPECT_CALL(name)                                         \
    do {                                                                \
        extern struct cmock_mock_ ## name ## _ctx cmock_mock_ ## name;  \
        /* @todo work around need for global state */                   \
        extern struct cmock_mock_info *mockinfo;                        \
        mockinfo = &cmock_mock_ ## name.info;                           \
        cmock_mock_ ## name

#define CMOCK_WILL_ONCE(...) cmock_will(1, __VA_ARGS__)
#define CMOCK_WILL_REPEATEDLY(...) cmock_will(-1, __VA_ARGS__)
#define CMOCK_WILL_N_TIMES(n, ...) cmock_will(n, __VA_ARGS__)
#define CMOCK_WILL_INVOKE_DEFAULT() cmock_will(0, opdata.invoke = CMOCK_OP_INVOKE)

#define CMOCK_INVOKE(func) cmock_setop(invoke, func, CMOCK_OP_INVOKE)
#define CMOCK_RETURN(value) cmock_setop(retval, value, CMOCK_OP_RETURN)
#define CMOCK_INCREMENT_COUNTER(init) cmock_setop(counter, init, CMOCK_OP_INCREMENT)

#ifndef CMOCK_PREFIX_ONLY
#define MOCK_FUNCTION(...)       CMOCK_MOCK_FUNCTION(__VA_ARGS__)
#define MOCK_FUNCTION0(...)      CMOCK_MOCK_FUNCTION0(__VA_ARGS__)
#define MOCK_FUNCTION_VOID(...)  CMOCK_MOCK_FUNCTION_VOID(__VA_ARGS__)
#define MOCK_FUNCTION_VOID0(...) CMOCK_MOCK_FUNCTION_VOID0(__VA_ARGS__)
#define EXPECT_CALL(...)         CMOCK_EXPECT_CALL(__VA_ARGS__)
#define WILL_ONCE(...)           CMOCK_WILL_ONCE(__VA_ARGS__)
#define WILL_REPEATEDLY(...)     CMOCK_WILL_REPEATEDLY(__VA_ARGS__)
#define WILL_N_TIMES(...)        CMOCK_WILL_N_TIMES(__VA_ARGS__)
#define WILL_INVOKE_DEFAULT()    CMOCK_WILL_INVOKE_DEFAULT()
#define INVOKE(...)              CMOCK_INVOKE(__VA_ARGS__)
#define RETURN(...)              CMOCK_RETURN(__VA_ARGS__)
#define INCREMENT_COUNTER(...)   CMOCK_INCREMENT_COUNTER(__VA_ARGS__)
#endif

#ifdef CMOCK_GMOCK_COMPAT
#define WillOnce(...)         CMOCK_WILL_ONCE(__VA_ARGS__)
#define WillRepeatedly(...)   CMOCK_WILL_REPEATEDLY(__VA_ARGS__)
#define WillNTimes(...)       CMOCK_WILL_N_TIMES(__VA_ARGS__)
#define WillInvokeDefault()   CMOCK_WILL_INVOKE_DEFAULT()
#define Invoke(...)           CMOCK_INVOKE(__VA_ARGS__)
#define Return(...)           CMOCK_RETURN(__VA_ARGS__)
#define IncrementCounter(...) CMOCK_INCREMENT_COUNTER(__VA_ARGS__)
#endif

#include "cmock_config.h"

#endif /* CMOCK_H */
