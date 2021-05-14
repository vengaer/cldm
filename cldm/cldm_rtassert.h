#ifndef CLDM_RTASSERT_H
#define CLDM_RTASSERT_H

#include "cldm_macro.h"
#include "cldm_log.h"

#include <stdio.h>
#include <stdlib.h>

#define cldm_static_assert2(condition, reason)      \
    ((void)sizeof(unsigned char[!!(condition) + !(condition) * -1]))

#define cldm_static_assert1(condition)              \
    cldm_static_assert2(condition, "")

#define cldm_static_assert(...)                     \
    cldm_overload(cldm_static_assert,__VA_ARGS__)

#define cldm_rtassert_trigger(condition, ...)       \
    do {                                            \
        if(!(condition)) {                          \
            cldm_log_stream(stderr, __VA_ARGS__);   \
            exit(2);                                \
        }                                           \
    } while (0)

#define cldm_rtassert_no_msg(condition)   \
    cldm_rtassert_trigger(condition, "%s:%s assertion '%s' failed\n", __FILE__, cldm_str_expand(__LINE__), #condition)

#define cldm_rtassert_msg(condition, msg) \
    cldm_rtassert_trigger(condition, "%s:%s assertion '%s' failed: " msg "\n", __FILE__, cldm_str_expand(__LINE__), #condition)

#define cldm_rtassert_variadic(condition, fmt, ...)   \
    cldm_rtassert_trigger(condition, "%s:%s assertion '%s' failed: " fmt "\n", __FILE__, cldm_str_expand(__LINE__), #condition, __VA_ARGS__)

#define cldm_rtassert(...)    \
    cldm_overload(cldm_rtassert, __VA_ARGS__)


#define cldm_rtassert1(...)   cldm_rtassert_no_msg(__VA_ARGS__)
#define cldm_rtassert2(...)   cldm_rtassert_msg(__VA_ARGS__)
#define cldm_rtassert3(...)   cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert4(...)   cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert5(...)   cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert6(...)   cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert7(...)   cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert8(...)   cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert9(...)   cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert10(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert11(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert12(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert13(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert14(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert15(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert16(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert17(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert18(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert19(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert20(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert21(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert22(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert23(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert24(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert25(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert26(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert27(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert28(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert29(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert30(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert31(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert32(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert33(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert34(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert35(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert36(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert37(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert38(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert39(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert40(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert41(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert42(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert43(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert44(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert45(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert46(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert47(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert48(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert49(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert50(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert51(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert52(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert53(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert54(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert55(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert56(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert57(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert58(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert59(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert60(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert61(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert62(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert63(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert64(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert65(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert66(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert67(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert68(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert69(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert70(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert71(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert72(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert73(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert74(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert75(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert76(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert77(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert78(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert79(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert80(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert81(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert82(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert83(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert84(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert85(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert86(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert87(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert88(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert89(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert90(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert91(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert92(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert93(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert94(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert95(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert96(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert97(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert98(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert99(...)  cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert100(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert101(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert102(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert103(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert104(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert105(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert106(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert107(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert108(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert109(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert110(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert111(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert112(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert113(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert114(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert115(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert116(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert117(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert118(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert119(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert120(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert121(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert122(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert123(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert124(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert125(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert126(...) cldm_rtassert_variadic(__VA_ARGS__)
#define cldm_rtassert127(...) cldm_rtassert_variadic(__VA_ARGS__)


#endif /* CLDM_RTASSERT_H */
