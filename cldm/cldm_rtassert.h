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

/* No message */
#define cldm_rtassert1(condition)               \
    cldm_rtassert_trigger(condition, "%s:%s assertion '%s' failed\n", __FILE__, cldm_str_expand(__LINE__), #condition)

/* Single string parameter */
#define cldm_rtassert01(condition, msg)         \
    cldm_rtassert_trigger(condition, "%s:%s assertion '%s' failed: " msg "\n", __FILE__, cldm_str_expand(__LINE__), #condition)

/* Format string with parameters */
#define cldm_rtassert00(condition, fmt, ...)    \
    cldm_rtassert_trigger(condition, "%s:%s assertion '%s' failed: " fmt "\n", __FILE__, cldm_str_expand(__LINE__), #condition, __VA_ARGS__)

/* At least a single string parameter */
#define cldm_rtassert0(condition, ...)  \
    cldm_cat_expand(cldm_rtassert0,cldm_token_1(cldm_count(__VA_ARGS__)))(condition, __VA_ARGS__)

#define cldm_rtassert(...)    \
    cldm_cat_expand(cldm_rtassert,cldm_token_1(cldm_count(__VA_ARGS__)))(__VA_ARGS__)

#endif /* CLDM_RTASSERT_H */
