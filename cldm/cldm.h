#ifndef CLDM_H
#define CLDM_H

#include "cldm_rtassert.h"
#include "cldm_dl.h"
#include "cldm_macro.h"
#include "cldm_test.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define cldm_genparams_1(type)        type a0
#define cldm_genparams_2(type, ...)   type a1,   cldm_genparams_1(__VA_ARGS__)
#define cldm_genparams_3(type, ...)   type a2,   cldm_genparams_2(__VA_ARGS__)
#define cldm_genparams_4(type, ...)   type a3,   cldm_genparams_3(__VA_ARGS__)
#define cldm_genparams_5(type, ...)   type a4,   cldm_genparams_4(__VA_ARGS__)
#define cldm_genparams_6(type, ...)   type a5,   cldm_genparams_5(__VA_ARGS__)
#define cldm_genparams_7(type, ...)   type a6,   cldm_genparams_6(__VA_ARGS__)
#define cldm_genparams_8(type, ...)   type a7,   cldm_genparams_7(__VA_ARGS__)
#define cldm_genparams_9(type, ...)   type a8,   cldm_genparams_8(__VA_ARGS__)
#define cldm_genparams_10(type, ...)  type a9,   cldm_genparams_9(__VA_ARGS__)
#define cldm_genparams_11(type, ...)  type a10,  cldm_genparams_10(__VA_ARGS__)
#define cldm_genparams_12(type, ...)  type a11,  cldm_genparams_11(__VA_ARGS__)
#define cldm_genparams_13(type, ...)  type a12,  cldm_genparams_12(__VA_ARGS__)
#define cldm_genparams_14(type, ...)  type a13,  cldm_genparams_13(__VA_ARGS__)
#define cldm_genparams_15(type, ...)  type a14,  cldm_genparams_14(__VA_ARGS__)
#define cldm_genparams_16(type, ...)  type a15,  cldm_genparams_15(__VA_ARGS__)
#define cldm_genparams_17(type, ...)  type a16,  cldm_genparams_16(__VA_ARGS__)
#define cldm_genparams_18(type, ...)  type a17,  cldm_genparams_17(__VA_ARGS__)
#define cldm_genparams_19(type, ...)  type a18,  cldm_genparams_18(__VA_ARGS__)
#define cldm_genparams_20(type, ...)  type a19,  cldm_genparams_19(__VA_ARGS__)
#define cldm_genparams_21(type, ...)  type a20,  cldm_genparams_20(__VA_ARGS__)
#define cldm_genparams_22(type, ...)  type a21,  cldm_genparams_21(__VA_ARGS__)
#define cldm_genparams_23(type, ...)  type a22,  cldm_genparams_22(__VA_ARGS__)
#define cldm_genparams_24(type, ...)  type a23,  cldm_genparams_23(__VA_ARGS__)
#define cldm_genparams_25(type, ...)  type a24,  cldm_genparams_24(__VA_ARGS__)
#define cldm_genparams_26(type, ...)  type a25,  cldm_genparams_25(__VA_ARGS__)
#define cldm_genparams_27(type, ...)  type a26,  cldm_genparams_26(__VA_ARGS__)
#define cldm_genparams_28(type, ...)  type a27,  cldm_genparams_27(__VA_ARGS__)
#define cldm_genparams_29(type, ...)  type a28,  cldm_genparams_28(__VA_ARGS__)
#define cldm_genparams_30(type, ...)  type a29,  cldm_genparams_29(__VA_ARGS__)
#define cldm_genparams_31(type, ...)  type a30,  cldm_genparams_30(__VA_ARGS__)
#define cldm_genparams_32(type, ...)  type a31,  cldm_genparams_31(__VA_ARGS__)
#define cldm_genparams_33(type, ...)  type a32,  cldm_genparams_32(__VA_ARGS__)
#define cldm_genparams_34(type, ...)  type a33,  cldm_genparams_33(__VA_ARGS__)
#define cldm_genparams_35(type, ...)  type a34,  cldm_genparams_34(__VA_ARGS__)
#define cldm_genparams_36(type, ...)  type a35,  cldm_genparams_35(__VA_ARGS__)
#define cldm_genparams_37(type, ...)  type a36,  cldm_genparams_36(__VA_ARGS__)
#define cldm_genparams_38(type, ...)  type a37,  cldm_genparams_37(__VA_ARGS__)
#define cldm_genparams_39(type, ...)  type a38,  cldm_genparams_38(__VA_ARGS__)
#define cldm_genparams_40(type, ...)  type a39,  cldm_genparams_39(__VA_ARGS__)
#define cldm_genparams_41(type, ...)  type a40,  cldm_genparams_40(__VA_ARGS__)
#define cldm_genparams_42(type, ...)  type a41,  cldm_genparams_41(__VA_ARGS__)
#define cldm_genparams_43(type, ...)  type a42,  cldm_genparams_42(__VA_ARGS__)
#define cldm_genparams_44(type, ...)  type a43,  cldm_genparams_43(__VA_ARGS__)
#define cldm_genparams_45(type, ...)  type a44,  cldm_genparams_44(__VA_ARGS__)
#define cldm_genparams_46(type, ...)  type a45,  cldm_genparams_45(__VA_ARGS__)
#define cldm_genparams_47(type, ...)  type a46,  cldm_genparams_46(__VA_ARGS__)
#define cldm_genparams_48(type, ...)  type a47,  cldm_genparams_47(__VA_ARGS__)
#define cldm_genparams_49(type, ...)  type a48,  cldm_genparams_48(__VA_ARGS__)
#define cldm_genparams_50(type, ...)  type a49,  cldm_genparams_49(__VA_ARGS__)
#define cldm_genparams_51(type, ...)  type a50,  cldm_genparams_50(__VA_ARGS__)
#define cldm_genparams_52(type, ...)  type a51,  cldm_genparams_51(__VA_ARGS__)
#define cldm_genparams_53(type, ...)  type a52,  cldm_genparams_52(__VA_ARGS__)
#define cldm_genparams_54(type, ...)  type a53,  cldm_genparams_53(__VA_ARGS__)
#define cldm_genparams_55(type, ...)  type a54,  cldm_genparams_54(__VA_ARGS__)
#define cldm_genparams_56(type, ...)  type a55,  cldm_genparams_55(__VA_ARGS__)
#define cldm_genparams_57(type, ...)  type a56,  cldm_genparams_56(__VA_ARGS__)
#define cldm_genparams_58(type, ...)  type a57,  cldm_genparams_57(__VA_ARGS__)
#define cldm_genparams_59(type, ...)  type a58,  cldm_genparams_58(__VA_ARGS__)
#define cldm_genparams_60(type, ...)  type a59,  cldm_genparams_59(__VA_ARGS__)
#define cldm_genparams_61(type, ...)  type a60,  cldm_genparams_60(__VA_ARGS__)
#define cldm_genparams_62(type, ...)  type a61,  cldm_genparams_61(__VA_ARGS__)
#define cldm_genparams_63(type, ...)  type a62,  cldm_genparams_62(__VA_ARGS__)
#define cldm_genparams_64(type, ...)  type a63,  cldm_genparams_63(__VA_ARGS__)
#define cldm_genparams_65(type, ...)  type a64,  cldm_genparams_64(__VA_ARGS__)
#define cldm_genparams_66(type, ...)  type a65,  cldm_genparams_65(__VA_ARGS__)
#define cldm_genparams_67(type, ...)  type a66,  cldm_genparams_66(__VA_ARGS__)
#define cldm_genparams_68(type, ...)  type a67,  cldm_genparams_67(__VA_ARGS__)
#define cldm_genparams_69(type, ...)  type a68,  cldm_genparams_68(__VA_ARGS__)
#define cldm_genparams_70(type, ...)  type a69,  cldm_genparams_69(__VA_ARGS__)
#define cldm_genparams_71(type, ...)  type a70,  cldm_genparams_70(__VA_ARGS__)
#define cldm_genparams_72(type, ...)  type a71,  cldm_genparams_71(__VA_ARGS__)
#define cldm_genparams_73(type, ...)  type a72,  cldm_genparams_72(__VA_ARGS__)
#define cldm_genparams_74(type, ...)  type a73,  cldm_genparams_73(__VA_ARGS__)
#define cldm_genparams_75(type, ...)  type a74,  cldm_genparams_74(__VA_ARGS__)
#define cldm_genparams_76(type, ...)  type a75,  cldm_genparams_75(__VA_ARGS__)
#define cldm_genparams_77(type, ...)  type a76,  cldm_genparams_76(__VA_ARGS__)
#define cldm_genparams_78(type, ...)  type a77,  cldm_genparams_77(__VA_ARGS__)
#define cldm_genparams_79(type, ...)  type a78,  cldm_genparams_78(__VA_ARGS__)
#define cldm_genparams_80(type, ...)  type a79,  cldm_genparams_79(__VA_ARGS__)
#define cldm_genparams_81(type, ...)  type a80,  cldm_genparams_80(__VA_ARGS__)
#define cldm_genparams_82(type, ...)  type a81,  cldm_genparams_81(__VA_ARGS__)
#define cldm_genparams_83(type, ...)  type a82,  cldm_genparams_82(__VA_ARGS__)
#define cldm_genparams_84(type, ...)  type a83,  cldm_genparams_83(__VA_ARGS__)
#define cldm_genparams_85(type, ...)  type a84,  cldm_genparams_84(__VA_ARGS__)
#define cldm_genparams_86(type, ...)  type a85,  cldm_genparams_85(__VA_ARGS__)
#define cldm_genparams_87(type, ...)  type a86,  cldm_genparams_86(__VA_ARGS__)
#define cldm_genparams_88(type, ...)  type a87,  cldm_genparams_87(__VA_ARGS__)
#define cldm_genparams_89(type, ...)  type a88,  cldm_genparams_88(__VA_ARGS__)
#define cldm_genparams_90(type, ...)  type a89,  cldm_genparams_89(__VA_ARGS__)
#define cldm_genparams_91(type, ...)  type a90,  cldm_genparams_90(__VA_ARGS__)
#define cldm_genparams_92(type, ...)  type a91,  cldm_genparams_91(__VA_ARGS__)
#define cldm_genparams_93(type, ...)  type a92,  cldm_genparams_92(__VA_ARGS__)
#define cldm_genparams_94(type, ...)  type a93,  cldm_genparams_93(__VA_ARGS__)
#define cldm_genparams_95(type, ...)  type a94,  cldm_genparams_94(__VA_ARGS__)
#define cldm_genparams_96(type, ...)  type a95,  cldm_genparams_95(__VA_ARGS__)
#define cldm_genparams_97(type, ...)  type a96,  cldm_genparams_96(__VA_ARGS__)
#define cldm_genparams_98(type, ...)  type a97,  cldm_genparams_97(__VA_ARGS__)
#define cldm_genparams_99(type, ...)  type a98,  cldm_genparams_98(__VA_ARGS__)
#define cldm_genparams_100(type, ...) type a99,  cldm_genparams_99(__VA_ARGS__)
#define cldm_genparams_101(type, ...) type a100, cldm_genparams_100(__VA_ARGS__)
#define cldm_genparams_102(type, ...) type a101, cldm_genparams_101(__VA_ARGS__)
#define cldm_genparams_103(type, ...) type a102, cldm_genparams_102(__VA_ARGS__)
#define cldm_genparams_104(type, ...) type a103, cldm_genparams_103(__VA_ARGS__)
#define cldm_genparams_105(type, ...) type a104, cldm_genparams_104(__VA_ARGS__)
#define cldm_genparams_106(type, ...) type a105, cldm_genparams_105(__VA_ARGS__)
#define cldm_genparams_107(type, ...) type a106, cldm_genparams_106(__VA_ARGS__)
#define cldm_genparams_108(type, ...) type a107, cldm_genparams_107(__VA_ARGS__)
#define cldm_genparams_109(type, ...) type a108, cldm_genparams_108(__VA_ARGS__)
#define cldm_genparams_110(type, ...) type a109, cldm_genparams_109(__VA_ARGS__)
#define cldm_genparams_111(type, ...) type a110, cldm_genparams_110(__VA_ARGS__)
#define cldm_genparams_112(type, ...) type a111, cldm_genparams_111(__VA_ARGS__)
#define cldm_genparams_113(type, ...) type a112, cldm_genparams_112(__VA_ARGS__)
#define cldm_genparams_114(type, ...) type a113, cldm_genparams_113(__VA_ARGS__)
#define cldm_genparams_115(type, ...) type a114, cldm_genparams_114(__VA_ARGS__)
#define cldm_genparams_116(type, ...) type a115, cldm_genparams_115(__VA_ARGS__)
#define cldm_genparams_117(type, ...) type a116, cldm_genparams_116(__VA_ARGS__)
#define cldm_genparams_118(type, ...) type a117, cldm_genparams_117(__VA_ARGS__)
#define cldm_genparams_119(type, ...) type a118, cldm_genparams_118(__VA_ARGS__)
#define cldm_genparams_120(type, ...) type a119, cldm_genparams_119(__VA_ARGS__)
#define cldm_genparams_121(type, ...) type a120, cldm_genparams_120(__VA_ARGS__)
#define cldm_genparams_122(type, ...) type a121, cldm_genparams_121(__VA_ARGS__)
#define cldm_genparams_123(type, ...) type a122, cldm_genparams_122(__VA_ARGS__)
#define cldm_genparams_124(type, ...) type a123, cldm_genparams_123(__VA_ARGS__)
#define cldm_genparams_125(type, ...) type a124, cldm_genparams_124(__VA_ARGS__)
#define cldm_genparams_126(type, ...) type a125, cldm_genparams_125(__VA_ARGS__)
#define cldm_genparams_127(type, ...) type a126, cldm_genparams_126(__VA_ARGS__)

#define cldm_genparams(...) cldm_cat_expand(cldm_genparams_,cldm_count(__VA_ARGS__))(__VA_ARGS__)

#define cldm_arglist_1   a0
#define cldm_arglist_2   a1,   cldm_arglist_1
#define cldm_arglist_3   a2,   cldm_arglist_2
#define cldm_arglist_4   a3,   cldm_arglist_3
#define cldm_arglist_5   a4,   cldm_arglist_4
#define cldm_arglist_6   a5,   cldm_arglist_5
#define cldm_arglist_7   a6,   cldm_arglist_6
#define cldm_arglist_8   a7,   cldm_arglist_7
#define cldm_arglist_9   a8,   cldm_arglist_8
#define cldm_arglist_10  a9,   cldm_arglist_9
#define cldm_arglist_11  a10,  cldm_arglist_10
#define cldm_arglist_12  a11,  cldm_arglist_11
#define cldm_arglist_13  a12,  cldm_arglist_12
#define cldm_arglist_14  a13,  cldm_arglist_13
#define cldm_arglist_15  a14,  cldm_arglist_14
#define cldm_arglist_16  a15,  cldm_arglist_15
#define cldm_arglist_17  a16,  cldm_arglist_16
#define cldm_arglist_18  a17,  cldm_arglist_17
#define cldm_arglist_19  a18,  cldm_arglist_18
#define cldm_arglist_20  a19,  cldm_arglist_19
#define cldm_arglist_21  a20,  cldm_arglist_20
#define cldm_arglist_22  a21,  cldm_arglist_21
#define cldm_arglist_23  a22,  cldm_arglist_22
#define cldm_arglist_24  a23,  cldm_arglist_23
#define cldm_arglist_25  a24,  cldm_arglist_24
#define cldm_arglist_26  a25,  cldm_arglist_25
#define cldm_arglist_27  a26,  cldm_arglist_26
#define cldm_arglist_28  a27,  cldm_arglist_27
#define cldm_arglist_29  a28,  cldm_arglist_28
#define cldm_arglist_30  a29,  cldm_arglist_29
#define cldm_arglist_31  a30,  cldm_arglist_30
#define cldm_arglist_32  a31,  cldm_arglist_31
#define cldm_arglist_33  a32,  cldm_arglist_32
#define cldm_arglist_34  a33,  cldm_arglist_33
#define cldm_arglist_35  a34,  cldm_arglist_34
#define cldm_arglist_36  a35,  cldm_arglist_35
#define cldm_arglist_37  a36,  cldm_arglist_36
#define cldm_arglist_38  a37,  cldm_arglist_37
#define cldm_arglist_39  a38,  cldm_arglist_38
#define cldm_arglist_40  a39,  cldm_arglist_39
#define cldm_arglist_41  a40,  cldm_arglist_40
#define cldm_arglist_42  a41,  cldm_arglist_41
#define cldm_arglist_43  a42,  cldm_arglist_42
#define cldm_arglist_44  a43,  cldm_arglist_43
#define cldm_arglist_45  a44,  cldm_arglist_44
#define cldm_arglist_46  a45,  cldm_arglist_45
#define cldm_arglist_47  a46,  cldm_arglist_46
#define cldm_arglist_48  a47,  cldm_arglist_47
#define cldm_arglist_49  a48,  cldm_arglist_48
#define cldm_arglist_50  a49,  cldm_arglist_49
#define cldm_arglist_51  a50,  cldm_arglist_50
#define cldm_arglist_52  a51,  cldm_arglist_51
#define cldm_arglist_53  a52,  cldm_arglist_52
#define cldm_arglist_54  a53,  cldm_arglist_53
#define cldm_arglist_55  a54,  cldm_arglist_54
#define cldm_arglist_56  a55,  cldm_arglist_55
#define cldm_arglist_57  a56,  cldm_arglist_56
#define cldm_arglist_58  a57,  cldm_arglist_57
#define cldm_arglist_59  a58,  cldm_arglist_58
#define cldm_arglist_60  a59,  cldm_arglist_59
#define cldm_arglist_61  a60,  cldm_arglist_60
#define cldm_arglist_62  a61,  cldm_arglist_61
#define cldm_arglist_63  a62,  cldm_arglist_62
#define cldm_arglist_64  a63,  cldm_arglist_63
#define cldm_arglist_65  a64,  cldm_arglist_64
#define cldm_arglist_66  a65,  cldm_arglist_65
#define cldm_arglist_67  a66,  cldm_arglist_66
#define cldm_arglist_68  a67,  cldm_arglist_67
#define cldm_arglist_69  a68,  cldm_arglist_68
#define cldm_arglist_70  a69,  cldm_arglist_69
#define cldm_arglist_71  a70,  cldm_arglist_70
#define cldm_arglist_72  a71,  cldm_arglist_71
#define cldm_arglist_73  a72,  cldm_arglist_72
#define cldm_arglist_74  a73,  cldm_arglist_73
#define cldm_arglist_75  a74,  cldm_arglist_74
#define cldm_arglist_76  a75,  cldm_arglist_75
#define cldm_arglist_77  a76,  cldm_arglist_76
#define cldm_arglist_78  a77,  cldm_arglist_77
#define cldm_arglist_79  a78,  cldm_arglist_78
#define cldm_arglist_80  a79,  cldm_arglist_79
#define cldm_arglist_81  a80,  cldm_arglist_80
#define cldm_arglist_82  a81,  cldm_arglist_81
#define cldm_arglist_83  a82,  cldm_arglist_82
#define cldm_arglist_84  a83,  cldm_arglist_83
#define cldm_arglist_85  a84,  cldm_arglist_84
#define cldm_arglist_86  a85,  cldm_arglist_85
#define cldm_arglist_87  a86,  cldm_arglist_86
#define cldm_arglist_88  a87,  cldm_arglist_87
#define cldm_arglist_89  a88,  cldm_arglist_88
#define cldm_arglist_90  a89,  cldm_arglist_89
#define cldm_arglist_91  a90,  cldm_arglist_90
#define cldm_arglist_92  a91,  cldm_arglist_91
#define cldm_arglist_93  a92,  cldm_arglist_92
#define cldm_arglist_94  a93,  cldm_arglist_93
#define cldm_arglist_95  a94,  cldm_arglist_94
#define cldm_arglist_96  a95,  cldm_arglist_95
#define cldm_arglist_97  a96,  cldm_arglist_96
#define cldm_arglist_98  a97,  cldm_arglist_97
#define cldm_arglist_99  a98,  cldm_arglist_98
#define cldm_arglist_100 a99,  cldm_arglist_99
#define cldm_arglist_101 a100, cldm_arglist_100
#define cldm_arglist_102 a101, cldm_arglist_101
#define cldm_arglist_103 a102, cldm_arglist_102
#define cldm_arglist_104 a103, cldm_arglist_103
#define cldm_arglist_105 a104, cldm_arglist_104
#define cldm_arglist_106 a105, cldm_arglist_105
#define cldm_arglist_107 a106, cldm_arglist_106
#define cldm_arglist_108 a107, cldm_arglist_107
#define cldm_arglist_109 a108, cldm_arglist_108
#define cldm_arglist_110 a109, cldm_arglist_109
#define cldm_arglist_111 a110, cldm_arglist_110
#define cldm_arglist_112 a111, cldm_arglist_111
#define cldm_arglist_113 a112, cldm_arglist_112
#define cldm_arglist_114 a113, cldm_arglist_113
#define cldm_arglist_115 a114, cldm_arglist_114
#define cldm_arglist_116 a115, cldm_arglist_115
#define cldm_arglist_117 a116, cldm_arglist_116
#define cldm_arglist_118 a117, cldm_arglist_117
#define cldm_arglist_119 a118, cldm_arglist_118
#define cldm_arglist_120 a119, cldm_arglist_119
#define cldm_arglist_121 a120, cldm_arglist_120
#define cldm_arglist_122 a121, cldm_arglist_121
#define cldm_arglist_123 a122, cldm_arglist_122
#define cldm_arglist_124 a123, cldm_arglist_123
#define cldm_arglist_125 a124, cldm_arglist_124
#define cldm_arglist_126 a125, cldm_arglist_125
#define cldm_arglist_127 a126, cldm_arglist_126

#define cldm_arglist(size) cldm_cat(cldm_arglist_, size)

#define cldm_mock_function2(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function3(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function4(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function5(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function6(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function7(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function8(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function9(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function10(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function11(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function12(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function13(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function14(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function15(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function16(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function17(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function18(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function19(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function20(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function21(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function22(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function23(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function24(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function25(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function26(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function27(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function28(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function29(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function30(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function31(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function32(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function33(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function34(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function35(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function36(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function37(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function38(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function39(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function40(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function41(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function42(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function43(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function44(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function45(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function46(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function47(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function48(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function49(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function50(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function51(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function52(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function53(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function54(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function55(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function56(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function57(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function58(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function59(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function60(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function61(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function62(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function63(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function64(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function65(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function66(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function67(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function68(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function69(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function70(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function71(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function72(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function73(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function74(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function75(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function76(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function77(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function78(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function79(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function80(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function81(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function82(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function83(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function84(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function85(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function86(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function87(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function88(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function89(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function90(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function91(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function92(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function93(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function94(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function95(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function96(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function97(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function98(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function99(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function100(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function101(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function102(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function103(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function104(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function105(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function106(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function107(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function108(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function109(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function110(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function111(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function112(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function113(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function114(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function115(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function116(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function117(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function118(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function119(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function120(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function121(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function122(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function123(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function124(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function125(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function126(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function127(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function128(...) cldm_mock_function(__VA_ARGS__)

#define cldm_argframe1(addrs, type)        cldm_argframe_populate_index(addrs, 0)
#define cldm_argframe2(addrs, type, ...)   cldm_argframe_populate_index(addrs, 1);   cldm_argframe1(addrs, __VA_ARGS__)
#define cldm_argframe3(addrs, type, ...)   cldm_argframe_populate_index(addrs, 2);   cldm_argframe2(addrs, __VA_ARGS__)
#define cldm_argframe4(addrs, type, ...)   cldm_argframe_populate_index(addrs, 3);   cldm_argframe3(addrs, __VA_ARGS__)
#define cldm_argframe5(addrs, type, ...)   cldm_argframe_populate_index(addrs, 4);   cldm_argframe4(addrs, __VA_ARGS__)
#define cldm_argframe6(addrs, type, ...)   cldm_argframe_populate_index(addrs, 5);   cldm_argframe5(addrs, __VA_ARGS__)
#define cldm_argframe7(addrs, type, ...)   cldm_argframe_populate_index(addrs, 6);   cldm_argframe6(addrs, __VA_ARGS__)
#define cldm_argframe8(addrs, type, ...)   cldm_argframe_populate_index(addrs, 7);   cldm_argframe7(addrs, __VA_ARGS__)
#define cldm_argframe9(addrs, type, ...)   cldm_argframe_populate_index(addrs, 8);   cldm_argframe8(addrs, __VA_ARGS__)
#define cldm_argframe10(addrs, type, ...)  cldm_argframe_populate_index(addrs, 9);   cldm_argframe9(addrs, __VA_ARGS__)
#define cldm_argframe11(addrs, type, ...)  cldm_argframe_populate_index(addrs, 10);  cldm_argframe10(addrs, __VA_ARGS__)
#define cldm_argframe12(addrs, type, ...)  cldm_argframe_populate_index(addrs, 11);  cldm_argframe11(addrs, __VA_ARGS__)
#define cldm_argframe13(addrs, type, ...)  cldm_argframe_populate_index(addrs, 12);  cldm_argframe12(addrs, __VA_ARGS__)
#define cldm_argframe14(addrs, type, ...)  cldm_argframe_populate_index(addrs, 13);  cldm_argframe13(addrs, __VA_ARGS__)
#define cldm_argframe15(addrs, type, ...)  cldm_argframe_populate_index(addrs, 14);  cldm_argframe14(addrs, __VA_ARGS__)
#define cldm_argframe16(addrs, type, ...)  cldm_argframe_populate_index(addrs, 15);  cldm_argframe15(addrs, __VA_ARGS__)
#define cldm_argframe17(addrs, type, ...)  cldm_argframe_populate_index(addrs, 16);  cldm_argframe16(addrs, __VA_ARGS__)
#define cldm_argframe18(addrs, type, ...)  cldm_argframe_populate_index(addrs, 17);  cldm_argframe17(addrs, __VA_ARGS__)
#define cldm_argframe19(addrs, type, ...)  cldm_argframe_populate_index(addrs, 18);  cldm_argframe18(addrs, __VA_ARGS__)
#define cldm_argframe20(addrs, type, ...)  cldm_argframe_populate_index(addrs, 19);  cldm_argframe19(addrs, __VA_ARGS__)
#define cldm_argframe21(addrs, type, ...)  cldm_argframe_populate_index(addrs, 20);  cldm_argframe20(addrs, __VA_ARGS__)
#define cldm_argframe22(addrs, type, ...)  cldm_argframe_populate_index(addrs, 21);  cldm_argframe21(addrs, __VA_ARGS__)
#define cldm_argframe23(addrs, type, ...)  cldm_argframe_populate_index(addrs, 22);  cldm_argframe22(addrs, __VA_ARGS__)
#define cldm_argframe24(addrs, type, ...)  cldm_argframe_populate_index(addrs, 23);  cldm_argframe23(addrs, __VA_ARGS__)
#define cldm_argframe25(addrs, type, ...)  cldm_argframe_populate_index(addrs, 24);  cldm_argframe24(addrs, __VA_ARGS__)
#define cldm_argframe26(addrs, type, ...)  cldm_argframe_populate_index(addrs, 25);  cldm_argframe25(addrs, __VA_ARGS__)
#define cldm_argframe27(addrs, type, ...)  cldm_argframe_populate_index(addrs, 26);  cldm_argframe26(addrs, __VA_ARGS__)
#define cldm_argframe28(addrs, type, ...)  cldm_argframe_populate_index(addrs, 27);  cldm_argframe27(addrs, __VA_ARGS__)
#define cldm_argframe29(addrs, type, ...)  cldm_argframe_populate_index(addrs, 28);  cldm_argframe28(addrs, __VA_ARGS__)
#define cldm_argframe30(addrs, type, ...)  cldm_argframe_populate_index(addrs, 29);  cldm_argframe29(addrs, __VA_ARGS__)
#define cldm_argframe31(addrs, type, ...)  cldm_argframe_populate_index(addrs, 30);  cldm_argframe30(addrs, __VA_ARGS__)
#define cldm_argframe32(addrs, type, ...)  cldm_argframe_populate_index(addrs, 31);  cldm_argframe31(addrs, __VA_ARGS__)
#define cldm_argframe33(addrs, type, ...)  cldm_argframe_populate_index(addrs, 32);  cldm_argframe32(addrs, __VA_ARGS__)
#define cldm_argframe34(addrs, type, ...)  cldm_argframe_populate_index(addrs, 33);  cldm_argframe33(addrs, __VA_ARGS__)
#define cldm_argframe35(addrs, type, ...)  cldm_argframe_populate_index(addrs, 34);  cldm_argframe34(addrs, __VA_ARGS__)
#define cldm_argframe36(addrs, type, ...)  cldm_argframe_populate_index(addrs, 35);  cldm_argframe35(addrs, __VA_ARGS__)
#define cldm_argframe37(addrs, type, ...)  cldm_argframe_populate_index(addrs, 36);  cldm_argframe36(addrs, __VA_ARGS__)
#define cldm_argframe38(addrs, type, ...)  cldm_argframe_populate_index(addrs, 37);  cldm_argframe37(addrs, __VA_ARGS__)
#define cldm_argframe39(addrs, type, ...)  cldm_argframe_populate_index(addrs, 38);  cldm_argframe38(addrs, __VA_ARGS__)
#define cldm_argframe40(addrs, type, ...)  cldm_argframe_populate_index(addrs, 39);  cldm_argframe39(addrs, __VA_ARGS__)
#define cldm_argframe41(addrs, type, ...)  cldm_argframe_populate_index(addrs, 40);  cldm_argframe40(addrs, __VA_ARGS__)
#define cldm_argframe42(addrs, type, ...)  cldm_argframe_populate_index(addrs, 41);  cldm_argframe41(addrs, __VA_ARGS__)
#define cldm_argframe43(addrs, type, ...)  cldm_argframe_populate_index(addrs, 42);  cldm_argframe42(addrs, __VA_ARGS__)
#define cldm_argframe44(addrs, type, ...)  cldm_argframe_populate_index(addrs, 43);  cldm_argframe43(addrs, __VA_ARGS__)
#define cldm_argframe45(addrs, type, ...)  cldm_argframe_populate_index(addrs, 44);  cldm_argframe44(addrs, __VA_ARGS__)
#define cldm_argframe46(addrs, type, ...)  cldm_argframe_populate_index(addrs, 45);  cldm_argframe45(addrs, __VA_ARGS__)
#define cldm_argframe47(addrs, type, ...)  cldm_argframe_populate_index(addrs, 46);  cldm_argframe46(addrs, __VA_ARGS__)
#define cldm_argframe48(addrs, type, ...)  cldm_argframe_populate_index(addrs, 47);  cldm_argframe47(addrs, __VA_ARGS__)
#define cldm_argframe49(addrs, type, ...)  cldm_argframe_populate_index(addrs, 48);  cldm_argframe48(addrs, __VA_ARGS__)
#define cldm_argframe50(addrs, type, ...)  cldm_argframe_populate_index(addrs, 49);  cldm_argframe49(addrs, __VA_ARGS__)
#define cldm_argframe51(addrs, type, ...)  cldm_argframe_populate_index(addrs, 50);  cldm_argframe50(addrs, __VA_ARGS__)
#define cldm_argframe52(addrs, type, ...)  cldm_argframe_populate_index(addrs, 51);  cldm_argframe51(addrs, __VA_ARGS__)
#define cldm_argframe53(addrs, type, ...)  cldm_argframe_populate_index(addrs, 52);  cldm_argframe52(addrs, __VA_ARGS__)
#define cldm_argframe54(addrs, type, ...)  cldm_argframe_populate_index(addrs, 53);  cldm_argframe53(addrs, __VA_ARGS__)
#define cldm_argframe55(addrs, type, ...)  cldm_argframe_populate_index(addrs, 54);  cldm_argframe54(addrs, __VA_ARGS__)
#define cldm_argframe56(addrs, type, ...)  cldm_argframe_populate_index(addrs, 55);  cldm_argframe55(addrs, __VA_ARGS__)
#define cldm_argframe57(addrs, type, ...)  cldm_argframe_populate_index(addrs, 56);  cldm_argframe56(addrs, __VA_ARGS__)
#define cldm_argframe58(addrs, type, ...)  cldm_argframe_populate_index(addrs, 57);  cldm_argframe57(addrs, __VA_ARGS__)
#define cldm_argframe59(addrs, type, ...)  cldm_argframe_populate_index(addrs, 58);  cldm_argframe58(addrs, __VA_ARGS__)
#define cldm_argframe60(addrs, type, ...)  cldm_argframe_populate_index(addrs, 59);  cldm_argframe59(addrs, __VA_ARGS__)
#define cldm_argframe61(addrs, type, ...)  cldm_argframe_populate_index(addrs, 60);  cldm_argframe60(addrs, __VA_ARGS__)
#define cldm_argframe62(addrs, type, ...)  cldm_argframe_populate_index(addrs, 61);  cldm_argframe61(addrs, __VA_ARGS__)
#define cldm_argframe63(addrs, type, ...)  cldm_argframe_populate_index(addrs, 62);  cldm_argframe62(addrs, __VA_ARGS__)
#define cldm_argframe64(addrs, type, ...)  cldm_argframe_populate_index(addrs, 63);  cldm_argframe63(addrs, __VA_ARGS__)
#define cldm_argframe65(addrs, type, ...)  cldm_argframe_populate_index(addrs, 64);  cldm_argframe64(addrs, __VA_ARGS__)
#define cldm_argframe66(addrs, type, ...)  cldm_argframe_populate_index(addrs, 65);  cldm_argframe65(addrs, __VA_ARGS__)
#define cldm_argframe67(addrs, type, ...)  cldm_argframe_populate_index(addrs, 66);  cldm_argframe66(addrs, __VA_ARGS__)
#define cldm_argframe68(addrs, type, ...)  cldm_argframe_populate_index(addrs, 67);  cldm_argframe67(addrs, __VA_ARGS__)
#define cldm_argframe69(addrs, type, ...)  cldm_argframe_populate_index(addrs, 68);  cldm_argframe68(addrs, __VA_ARGS__)
#define cldm_argframe70(addrs, type, ...)  cldm_argframe_populate_index(addrs, 69);  cldm_argframe69(addrs, __VA_ARGS__)
#define cldm_argframe71(addrs, type, ...)  cldm_argframe_populate_index(addrs, 70);  cldm_argframe70(addrs, __VA_ARGS__)
#define cldm_argframe72(addrs, type, ...)  cldm_argframe_populate_index(addrs, 71);  cldm_argframe71(addrs, __VA_ARGS__)
#define cldm_argframe73(addrs, type, ...)  cldm_argframe_populate_index(addrs, 72);  cldm_argframe72(addrs, __VA_ARGS__)
#define cldm_argframe74(addrs, type, ...)  cldm_argframe_populate_index(addrs, 73);  cldm_argframe73(addrs, __VA_ARGS__)
#define cldm_argframe75(addrs, type, ...)  cldm_argframe_populate_index(addrs, 74);  cldm_argframe74(addrs, __VA_ARGS__)
#define cldm_argframe76(addrs, type, ...)  cldm_argframe_populate_index(addrs, 75);  cldm_argframe75(addrs, __VA_ARGS__)
#define cldm_argframe77(addrs, type, ...)  cldm_argframe_populate_index(addrs, 76);  cldm_argframe76(addrs, __VA_ARGS__)
#define cldm_argframe78(addrs, type, ...)  cldm_argframe_populate_index(addrs, 77);  cldm_argframe77(addrs, __VA_ARGS__)
#define cldm_argframe79(addrs, type, ...)  cldm_argframe_populate_index(addrs, 78);  cldm_argframe78(addrs, __VA_ARGS__)
#define cldm_argframe80(addrs, type, ...)  cldm_argframe_populate_index(addrs, 79);  cldm_argframe79(addrs, __VA_ARGS__)
#define cldm_argframe81(addrs, type, ...)  cldm_argframe_populate_index(addrs, 80);  cldm_argframe80(addrs, __VA_ARGS__)
#define cldm_argframe82(addrs, type, ...)  cldm_argframe_populate_index(addrs, 81);  cldm_argframe81(addrs, __VA_ARGS__)
#define cldm_argframe83(addrs, type, ...)  cldm_argframe_populate_index(addrs, 82);  cldm_argframe82(addrs, __VA_ARGS__)
#define cldm_argframe84(addrs, type, ...)  cldm_argframe_populate_index(addrs, 83);  cldm_argframe83(addrs, __VA_ARGS__)
#define cldm_argframe85(addrs, type, ...)  cldm_argframe_populate_index(addrs, 84);  cldm_argframe84(addrs, __VA_ARGS__)
#define cldm_argframe86(addrs, type, ...)  cldm_argframe_populate_index(addrs, 85);  cldm_argframe85(addrs, __VA_ARGS__)
#define cldm_argframe87(addrs, type, ...)  cldm_argframe_populate_index(addrs, 86);  cldm_argframe86(addrs, __VA_ARGS__)
#define cldm_argframe88(addrs, type, ...)  cldm_argframe_populate_index(addrs, 87);  cldm_argframe87(addrs, __VA_ARGS__)
#define cldm_argframe89(addrs, type, ...)  cldm_argframe_populate_index(addrs, 88);  cldm_argframe88(addrs, __VA_ARGS__)
#define cldm_argframe90(addrs, type, ...)  cldm_argframe_populate_index(addrs, 89);  cldm_argframe89(addrs, __VA_ARGS__)
#define cldm_argframe91(addrs, type, ...)  cldm_argframe_populate_index(addrs, 90);  cldm_argframe90(addrs, __VA_ARGS__)
#define cldm_argframe92(addrs, type, ...)  cldm_argframe_populate_index(addrs, 91);  cldm_argframe91(addrs, __VA_ARGS__)
#define cldm_argframe93(addrs, type, ...)  cldm_argframe_populate_index(addrs, 92);  cldm_argframe92(addrs, __VA_ARGS__)
#define cldm_argframe94(addrs, type, ...)  cldm_argframe_populate_index(addrs, 93);  cldm_argframe93(addrs, __VA_ARGS__)
#define cldm_argframe95(addrs, type, ...)  cldm_argframe_populate_index(addrs, 94);  cldm_argframe94(addrs, __VA_ARGS__)
#define cldm_argframe96(addrs, type, ...)  cldm_argframe_populate_index(addrs, 95);  cldm_argframe95(addrs, __VA_ARGS__)
#define cldm_argframe97(addrs, type, ...)  cldm_argframe_populate_index(addrs, 96);  cldm_argframe96(addrs, __VA_ARGS__)
#define cldm_argframe98(addrs, type, ...)  cldm_argframe_populate_index(addrs, 97);  cldm_argframe97(addrs, __VA_ARGS__)
#define cldm_argframe99(addrs, type, ...)  cldm_argframe_populate_index(addrs, 98);  cldm_argframe98(addrs, __VA_ARGS__)
#define cldm_argframe100(addrs, type, ...) cldm_argframe_populate_index(addrs, 99);  cldm_argframe99(addrs, __VA_ARGS__)
#define cldm_argframe101(addrs, type, ...) cldm_argframe_populate_index(addrs, 100); cldm_argframe100(addrs, __VA_ARGS__)
#define cldm_argframe102(addrs, type, ...) cldm_argframe_populate_index(addrs, 101); cldm_argframe101(addrs, __VA_ARGS__)
#define cldm_argframe103(addrs, type, ...) cldm_argframe_populate_index(addrs, 102); cldm_argframe102(addrs, __VA_ARGS__)
#define cldm_argframe104(addrs, type, ...) cldm_argframe_populate_index(addrs, 103); cldm_argframe103(addrs, __VA_ARGS__)
#define cldm_argframe105(addrs, type, ...) cldm_argframe_populate_index(addrs, 104); cldm_argframe104(addrs, __VA_ARGS__)
#define cldm_argframe106(addrs, type, ...) cldm_argframe_populate_index(addrs, 105); cldm_argframe105(addrs, __VA_ARGS__)
#define cldm_argframe107(addrs, type, ...) cldm_argframe_populate_index(addrs, 106); cldm_argframe106(addrs, __VA_ARGS__)
#define cldm_argframe108(addrs, type, ...) cldm_argframe_populate_index(addrs, 107); cldm_argframe107(addrs, __VA_ARGS__)
#define cldm_argframe109(addrs, type, ...) cldm_argframe_populate_index(addrs, 108); cldm_argframe108(addrs, __VA_ARGS__)
#define cldm_argframe110(addrs, type, ...) cldm_argframe_populate_index(addrs, 109); cldm_argframe109(addrs, __VA_ARGS__)
#define cldm_argframe111(addrs, type, ...) cldm_argframe_populate_index(addrs, 110); cldm_argframe110(addrs, __VA_ARGS__)
#define cldm_argframe112(addrs, type, ...) cldm_argframe_populate_index(addrs, 111); cldm_argframe111(addrs, __VA_ARGS__)
#define cldm_argframe113(addrs, type, ...) cldm_argframe_populate_index(addrs, 112); cldm_argframe112(addrs, __VA_ARGS__)
#define cldm_argframe114(addrs, type, ...) cldm_argframe_populate_index(addrs, 113); cldm_argframe113(addrs, __VA_ARGS__)
#define cldm_argframe115(addrs, type, ...) cldm_argframe_populate_index(addrs, 114); cldm_argframe114(addrs, __VA_ARGS__)
#define cldm_argframe116(addrs, type, ...) cldm_argframe_populate_index(addrs, 115); cldm_argframe115(addrs, __VA_ARGS__)
#define cldm_argframe117(addrs, type, ...) cldm_argframe_populate_index(addrs, 116); cldm_argframe116(addrs, __VA_ARGS__)
#define cldm_argframe118(addrs, type, ...) cldm_argframe_populate_index(addrs, 117); cldm_argframe117(addrs, __VA_ARGS__)
#define cldm_argframe119(addrs, type, ...) cldm_argframe_populate_index(addrs, 118); cldm_argframe118(addrs, __VA_ARGS__)
#define cldm_argframe120(addrs, type, ...) cldm_argframe_populate_index(addrs, 119); cldm_argframe119(addrs, __VA_ARGS__)
#define cldm_argframe121(addrs, type, ...) cldm_argframe_populate_index(addrs, 120); cldm_argframe120(addrs, __VA_ARGS__)
#define cldm_argframe122(addrs, type, ...) cldm_argframe_populate_index(addrs, 121); cldm_argframe121(addrs, __VA_ARGS__)
#define cldm_argframe123(addrs, type, ...) cldm_argframe_populate_index(addrs, 122); cldm_argframe122(addrs, __VA_ARGS__)
#define cldm_argframe124(addrs, type, ...) cldm_argframe_populate_index(addrs, 123); cldm_argframe123(addrs, __VA_ARGS__)
#define cldm_argframe125(addrs, type, ...) cldm_argframe_populate_index(addrs, 124); cldm_argframe124(addrs, __VA_ARGS__)
#define cldm_argframe126(addrs, type, ...) cldm_argframe_populate_index(addrs, 125); cldm_argframe125(addrs, __VA_ARGS__)
#define cldm_argframe127(addrs, type, ...) cldm_argframe_populate_index(addrs, 126); cldm_argframe126(addrs, __VA_ARGS__)

#define cldm_argframe_populate_index(addrs, index)    \
    addrs[cldm_arrsize(addrs)- index - 1] = (void const *)&cldm_cat_expand(a, index)

#define cldm_argframe_populate(addrs, ...) \
    cldm_cat_expand(cldm_argframe, cldm_count(__VA_ARGS__))(addrs, __VA_ARGS__)

#define cldm_mock_function0_1(rettype, name) CLDM_MOCK_FUNCTION(rettype, name)
#define cldm_mock_function0_2(rettype, name, _) CLDM_MOCK_FUNCTION(rettype, name)

#define cldm_mock_function_void0_1(name) CLDM_MOCK_FUNCTION_VOID(name)
#define cldm_mock_function_void0_2(name, _) CLDM_MOCK_FUNCTION_VOID(name)

#define cldm_will(invocations, ...)                                                                     \
        __VA_ARGS__;                                                                                    \
        *(int *)((unsigned char *)mockinfo->addr + mockinfo->invocations_offset) = invocations;         \
        cldm_rtassert((*(int *)((unsigned char *)mockinfo->addr + mockinfo->invocations_offset)) > -2); \
    } while (0)

#define cldm_set_opmode(mode)                                                               \
    enum cldm_opmode *cldm_cat_expand(cldm_opdata, __LINE__) =                              \
        (enum cldm_opmode *)((unsigned char*)mockinfo->addr + mockinfo->opmode_offset);     \
    *cldm_cat_expand(cldm_opdata, __LINE__) = mode

#define cldm_setop(field, value, mode)      \
    opdata.field = value;                   \
    cldm_set_opmode(mode)

#define cldm_assign2(lhs, rhs)              \
    cldm_setop(act.assign_ctx, ((struct cldm_assign_ctx){ &lhs, &rhs, sizeof(lhs) }), cldm_op_assign)

#define cldm_assign3(lhs, rhs, type)        \
    cldm_assign2(lhs, (type){ rhs })

struct cldm_mock_info {
    void *addr;
    unsigned invocations_offset;
    unsigned opmode_offset;
};

struct cldm_assign_ctx {
    void *lhs;
    void const *rhs;
    unsigned argsize;
};

struct cldm_assignarg_ctx {
    void *addr;
    unsigned argidx;
    unsigned argsize;
};

enum cldm_opmode {
    cldm_op_invoke,
    cldm_op_return,
    cldm_op_increment,
    cldm_op_retarg,
    cldm_op_retpointee,
    cldm_op_assign,
    cldm_op_assignarg
};

#define cldm_generate_mock_ctx(utype, rettype, name, ...)   \
    struct cldm_mock_ ## name ## _ctx {                     \
        struct cldm_mock_info info;                         \
        int invocations;                                    \
        struct cldm_mock_ ## name ## _opdata{               \
            enum cldm_opmode mode;                          \
            union {                                         \
                rettype(*invoke)(__VA_ARGS__);              \
                utype retval;                               \
                utype counter;                              \
                unsigned argindex;                          \
                struct cldm_assign_ctx assign_ctx;          \
                struct cldm_assignarg_ctx assignarg_ctx;    \
            } act;                                          \
        } opdata;                                           \
    }

#ifdef CLDM_GENERATE_SYMBOLS
#define cldm_mock_function(rvinit, call_prefix, retstatement, utype, rettype, name, ...)                            \
    cldm_generate_mock_ctx(utype, rettype, name, __VA_ARGS__);                                                      \
    struct cldm_mock_ ## name ## _ctx cldm_mock_ ## name = {                                                        \
        .info = {                                                                                                   \
            .addr = &cldm_mock_ ## name,                                                                            \
            .invocations_offset = cldm_offset(struct cldm_mock_ ## name ## _ctx, invocations),                      \
            .opmode_offset = cldm_offset(struct cldm_mock_ ## name ## _ctx, opdata) +                               \
                             cldm_offset(struct cldm_mock_ ## name ## _opdata, mode)                                \
        },                                                                                                          \
        .invocations = 0,                                                                                           \
        .opdata = {                                                                                                 \
            .mode = cldm_op_invoke,                                                                                 \
            .act.invoke = 0                                                                                         \
        }                                                                                                           \
    };                                                                                                              \
    rettype name(cldm_genparams(__VA_ARGS__)) {                                                                     \
        rvinit;                                                                                                     \
        void const *argaddrs[cldm_count(__VA_ARGS__)];                                                              \
        cldm_argframe_populate(argaddrs, __VA_ARGS__);                                                              \
        extern bool cldm_mock_force_disable;                                                                        \
        if(!cldm_mock_force_disable && cldm_mock_ ## name.invocations) {                                            \
            if(cldm_mock_ ## name.invocations != -1) {                                                              \
                --cldm_mock_ ## name.invocations;                                                                   \
            }                                                                                                       \
            switch(cldm_mock_ ## name.opdata.mode) {                                                                \
                case cldm_op_invoke:                                                                                \
                    call_prefix cldm_mock_ ## name.opdata.act.invoke(cldm_arglist(cldm_count(__VA_ARGS__)));        \
                    break;                                                                                          \
                case cldm_op_return:                                                                                \
                    call_prefix cldm_mock_ ## name.opdata.act.retval;                                               \
                    break;                                                                                          \
                case cldm_op_increment:                                                                             \
                    call_prefix ++cldm_mock_ ## name.opdata.act.counter;                                            \
                    break;                                                                                          \
                case cldm_op_retarg:                                                                                \
                    cldm_rtassert(cldm_mock_ ## name.opdata.act.argindex < cldm_arrsize(argaddrs),                  \
                                 "Attempt to access parameter %u in function taking only %zu",                      \
                                 cldm_mock_ ## name.opdata.act.argindex + 1, cldm_arrsize(argaddrs));               \
                    call_prefix *(utype *)argaddrs[cldm_mock_ ## name.opdata.act.argindex];                         \
                    break;                                                                                          \
                case cldm_op_retpointee:                                                                            \
                    cldm_rtassert(cldm_mock_ ## name.opdata.act.argindex < cldm_arrsize(argaddrs),                  \
                                 "Attempt to access parameter %u in function taking only %zu",                      \
                                 cldm_mock_ ## name.opdata.act.argindex + 1, cldm_arrsize(argaddrs));               \
                    call_prefix **(utype **)argaddrs[cldm_mock_ ## name.opdata.act.argindex];                       \
                    break;                                                                                          \
                case cldm_op_assign:                                                                                \
                    memcpy(cldm_mock_ ## name.opdata.act.assign_ctx.lhs,                                            \
                           cldm_mock_ ## name.opdata.act.assign_ctx.rhs,                                            \
                           cldm_mock_ ## name.opdata.act.assign_ctx.argsize);                                       \
                    break;                                                                                          \
                case cldm_op_assignarg:                                                                             \
                    cldm_rtassert(cldm_mock_ ## name.opdata.act.assignarg_ctx.argidx < cldm_arrsize(argaddrs),      \
                                 "Attempt to access parameter %u in function taking only %zu",                      \
                                 cldm_mock_ ## name.opdata.act.assignarg_ctx.argidx + 1, cldm_arrsize(argaddrs));   \
                    memcpy(cldm_mock_ ## name.opdata.act.assignarg_ctx.addr,                                        \
                           argaddrs[cldm_mock_ ## name.opdata.act.assignarg_ctx.argidx],                            \
                           cldm_mock_ ## name.opdata.act.assignarg_ctx.argsize);                                    \
                    break;                                                                                          \
                default:                                                                                            \
                    cldm_rtassert(0, "Invalid opmode %d", cldm_mock_ ## name.opdata.mode);                          \
            }                                                                                                       \
            retstatement;                                                                                           \
        }                                                                                                           \
        rettype(* cldm_handle_ ## name)(__VA_ARGS__);                                                               \
        *(void **) (& cldm_handle_ ## name) = cldm_dlsym_next(#name);                                               \
        cldm_rtassert(cldm_handle_ ## name);                                                                        \
        call_prefix cldm_handle_ ## name(cldm_arglist(cldm_count(__VA_ARGS__)));                                    \
        retstatement;                                                                                               \
    }                                                                                                               \
    void cldm_trailing_ ## name (void)

#define cldm_mock_function1(rvinit, call_prefix, retstatement, utype, rettype, name)            \
    cldm_generate_mock_ctx(utype, rettype, name, void);                                         \
    struct cldm_mock_ ## name ## _ctx cldm_mock_ ## name = {                                    \
        .info = {                                                                               \
            .addr = &cldm_mock_ ## name,                                                        \
            .invocations_offset = cldm_offset(struct cldm_mock_ ## name ## _ctx, invocations),  \
            .opmode_offset = cldm_offset(struct cldm_mock_ ## name ## _ctx, opdata) +           \
                             cldm_offset(struct cldm_mock_ ## name ## _opdata, mode)            \
        },                                                                                      \
        .invocations = 0,                                                                       \
        .opdata = {                                                                             \
            .mode = cldm_op_invoke,                                                             \
            .act.invoke = 0                                                                     \
        }                                                                                       \
    };                                                                                          \
    rettype name(void) {                                                                        \
        rvinit;                                                                                 \
        extern bool cldm_mock_force_disable;                                                    \
        if(!cldm_mock_force_disable && cldm_mock_ ## name.invocations) {                        \
            if(cldm_mock_ ## name.invocations != -1) {                                          \
                --cldm_mock_ ## name.invocations;                                               \
            }                                                                                   \
            switch(cldm_mock_ ## name.opdata.mode) {                                            \
                case cldm_op_invoke:                                                            \
                    call_prefix cldm_mock_ ## name.opdata.act.invoke();                         \
                    break;                                                                      \
                case cldm_op_return:                                                            \
                    call_prefix cldm_mock_ ## name.opdata.act.retval;                           \
                    break;                                                                      \
                case cldm_op_increment:                                                         \
                    call_prefix ++cldm_mock_ ## name.opdata.act.counter;                        \
                    break;                                                                      \
                case cldm_op_assign:                                                            \
                    memcpy(cldm_mock_ ## name.opdata.act.assign_ctx.lhs,                        \
                           cldm_mock_ ## name.opdata.act.assign_ctx.rhs,                        \
                           cldm_mock_ ## name.opdata.act.assign_ctx.argsize);                   \
                    break;                                                                      \
                default:                                                                        \
                    cldm_rtassert(0, "Invalid opmode %d", cldm_mock_ ## name.opdata.mode);      \
            }                                                                                   \
            retstatement;                                                                       \
        }                                                                                       \
        rettype(* cldm_handle_ ## name)(void);                                                  \
        *(void **) (& cldm_handle_ ## name) = cldm_dlsym_next(#name);                           \
        cldm_rtassert(cldm_handle_ ## name);                                                    \
        call_prefix cldm_handle_ ## name ();                                                    \
        retstatement;                                                                           \
    }                                                                                           \
    void cldm_trailing_ ## name (void)
#else

#define cldm_mock_function(rvinit, call_prefix, retstatement, utype, rettype, name, ...)    \
    cldm_generate_mock_ctx(utype, rettype, name, __VA_ARGS__)

#define cldm_mock_function1(rvinit, call_prefix, retstatement, utype, rettype, name)        \
    cldm_generate_mock_ctx(utype, rettype, name, void)
#endif

#define CLDM_MOCK_FUNCTION(rettype, ...)                                                                            \
    cldm_cat_expand(cldm_mock_function, cldm_count(__VA_ARGS__))(rettype cldm_cat_expand(cldm_rv_, __LINE__),       \
                                                                    cldm_cat_expand(cldm_rv_, __LINE__) =,          \
                                                                    return cldm_cat_expand(cldm_rv_, __LINE__),     \
                                                                    rettype, rettype, __VA_ARGS__)

#define CLDM_MOCK_FUNCTION0(rettype, ...)   \
    cldm_cat_expand(cldm_mock_function0_, cldm_count(__VA_ARGS__))(rettype, __VA_ARGS__)


#define CLDM_MOCK_FUNCTION_VOID(...)    \
    cldm_cat_expand(cldm_mock_function, cldm_count(__VA_ARGS__))((void)0, (void), return, int, void, __VA_ARGS__)

#define CLDM_MOCK_FUNCTION_VOID0(...)   \
    cldm_cat_expand(cldm_mock_function_void0_, cldm_count(__VA_ARGS__))(__VA_ARGS__)

#define CLDM_EXPECT_CALL(name)                                          \
    do {                                                                \
        extern struct cldm_mock_ ## name ## _ctx cldm_mock_ ## name;    \
        /* @todo work around need for global state */                   \
        extern struct cldm_mock_info *mockinfo;                         \
        mockinfo = &cldm_mock_ ## name.info;                            \
        cldm_mock_ ## name

#define CLDM_WILL_ONCE(...) cldm_will(1, __VA_ARGS__)
#define CLDM_WILL_REPEATEDLY(...) cldm_will(-1, __VA_ARGS__)
#define CLDM_WILL_N_TIMES(n, ...) cldm_will(n, __VA_ARGS__)
#define CLDM_WILL_INVOKE_DEFAULT() cldm_will(0, opdata.act.invoke = cldm_op_invoke)

#define CLDM_INVOKE(func)            cldm_setop(act.invoke, func, cldm_op_invoke)
#define CLDM_RETURN(value)           cldm_setop(act.retval, value, cldm_op_return)
#define CLDM_RETURN_ARG(index)       cldm_setop(act.argindex, index, cldm_op_retarg)
#define CLDM_RETURN_POINTEE(index)   cldm_setop(act.argindex, index, cldm_op_retpointee)
#define CLDM_INCREMENT_COUNTER(init) cldm_setop(act.counter, init, cldm_op_increment)
#define CLDM_ASSIGN(...)             cldm_overload(cldm_assign,__VA_ARGS__)
#define CLDM_ASSIGN_ARG(index, lhs)  cldm_setop(act.assignarg_ctx, ((struct cldm_assignarg_ctx){ &lhs, index, sizeof(lhs) }), cldm_op_assignarg)


#ifndef CLDM_PREFIX_ONLY
#define MOCK_FUNCTION(...)       CLDM_MOCK_FUNCTION(__VA_ARGS__)
#define MOCK_FUNCTION0(...)      CLDM_MOCK_FUNCTION0(__VA_ARGS__)
#define MOCK_FUNCTION_VOID(...)  CLDM_MOCK_FUNCTION_VOID(__VA_ARGS__)
#define MOCK_FUNCTION_VOID0(...) CLDM_MOCK_FUNCTION_VOID0(__VA_ARGS__)
#define EXPECT_CALL(...)         CLDM_EXPECT_CALL(__VA_ARGS__)
#define WILL_ONCE(...)           CLDM_WILL_ONCE(__VA_ARGS__)
#define WILL_REPEATEDLY(...)     CLDM_WILL_REPEATEDLY(__VA_ARGS__)
#define WILL_N_TIMES(...)        CLDM_WILL_N_TIMES(__VA_ARGS__)
#define WILL_INVOKE_DEFAULT()    CLDM_WILL_INVOKE_DEFAULT()
#define INVOKE(...)              CLDM_INVOKE(__VA_ARGS__)
#define RETURN(...)              CLDM_RETURN(__VA_ARGS__)
#define RETURN_ARG(...)          CLDM_RETURN_ARG(__VA_ARGS__)
#define RETURN_POINTEE(...)      CLDM_RETURN_POINTEE(__VA_ARGS__)
#define INCREMENT_COUNTER(...)   CLDM_INCREMENT_COUNTER(__VA_ARGS__)
#define ASSIGN(...)              CLDM_ASSIGN(__VA_ARGS__)
#define ASSIGN_ARG(...)          CLDM_ASSIGN_ARG(__VA_ARGS__)
#endif

#ifdef CLDM_GMOCK_COMPAT
#define WillOnce(...)         CLDM_WILL_ONCE(__VA_ARGS__)
#define WillRepeatedly(...)   CLDM_WILL_REPEATEDLY(__VA_ARGS__)
#define WillNTimes(...)       CLDM_WILL_N_TIMES(__VA_ARGS__)
#define WillInvokeDefault()   CLDM_WILL_INVOKE_DEFAULT()
#define Invoke(...)           CLDM_INVOKE(__VA_ARGS__)
#define Return(...)           CLDM_RETURN(__VA_ARGS__)
#define ReturnArg(...)        CLDM_RETURN_ARG(__VA_ARGS__)
#define ReturnPointee(...)    CLDM_RETURN_POINTEE(__VA_ARGS__)
#define IncrementCounter(...) CLDM_INCREMENT_COUNTER(__VA_ARGS__)
#define Assign(...)           CLDM_ASSIGN(__VA_ARGS__)
#define AssignArg(...)        CLDM_ASSIGN_ARG(__VA_ARGS__)
#endif

#include "cldmgen.h"

#endif /* CLDM_H */
