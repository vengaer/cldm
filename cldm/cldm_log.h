#ifndef CLDM_LOG_H
#define CLDM_LOG_H

#include <stdio.h>

#define cldm_err_internal(fmt, ...)     \
    fprintf(stderr, "Error: " fmt "%.0d\n", __VA_ARGS__)

#define cldm_warn_internal(fmt, ...)    \
    fprintf(stderr, "Warning: " fmt "%.0d\n", __VA_ARGS__)

#define cldm_err(...)   \
    cldm_err_internal(__VA_ARGS__, 0)

#define cldm_warn(...)  \
    cldm_warn_internal(__VA_ARGS__, 0)

#endif /* CLDM_LOG_H */
