#ifndef CLDM_LOG_H
#define CLDM_LOG_H

#include "cldm_dl.h"
#include "cldm_io.h"

#include <stdio.h>

#ifndef CLDM_SUPPRESS_LOGGING

#define cldm_print_guard_raw(stream, fallback, fmt, ...)    \
    cldm_explicit_fprintf(stream ? stream : fallback, fmt, __VA_ARGS__)

#define cldm_print_guard(stream, fallback, fmt, ...)    \
    cldm_print_guard_raw(stream, fallback, fmt "%.0d\n", __VA_ARGS__)

#define cldm_err_internal(fmt, ...)     \
    cldm_print_guard(cldm_stderr, stderr, "Error: " fmt, __VA_ARGS__)

#define cldm_warn_internal(fmt, ...)    \
    cldm_print_guard(cldm_stderr, stderr, "Warning: " fmt, __VA_ARGS__)

#define cldm_log_internal(fmt, ...)     \
    cldm_print_guard(cldm_stdout, stdout, fmt, __VA_ARGS__)

#define cldm_log_stream_internal(stream, fmt, ...)  \
    cldm_print_guard(stream, stdout, fmt, __VA_ARGS__)

#define cldm_err(...)   \
    cldm_err_internal(__VA_ARGS__, 0)

#define cldm_warn(...)  \
    cldm_warn_internal(__VA_ARGS__, 0)

#define cldm_log(...)   \
    cldm_log_internal(__VA_ARGS__, 0)

#define cldm_log_stream(stream, ...)    \
    cldm_log_stream_internal(stream, __VA_ARGS__, 0)

#define cldm_log_raw(...)   \
    cldm_print_guard_raw(cldm_stdout, stdout, __VA_ARGS__)

#else
static inline void cldm_swallow(void *p, ...) { (void)p; }

#define cldm_err(...) cldm_swallow(0, __VA_ARGS__)
#define cldm_warn(...) cldm_swallow(0, __VA_ARGS__)
#define cldm_log(...) cldm_swallow(0, __VA_ARGS__)
#define cldm_log_stream(...) cldm_swallow(0, __VA_ARGS__)
#define cldm_log_raw(...) cldm_swallow(0, __VA_ARGS__)
#endif

#endif /* CLDM_LOG_H */
