#ifndef CLDM_LOG_H
#define CLDM_LOG_H

#include <stdio.h>

#define cldm_err_internal(fmt, ...) \
    fprintf(stderr, fmt "%s\n", __VA_ARGS__)

#define cldm_err(...)   \
    cldm_err_internal(__VA_ARGS__, "")

#endif /* CLDM_LOG_H */
