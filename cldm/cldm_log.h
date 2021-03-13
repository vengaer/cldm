#ifndef CLDM_LOG_H
#define CLDM_LOG_H

#include "cldm_io.h"

#include <stdio.h>

#define cldm_err_internal(fmt, ...)     \
    fprintf(cldm_stderr ? cldm_stderr : stderr, "Error: " fmt "%.0d\n", __VA_ARGS__)

#define cldm_warn_internal(fmt, ...)    \
    fprintf(cldm_stderr ? cldm_stderr : stderr, "Warning: " fmt "%.0d\n", __VA_ARGS__)

#define cldm_log_internal(fmt, ...)     \
    fprintf(cldm_stdout ? cldm_stdout : stdout, fmt "%.0d\n", __VA_ARGS__)

#define cldm_log_stream_internal(stream, fmt, ...)  \
    fprintf(stream, fmt "%.0d\n", __VA_ARGS__)

#define cldm_err(...)   \
    cldm_err_internal(__VA_ARGS__, 0)

#define cldm_warn(...)  \
    cldm_warn_internal(__VA_ARGS__, 0)

#define cldm_log(...)   \
    cldm_log_internal(__VA_ARGS__, 0)

#define cldm_log_stream(stream, ...)    \
    cldm_log_stream_internal(stream, __VA_ARGS__, 0)

#endif /* CLDM_LOG_H */
