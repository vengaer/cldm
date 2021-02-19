#ifndef LMC_H
#define LMC_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dlfcn.h>

#define lmc_count_sequence 127, 126, 125, 124, 123, 122, 121, 120,  \
                           119, 118, 117, 116, 115, 114, 113, 112,  \
                           111, 110, 109, 108, 107, 106, 105, 104,  \
                           103, 102, 101, 100, 99,  98,  97,  96,   \
                           95,  94,  93,  92,  91,  90,  89,  88,   \
                           87,  86,  85,  84,  83,  82,  81,  80,   \
                           79,  78,  77,  76,  75,  74,  73,  72,   \
                           71,  70,  69,  68,  67,  66,  65,  64,   \
                           63,  62,  61,  60,  59,  58,  57,  56,   \
                           55,  54,  53,  52,  51,  50,  49,  48,   \
                           47,  46,  45,  44,  43,  42,  41,  40,   \
                           39,  38,  37,  36,  35,  34,  33,  32,   \
                           31,  30,  29,  28,  27,  26,  25,  24,   \
                           23,  22,  21,  20,  19,  18,  17,  16,   \
                           15,  14,  13,  12,  11,  10,  9,   8,    \
                           7,   6,   5,   4,   3,   2,   1,   0

#define lmc_count_pick(a0,   a1,   a2,   a3,   a4,   a5,   a6,   a7,    \
                       a8,   a9,   a10,  a11,  a12,  a13,  a14,  a15,   \
                       a16,  a17,  a18,  a19,  a20,  a21,  a22,  a23,   \
                       a24,  a25,  a26,  a27,  a28,  a29,  a30,  a31,   \
                       a32,  a33,  a34,  a35,  a36,  a37,  a38,  a39,   \
                       a40,  a41,  a42,  a43,  a44,  a45,  a46,  a47,   \
                       a48,  a49,  a50,  a51,  a52,  a53,  a54,  a55,   \
                       a56,  a57,  a58,  a59,  a60,  a61,  a62,  a63,   \
                       a64,  a65,  a66,  a67,  a68,  a69,  a70,  a71,   \
                       a72,  a73,  a74,  a75,  a76,  a77,  a78,  a79,   \
                       a80,  a81,  a82,  a83,  a84,  a85,  a86,  a87,   \
                       a88,  a89,  a90,  a91,  a92,  a93,  a94,  a95,   \
                       a96,  a97,  a98,  a99,  a100, a101, a102, a103,  \
                       a104, a105, a106, a107, a108, a109, a110, a111,  \
                       a112, a113, a114, a115, a116, a117, a118, a119,  \
                       a120, a121, a122, a123, a124, a125, a126, a127,  \
                       ...) a127

#define lmc_count_expand(...) lmc_count_pick(__VA_ARGS__)
#define lmc_count(...) lmc_count_expand(__VA_ARGS__, lmc_count_sequence)

#define lmc_cat(a, b) a ## b
#define lmc_cat_expand(a,b) lmc_cat(a,b)

#define lmc_genparams_1(type)        type a1
#define lmc_genparams_2(type, ...)   type a2,   lmc_genparams_1(__VA_ARGS__)
#define lmc_genparams_3(type, ...)   type a3,   lmc_genparams_2(__VA_ARGS__)
#define lmc_genparams_4(type, ...)   type a4,   lmc_genparams_3(__VA_ARGS__)
#define lmc_genparams_5(type, ...)   type a5,   lmc_genparams_4(__VA_ARGS__)
#define lmc_genparams_6(type, ...)   type a6,   lmc_genparams_5(__VA_ARGS__)
#define lmc_genparams_7(type, ...)   type a7,   lmc_genparams_6(__VA_ARGS__)
#define lmc_genparams_8(type, ...)   type a8,   lmc_genparams_7(__VA_ARGS__)
#define lmc_genparams_9(type, ...)   type a9,   lmc_genparams_8(__VA_ARGS__)
#define lmc_genparams_10(type, ...)  type a10,  lmc_genparams_9(__VA_ARGS__)
#define lmc_genparams_11(type, ...)  type a11,  lmc_genparams_10(__VA_ARGS__)
#define lmc_genparams_12(type, ...)  type a12,  lmc_genparams_11(__VA_ARGS__)
#define lmc_genparams_13(type, ...)  type a13,  lmc_genparams_12(__VA_ARGS__)
#define lmc_genparams_14(type, ...)  type a14,  lmc_genparams_13(__VA_ARGS__)
#define lmc_genparams_15(type, ...)  type a15,  lmc_genparams_14(__VA_ARGS__)
#define lmc_genparams_16(type, ...)  type a16,  lmc_genparams_15(__VA_ARGS__)
#define lmc_genparams_17(type, ...)  type a17,  lmc_genparams_16(__VA_ARGS__)
#define lmc_genparams_18(type, ...)  type a18,  lmc_genparams_17(__VA_ARGS__)
#define lmc_genparams_19(type, ...)  type a19,  lmc_genparams_18(__VA_ARGS__)
#define lmc_genparams_20(type, ...)  type a20,  lmc_genparams_19(__VA_ARGS__)
#define lmc_genparams_21(type, ...)  type a21,  lmc_genparams_20(__VA_ARGS__)
#define lmc_genparams_22(type, ...)  type a22,  lmc_genparams_21(__VA_ARGS__)
#define lmc_genparams_23(type, ...)  type a23,  lmc_genparams_22(__VA_ARGS__)
#define lmc_genparams_24(type, ...)  type a24,  lmc_genparams_23(__VA_ARGS__)
#define lmc_genparams_25(type, ...)  type a25,  lmc_genparams_24(__VA_ARGS__)
#define lmc_genparams_26(type, ...)  type a26,  lmc_genparams_25(__VA_ARGS__)
#define lmc_genparams_27(type, ...)  type a27,  lmc_genparams_26(__VA_ARGS__)
#define lmc_genparams_28(type, ...)  type a28,  lmc_genparams_27(__VA_ARGS__)
#define lmc_genparams_29(type, ...)  type a29,  lmc_genparams_28(__VA_ARGS__)
#define lmc_genparams_30(type, ...)  type a30,  lmc_genparams_29(__VA_ARGS__)
#define lmc_genparams_31(type, ...)  type a31,  lmc_genparams_30(__VA_ARGS__)
#define lmc_genparams_32(type, ...)  type a32,  lmc_genparams_31(__VA_ARGS__)
#define lmc_genparams_33(type, ...)  type a33,  lmc_genparams_32(__VA_ARGS__)
#define lmc_genparams_34(type, ...)  type a34,  lmc_genparams_33(__VA_ARGS__)
#define lmc_genparams_35(type, ...)  type a35,  lmc_genparams_34(__VA_ARGS__)
#define lmc_genparams_36(type, ...)  type a36,  lmc_genparams_35(__VA_ARGS__)
#define lmc_genparams_37(type, ...)  type a37,  lmc_genparams_36(__VA_ARGS__)
#define lmc_genparams_38(type, ...)  type a38,  lmc_genparams_37(__VA_ARGS__)
#define lmc_genparams_39(type, ...)  type a39,  lmc_genparams_38(__VA_ARGS__)
#define lmc_genparams_40(type, ...)  type a40,  lmc_genparams_39(__VA_ARGS__)
#define lmc_genparams_41(type, ...)  type a41,  lmc_genparams_40(__VA_ARGS__)
#define lmc_genparams_42(type, ...)  type a42,  lmc_genparams_41(__VA_ARGS__)
#define lmc_genparams_43(type, ...)  type a43,  lmc_genparams_42(__VA_ARGS__)
#define lmc_genparams_44(type, ...)  type a44,  lmc_genparams_43(__VA_ARGS__)
#define lmc_genparams_45(type, ...)  type a45,  lmc_genparams_44(__VA_ARGS__)
#define lmc_genparams_46(type, ...)  type a46,  lmc_genparams_45(__VA_ARGS__)
#define lmc_genparams_47(type, ...)  type a47,  lmc_genparams_46(__VA_ARGS__)
#define lmc_genparams_48(type, ...)  type a48,  lmc_genparams_47(__VA_ARGS__)
#define lmc_genparams_49(type, ...)  type a49,  lmc_genparams_48(__VA_ARGS__)
#define lmc_genparams_50(type, ...)  type a50,  lmc_genparams_49(__VA_ARGS__)
#define lmc_genparams_51(type, ...)  type a51,  lmc_genparams_50(__VA_ARGS__)
#define lmc_genparams_52(type, ...)  type a52,  lmc_genparams_51(__VA_ARGS__)
#define lmc_genparams_53(type, ...)  type a53,  lmc_genparams_52(__VA_ARGS__)
#define lmc_genparams_54(type, ...)  type a54,  lmc_genparams_53(__VA_ARGS__)
#define lmc_genparams_55(type, ...)  type a55,  lmc_genparams_54(__VA_ARGS__)
#define lmc_genparams_56(type, ...)  type a56,  lmc_genparams_55(__VA_ARGS__)
#define lmc_genparams_57(type, ...)  type a57,  lmc_genparams_56(__VA_ARGS__)
#define lmc_genparams_58(type, ...)  type a58,  lmc_genparams_57(__VA_ARGS__)
#define lmc_genparams_59(type, ...)  type a59,  lmc_genparams_58(__VA_ARGS__)
#define lmc_genparams_60(type, ...)  type a60,  lmc_genparams_59(__VA_ARGS__)
#define lmc_genparams_61(type, ...)  type a61,  lmc_genparams_60(__VA_ARGS__)
#define lmc_genparams_62(type, ...)  type a62,  lmc_genparams_61(__VA_ARGS__)
#define lmc_genparams_63(type, ...)  type a63,  lmc_genparams_62(__VA_ARGS__)
#define lmc_genparams_64(type, ...)  type a64,  lmc_genparams_63(__VA_ARGS__)
#define lmc_genparams_65(type, ...)  type a65,  lmc_genparams_64(__VA_ARGS__)
#define lmc_genparams_66(type, ...)  type a66,  lmc_genparams_65(__VA_ARGS__)
#define lmc_genparams_67(type, ...)  type a67,  lmc_genparams_66(__VA_ARGS__)
#define lmc_genparams_68(type, ...)  type a68,  lmc_genparams_67(__VA_ARGS__)
#define lmc_genparams_69(type, ...)  type a69,  lmc_genparams_68(__VA_ARGS__)
#define lmc_genparams_70(type, ...)  type a70,  lmc_genparams_69(__VA_ARGS__)
#define lmc_genparams_71(type, ...)  type a71,  lmc_genparams_70(__VA_ARGS__)
#define lmc_genparams_72(type, ...)  type a72,  lmc_genparams_71(__VA_ARGS__)
#define lmc_genparams_73(type, ...)  type a73,  lmc_genparams_72(__VA_ARGS__)
#define lmc_genparams_74(type, ...)  type a74,  lmc_genparams_73(__VA_ARGS__)
#define lmc_genparams_75(type, ...)  type a75,  lmc_genparams_74(__VA_ARGS__)
#define lmc_genparams_76(type, ...)  type a76,  lmc_genparams_75(__VA_ARGS__)
#define lmc_genparams_77(type, ...)  type a77,  lmc_genparams_76(__VA_ARGS__)
#define lmc_genparams_78(type, ...)  type a78,  lmc_genparams_77(__VA_ARGS__)
#define lmc_genparams_79(type, ...)  type a79,  lmc_genparams_78(__VA_ARGS__)
#define lmc_genparams_80(type, ...)  type a80,  lmc_genparams_79(__VA_ARGS__)
#define lmc_genparams_81(type, ...)  type a81,  lmc_genparams_80(__VA_ARGS__)
#define lmc_genparams_82(type, ...)  type a82,  lmc_genparams_81(__VA_ARGS__)
#define lmc_genparams_83(type, ...)  type a83,  lmc_genparams_82(__VA_ARGS__)
#define lmc_genparams_84(type, ...)  type a84,  lmc_genparams_83(__VA_ARGS__)
#define lmc_genparams_85(type, ...)  type a85,  lmc_genparams_84(__VA_ARGS__)
#define lmc_genparams_86(type, ...)  type a86,  lmc_genparams_85(__VA_ARGS__)
#define lmc_genparams_87(type, ...)  type a87,  lmc_genparams_86(__VA_ARGS__)
#define lmc_genparams_88(type, ...)  type a88,  lmc_genparams_87(__VA_ARGS__)
#define lmc_genparams_89(type, ...)  type a89,  lmc_genparams_88(__VA_ARGS__)
#define lmc_genparams_90(type, ...)  type a90,  lmc_genparams_89(__VA_ARGS__)
#define lmc_genparams_91(type, ...)  type a91,  lmc_genparams_90(__VA_ARGS__)
#define lmc_genparams_92(type, ...)  type a92,  lmc_genparams_91(__VA_ARGS__)
#define lmc_genparams_93(type, ...)  type a93,  lmc_genparams_92(__VA_ARGS__)
#define lmc_genparams_94(type, ...)  type a94,  lmc_genparams_93(__VA_ARGS__)
#define lmc_genparams_95(type, ...)  type a95,  lmc_genparams_94(__VA_ARGS__)
#define lmc_genparams_96(type, ...)  type a96,  lmc_genparams_95(__VA_ARGS__)
#define lmc_genparams_97(type, ...)  type a97,  lmc_genparams_96(__VA_ARGS__)
#define lmc_genparams_98(type, ...)  type a98,  lmc_genparams_97(__VA_ARGS__)
#define lmc_genparams_99(type, ...)  type a99,  lmc_genparams_98(__VA_ARGS__)
#define lmc_genparams_100(type, ...) type a100, lmc_genparams_99(__VA_ARGS__)
#define lmc_genparams_101(type, ...) type a101, lmc_genparams_100(__VA_ARGS__)
#define lmc_genparams_102(type, ...) type a102, lmc_genparams_101(__VA_ARGS__)
#define lmc_genparams_103(type, ...) type a103, lmc_genparams_102(__VA_ARGS__)
#define lmc_genparams_104(type, ...) type a104, lmc_genparams_103(__VA_ARGS__)
#define lmc_genparams_105(type, ...) type a105, lmc_genparams_104(__VA_ARGS__)
#define lmc_genparams_106(type, ...) type a106, lmc_genparams_105(__VA_ARGS__)
#define lmc_genparams_107(type, ...) type a107, lmc_genparams_106(__VA_ARGS__)
#define lmc_genparams_108(type, ...) type a108, lmc_genparams_107(__VA_ARGS__)
#define lmc_genparams_109(type, ...) type a109, lmc_genparams_108(__VA_ARGS__)
#define lmc_genparams_110(type, ...) type a110, lmc_genparams_109(__VA_ARGS__)
#define lmc_genparams_111(type, ...) type a111, lmc_genparams_110(__VA_ARGS__)
#define lmc_genparams_112(type, ...) type a112, lmc_genparams_111(__VA_ARGS__)
#define lmc_genparams_113(type, ...) type a113, lmc_genparams_112(__VA_ARGS__)
#define lmc_genparams_114(type, ...) type a114, lmc_genparams_113(__VA_ARGS__)
#define lmc_genparams_115(type, ...) type a115, lmc_genparams_114(__VA_ARGS__)
#define lmc_genparams_116(type, ...) type a116, lmc_genparams_115(__VA_ARGS__)
#define lmc_genparams_117(type, ...) type a117, lmc_genparams_116(__VA_ARGS__)
#define lmc_genparams_118(type, ...) type a118, lmc_genparams_117(__VA_ARGS__)
#define lmc_genparams_119(type, ...) type a119, lmc_genparams_118(__VA_ARGS__)
#define lmc_genparams_120(type, ...) type a120, lmc_genparams_119(__VA_ARGS__)
#define lmc_genparams_121(type, ...) type a121, lmc_genparams_120(__VA_ARGS__)
#define lmc_genparams_122(type, ...) type a122, lmc_genparams_121(__VA_ARGS__)
#define lmc_genparams_123(type, ...) type a123, lmc_genparams_122(__VA_ARGS__)
#define lmc_genparams_124(type, ...) type a124, lmc_genparams_123(__VA_ARGS__)
#define lmc_genparams_125(type, ...) type a125, lmc_genparams_124(__VA_ARGS__)
#define lmc_genparams_126(type, ...) type a126, lmc_genparams_125(__VA_ARGS__)
#define lmc_genparams_127(type, ...) type a127, lmc_genparams_126(__VA_ARGS__)

#define lmc_genparams(...) lmc_cat_expand(lmc_genparams_,lmc_count(__VA_ARGS__))(__VA_ARGS__)

#define lmc_arglist_1   a1
#define lmc_arglist_2   a2,   lmc_arglist_1
#define lmc_arglist_3   a3,   lmc_arglist_2
#define lmc_arglist_4   a4,   lmc_arglist_3
#define lmc_arglist_5   a5,   lmc_arglist_4
#define lmc_arglist_6   a6,   lmc_arglist_5
#define lmc_arglist_7   a7,   lmc_arglist_6
#define lmc_arglist_8   a8,   lmc_arglist_7
#define lmc_arglist_9   a9,   lmc_arglist_8
#define lmc_arglist_10  a10,  lmc_arglist_9
#define lmc_arglist_11  a11,  lmc_arglist_10
#define lmc_arglist_12  a12,  lmc_arglist_11
#define lmc_arglist_13  a13,  lmc_arglist_12
#define lmc_arglist_14  a14,  lmc_arglist_13
#define lmc_arglist_15  a15,  lmc_arglist_14
#define lmc_arglist_16  a16,  lmc_arglist_15
#define lmc_arglist_17  a17,  lmc_arglist_16
#define lmc_arglist_18  a18,  lmc_arglist_17
#define lmc_arglist_19  a19,  lmc_arglist_18
#define lmc_arglist_20  a20,  lmc_arglist_19
#define lmc_arglist_21  a21,  lmc_arglist_20
#define lmc_arglist_22  a22,  lmc_arglist_21
#define lmc_arglist_23  a23,  lmc_arglist_22
#define lmc_arglist_24  a24,  lmc_arglist_23
#define lmc_arglist_25  a25,  lmc_arglist_24
#define lmc_arglist_26  a26,  lmc_arglist_25
#define lmc_arglist_27  a27,  lmc_arglist_26
#define lmc_arglist_28  a28,  lmc_arglist_27
#define lmc_arglist_29  a29,  lmc_arglist_28
#define lmc_arglist_30  a30,  lmc_arglist_29
#define lmc_arglist_31  a31,  lmc_arglist_30
#define lmc_arglist_32  a32,  lmc_arglist_31
#define lmc_arglist_33  a33,  lmc_arglist_32
#define lmc_arglist_34  a34,  lmc_arglist_33
#define lmc_arglist_35  a35,  lmc_arglist_34
#define lmc_arglist_36  a36,  lmc_arglist_35
#define lmc_arglist_37  a37,  lmc_arglist_36
#define lmc_arglist_38  a38,  lmc_arglist_37
#define lmc_arglist_39  a39,  lmc_arglist_38
#define lmc_arglist_40  a40,  lmc_arglist_39
#define lmc_arglist_41  a41,  lmc_arglist_40
#define lmc_arglist_42  a42,  lmc_arglist_41
#define lmc_arglist_43  a43,  lmc_arglist_42
#define lmc_arglist_44  a44,  lmc_arglist_43
#define lmc_arglist_45  a45,  lmc_arglist_44
#define lmc_arglist_46  a46,  lmc_arglist_45
#define lmc_arglist_47  a47,  lmc_arglist_46
#define lmc_arglist_48  a48,  lmc_arglist_47
#define lmc_arglist_49  a49,  lmc_arglist_48
#define lmc_arglist_50  a50,  lmc_arglist_49
#define lmc_arglist_51  a51,  lmc_arglist_50
#define lmc_arglist_52  a52,  lmc_arglist_51
#define lmc_arglist_53  a53,  lmc_arglist_52
#define lmc_arglist_54  a54,  lmc_arglist_53
#define lmc_arglist_55  a55,  lmc_arglist_54
#define lmc_arglist_56  a56,  lmc_arglist_55
#define lmc_arglist_57  a57,  lmc_arglist_56
#define lmc_arglist_58  a58,  lmc_arglist_57
#define lmc_arglist_59  a59,  lmc_arglist_58
#define lmc_arglist_60  a60,  lmc_arglist_59
#define lmc_arglist_61  a61,  lmc_arglist_60
#define lmc_arglist_62  a62,  lmc_arglist_61
#define lmc_arglist_63  a63,  lmc_arglist_62
#define lmc_arglist_64  a64,  lmc_arglist_63
#define lmc_arglist_65  a65,  lmc_arglist_64
#define lmc_arglist_66  a66,  lmc_arglist_65
#define lmc_arglist_67  a67,  lmc_arglist_66
#define lmc_arglist_68  a68,  lmc_arglist_67
#define lmc_arglist_69  a69,  lmc_arglist_68
#define lmc_arglist_70  a70,  lmc_arglist_69
#define lmc_arglist_71  a71,  lmc_arglist_70
#define lmc_arglist_72  a72,  lmc_arglist_71
#define lmc_arglist_73  a73,  lmc_arglist_72
#define lmc_arglist_74  a74,  lmc_arglist_73
#define lmc_arglist_75  a75,  lmc_arglist_74
#define lmc_arglist_76  a76,  lmc_arglist_75
#define lmc_arglist_77  a77,  lmc_arglist_76
#define lmc_arglist_78  a78,  lmc_arglist_77
#define lmc_arglist_79  a79,  lmc_arglist_78
#define lmc_arglist_80  a80,  lmc_arglist_79
#define lmc_arglist_81  a81,  lmc_arglist_80
#define lmc_arglist_82  a82,  lmc_arglist_81
#define lmc_arglist_83  a83,  lmc_arglist_82
#define lmc_arglist_84  a84,  lmc_arglist_83
#define lmc_arglist_85  a85,  lmc_arglist_84
#define lmc_arglist_86  a86,  lmc_arglist_85
#define lmc_arglist_87  a87,  lmc_arglist_86
#define lmc_arglist_88  a88,  lmc_arglist_87
#define lmc_arglist_89  a89,  lmc_arglist_88
#define lmc_arglist_90  a90,  lmc_arglist_89
#define lmc_arglist_91  a91,  lmc_arglist_90
#define lmc_arglist_92  a92,  lmc_arglist_91
#define lmc_arglist_93  a93,  lmc_arglist_92
#define lmc_arglist_94  a94,  lmc_arglist_93
#define lmc_arglist_95  a95,  lmc_arglist_94
#define lmc_arglist_96  a96,  lmc_arglist_95
#define lmc_arglist_97  a97,  lmc_arglist_96
#define lmc_arglist_98  a98,  lmc_arglist_97
#define lmc_arglist_99  a99,  lmc_arglist_98
#define lmc_arglist_100 a100, lmc_arglist_99
#define lmc_arglist_101 a101, lmc_arglist_100
#define lmc_arglist_102 a102, lmc_arglist_101
#define lmc_arglist_103 a103, lmc_arglist_102
#define lmc_arglist_104 a104, lmc_arglist_103
#define lmc_arglist_105 a105, lmc_arglist_104
#define lmc_arglist_106 a106, lmc_arglist_105
#define lmc_arglist_107 a107, lmc_arglist_106
#define lmc_arglist_108 a108, lmc_arglist_107
#define lmc_arglist_109 a109, lmc_arglist_108
#define lmc_arglist_110 a110, lmc_arglist_109
#define lmc_arglist_111 a111, lmc_arglist_110
#define lmc_arglist_112 a112, lmc_arglist_111
#define lmc_arglist_113 a113, lmc_arglist_112
#define lmc_arglist_114 a114, lmc_arglist_113
#define lmc_arglist_115 a115, lmc_arglist_114
#define lmc_arglist_116 a116, lmc_arglist_115
#define lmc_arglist_117 a117, lmc_arglist_116
#define lmc_arglist_118 a118, lmc_arglist_117
#define lmc_arglist_119 a119, lmc_arglist_118
#define lmc_arglist_120 a120, lmc_arglist_119
#define lmc_arglist_121 a121, lmc_arglist_120
#define lmc_arglist_122 a122, lmc_arglist_121
#define lmc_arglist_123 a123, lmc_arglist_122
#define lmc_arglist_124 a124, lmc_arglist_123
#define lmc_arglist_125 a125, lmc_arglist_124
#define lmc_arglist_126 a126, lmc_arglist_125
#define lmc_arglist_127 a127, lmc_arglist_126

#define lmc_arglist(size) lmc_cat(lmc_arglist_, size)

#define lmc_mock_function2(...)   lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function3(...)   lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function4(...)   lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function5(...)   lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function6(...)   lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function7(...)   lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function8(...)   lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function9(...)   lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function10(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function11(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function12(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function13(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function14(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function15(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function16(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function17(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function18(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function19(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function20(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function21(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function22(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function23(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function24(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function25(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function26(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function27(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function28(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function29(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function30(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function31(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function32(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function33(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function34(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function35(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function36(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function37(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function38(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function39(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function40(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function41(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function42(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function43(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function44(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function45(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function46(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function47(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function48(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function49(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function50(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function51(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function52(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function53(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function54(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function55(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function56(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function57(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function58(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function59(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function60(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function61(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function62(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function63(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function64(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function65(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function66(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function67(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function68(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function69(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function70(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function71(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function72(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function73(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function74(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function75(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function76(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function77(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function78(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function79(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function80(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function81(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function82(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function83(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function84(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function85(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function86(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function87(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function88(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function89(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function90(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function91(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function92(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function93(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function94(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function95(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function96(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function97(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function98(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function99(...)  lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function100(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function101(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function102(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function103(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function104(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function105(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function106(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function107(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function108(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function109(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function110(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function111(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function112(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function113(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function114(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function115(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function116(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function117(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function118(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function119(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function120(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function121(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function122(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function123(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function124(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function125(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function126(...) lmc_mock_function(__VA_ARGS__)
#define lmc_mock_function127(...) lmc_mock_function(__VA_ARGS__)

#define lmc_mock_function0_1(rettype, name) LMC_MOCK_FUNCTION(rettype, name)
#define lmc_mock_function0_2(rettype, name, _) LMC_MOCK_FUNCTION(rettype, name)

#define lmc_mock_function_void0_1(name) LMC_MOCK_FUNCTION_VOID(name)
#define lmc_mock_function_void0_2(name, _) LMC_MOCK_FUNCTION_VOID(name)


#define lmc_assert(condition, msg)                                                                      \
    do {                                                                                                \
        if(!(condition)) {                                                                              \
            fprintf(stderr, "%s:%d assertion '%s' failed: %s\n", __FILE__, __LINE__, #condition, msg);  \
            _Exit(1);                                                                                   \
        }                                                                                               \
    } while (0)

#define lmc_overload(name, ...) \
    lmc_cat_expand(name, lmc_count(__VA_ARGS__))(__VA_ARGS__)

#define lmc_will(do_once, ...)                                                                      \
        __VA_ARGS__;                                                                                \
        memset((unsigned char *)mockinfo->addr + mockinfo->expired_offset, 0, sizeof(_Bool));       \
        memset((unsigned char *)mockinfo->addr + mockinfo->once_offset, do_once, sizeof(_Bool));    \
    } while (0)

#define lmc_set_opmode(mode)                                                                                                                \
    enum lmc_opmode *lmc_cat_expand(lmc_opdata, __LINE__) = (enum lmc_opmode *)((unsigned char*)mockinfo->addr + mockinfo->opmode_offset);  \
    *lmc_cat_expand(lmc_opdata, __LINE__) = mode

#define lmc_setop(field, value, mode)   \
    opdata.field = value;               \
    lmc_set_opmode(mode)

struct lmc_mock_info {
    void *addr;
    unsigned expired_offset;
    unsigned once_offset;
    unsigned opmode_offset;
};

enum lmc_opmode {
    LMC_OP_INVOKE,
    LMC_OP_RETURN,
    LMC_OP_INCREMENT,
};

#define lmc_mock_function(call_prefix, call_postop, utype, rettype, name, ...)                          \
    struct lmc_mock_ ## name ## _ctx {                                                                  \
        struct lmc_mock_info info;                                                                      \
        _Bool expired;                                                                                  \
        _Bool once;                                                                                     \
        struct lmc_mock_ ## name ## _opdata{                                                            \
            enum lmc_opmode mode;                                                                       \
            union {                                                                                     \
                rettype(*invoke)(__VA_ARGS__);                                                          \
                utype retval;                                                                           \
                utype counter;                                                                          \
            };                                                                                          \
        } opdata;                                                                                       \
    } lmc_mock_ ## name = {                                                                             \
        .info = {                                                                                       \
            .addr = &lmc_mock_ ## name,                                                                 \
            .expired_offset = offsetof(struct lmc_mock_ ## name ## _ctx, expired),                      \
            .once_offset = offsetof(struct lmc_mock_ ## name ## _ctx, once),                            \
            .opmode_offset = offsetof(struct lmc_mock_ ## name ## _ctx, opdata) +                       \
                             offsetof(struct lmc_mock_ ## name ## _opdata, mode)                        \
        },                                                                                              \
        .expired = 1,                                                                                   \
        .once = 0,                                                                                      \
        .opdata = {                                                                                     \
            .mode = LMC_OP_INVOKE,                                                                      \
            .invoke = 0                                                                                 \
        }                                                                                               \
    };                                                                                                  \
    rettype name(lmc_genparams(__VA_ARGS__)) {                                                          \
        if(!lmc_mock_ ## name.expired) {                                                                \
            if(lmc_mock_ ## name.once) {                                                                \
                lmc_mock_ ## name.expired = 1;                                                          \
            }                                                                                           \
            switch(lmc_mock_ ## name.opdata.mode) {                                                     \
                case LMC_OP_INVOKE:                                                                     \
                    call_prefix lmc_mock_ ## name.opdata.invoke(lmc_arglist(lmc_count(__VA_ARGS__)));   \
                    call_postop;                                                                        \
                    break;                                                                              \
                case LMC_OP_RETURN:                                                                     \
                    call_prefix lmc_mock_ ## name.opdata.retval;                                        \
                    call_postop;                                                                        \
                    break;                                                                              \
                case LMC_OP_INCREMENT:                                                                  \
                    call_prefix ++lmc_mock_ ## name.opdata.counter;                                     \
                    call_postop;                                                                        \
                    break;                                                                              \
                default:                                                                                \
                    lmc_assert(0, "Invalid opmode");                                                    \
            }                                                                                           \
        }                                                                                               \
        extern _Bool lmc_is_open(void);                                                                 \
        extern void *lmc_symbol(char const*);                                                           \
        lmc_assert(lmc_is_open(), "lmc has not been initialized");                                      \
        rettype(* lmc_handle_ ## name)(__VA_ARGS__);                                                    \
        dlerror();                                                                                      \
        *(void **) (& lmc_handle_ ## name) = lmc_symbol(#name);                                         \
        char const *lmc_error_ ## name = dlerror();                                                     \
        lmc_assert(!lmc_error_ ## name, lmc_error_ ## name);                                            \
        call_prefix lmc_handle_ ## name(lmc_arglist(lmc_count(__VA_ARGS__)));                           \
        call_postop;                                                                                    \
    }                                                                                                   \
    void lmc_trailing_ ## name (void)

#define lmc_mock_function1(call_prefix, call_postop, utype, rettype, name)                              \
    struct lmc_mock_ ## name ## _ctx {                                                                  \
        struct lmc_mock_info info;                                                                      \
        _Bool expired;                                                                                  \
        _Bool once;                                                                                     \
        struct lmc_mock_ ## name ## _opdata{                                                            \
            enum lmc_opmode mode;                                                                       \
            union {                                                                                     \
                rettype(*invoke)(void);                                                                 \
                utype retval;                                                                           \
                utype counter;                                                                          \
            };                                                                                          \
        } opdata;                                                                                       \
    } lmc_mock_ ## name = {                                                                             \
        .info = {                                                                                       \
            .addr = &lmc_mock_ ## name,                                                                 \
            .expired_offset = offsetof(struct lmc_mock_ ## name ## _ctx, expired),                      \
            .once_offset = offsetof(struct lmc_mock_ ## name ## _ctx, once),                            \
            .opmode_offset = offsetof(struct lmc_mock_ ## name ## _ctx, opdata) +                       \
                             offsetof(struct lmc_mock_ ## name ## _opdata, mode)                        \
        },                                                                                              \
        .expired = 1,                                                                                   \
        .once = 0,                                                                                      \
        .opdata = {                                                                                     \
            .mode = LMC_OP_INVOKE,                                                                      \
            .invoke = 0                                                                                 \
        }                                                                                               \
    };                                                                                                  \
    rettype name(void) {                                                                                \
        if(!lmc_mock_ ## name.expired) {                                                                \
            if(lmc_mock_ ## name.once) {                                                                \
                lmc_mock_ ## name.expired = 1;                                                          \
            }                                                                                           \
            switch(lmc_mock_ ## name.opdata.mode) {                                                     \
                case LMC_OP_INVOKE:                                                                     \
                    call_prefix lmc_mock_ ## name.opdata.invoke();                                      \
                    call_postop;                                                                        \
                    break;                                                                              \
                case LMC_OP_RETURN:                                                                     \
                    call_prefix lmc_mock_ ## name.opdata.retval;                                        \
                    call_postop;                                                                        \
                    break;                                                                              \
                case LMC_OP_INCREMENT:                                                                  \
                    call_prefix ++lmc_mock_ ## name.opdata.counter;                                     \
                    call_postop;                                                                        \
                    break;                                                                              \
                default:                                                                                \
                    lmc_assert(0, "Invalid opmode");                                                    \
            }                                                                                           \
        }                                                                                               \
        extern _Bool lmc_is_open(void);                                                                 \
        extern void *lmc_symbol(char const*);                                                           \
        lmc_assert(lmc_is_open(), "lmc has not been initialized");                                      \
        rettype(* lmc_handle_ ## name)(void);                                                           \
        dlerror();                                                                                      \
        *(void **) (& lmc_handle_ ## name) = lmc_symbol(#name);                                         \
        char const *lmc_error_ ## name = dlerror();                                                     \
        lmc_assert(!lmc_error_ ## name, lmc_error_ ## name);                                            \
        call_prefix lmc_handle_ ## name ();                                                             \
        call_postop;                                                                                    \
    }                                                                                                   \
    void lmc_trailing_ ## name (void)

#define LMC_MOCK_FUNCTION(rettype, ...) \
    lmc_cat_expand(lmc_mock_function, lmc_count(__VA_ARGS__))(return, (void)0, rettype, rettype, __VA_ARGS__)

#define LMC_MOCK_FUNCTION0(rettype, ...) \
    lmc_cat_expand(lmc_mock_function0_, lmc_count(__VA_ARGS__))(rettype, __VA_ARGS__)


#define LMC_MOCK_FUNCTION_VOID(...)     \
    lmc_cat_expand(lmc_mock_function, lmc_count(__VA_ARGS__))((void), return, int, void, __VA_ARGS__)

#define LMC_MOCK_FUNCTION_VOID0(...)    \
    lmc_cat_expand(lmc_mock_function_void0_, lmc_count(__VA_ARGS__))(__VA_ARGS__)

#define LMC_EXPECT_CALL(name)                                       \
    do {                                                            \
        extern struct lmc_mock_ ## name ## _ctx lmc_mock_ ## name;  \
        /* @todo work around need for global state */               \
        extern struct lmc_mock_info *mockinfo;                      \
        mockinfo = &lmc_mock_ ## name.info;                         \
        lmc_mock_ ## name

#define LMC_WILL_ONCE(...) lmc_will(1, __VA_ARGS__)
#define LMC_WILL_REPEATEDLY(...) lmc_will(0, __VA_ARGS__)

#define LMC_INVOKE(func) lmc_setop(invoke, &func, LMC_OP_INVOKE)
#define LMC_RETURN(value) lmc_setop(retval, value, LMC_OP_RETURN)
#define LMC_INCREMENT_COUNTER(init) lmc_setop(counter, init, LMC_OP_INCREMENT)

#ifndef LMC_PREFIX_ONLY
#define MOCK_FUNCTION(...)       LMC_MOCK_FUNCTION(__VA_ARGS__)
#define MOCK_FUNCTION0(...)      LMC_MOCK_FUNCTION0(__VA_ARGS__)
#define MOCK_FUNCTION_VOID(...)  LMC_MOCK_FUNCTION_VOID(__VA_ARGS__)
#define MOCK_FUNCTION_VOID0(...) LMC_MOCK_FUNCTION_VOID0(__VA_ARGS__)
#define EXPECT_CALL(...)         LMC_EXPECT_CALL(__VA_ARGS__)
#define WILL_ONCE(...)           LMC_WILL_ONCE(__VA_ARGS__)
#define WILL_REPEATEDLY(...)     LMC_WILL_REPEATEDLY(__VA_ARGS__)
#define INVOKE(...)              LMC_INVOKE(__VA_ARGS__)
#define RETURN(...)              LMC_RETURN(__VA_ARGS__)
#define INCREMENT_COUNTER(...)   LMC_INCREMENT_COUNTER(__VA_ARGS__)
#define ASSIGN(...)              LMC_ASSIGN(__VA_ARGS__)
#endif

#ifdef LMC_GMOCK_COMPAT
#define WillOnce(...)         LMC_WILL_ONCE(__VA_ARGS__)
#define WillRepeatedly(...)   LMC_WILL_REPEATEDLY(__VA_ARGS__)
#define Invoke(...)           LMC_INVOKE(__VA_ARGS__)
#define Return(...)           LMC_RETURN(__VA_ARGS__)
#define IncrementCounter(...) LMC_INCREMENT_COUNTER(__VA_ARGS__)
#define Assign(...)           LMC_ASSIGN(__VA_ARGS__)
#endif

int lmc_init2(int argc, char const **argv);
int lmc_init1(char const *binary_path);
void lmc_close(void);

#define lmc_init(...) lmc_overload(lmc_init, __VA_ARGS__)

#endif /* LMC_H */
