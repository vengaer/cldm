#ifndef CLDM_LOG_H
#define CLDM_LOG_H

#include "cldm_dl.h"
#include "cldm_dlwrp.h"
#include "cldm_io.h"

#include <stdio.h>

#define cldm_print_guard_raw(stream, fallback, fmt, ...)    \
    fprintf(stream ? stream : fallback, fmt, __VA_ARGS__)

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

#endif /* CLDM_LOG_H */
