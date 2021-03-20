#ifndef CLDM_DLWRP_H
#define CLDM_DLWRP_H

#include "cldm_dl.h"
#include "cldm_macro.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

extern bool cldm_dl_mapped;

#define cldm_wrapfunc(func, ...)    \
    (cldm_dl_mapped ? cldm_cat_expand(cldm_dl_,func)(__VA_ARGS__) : func(__VA_ARGS__))

#undef fprintf
#undef fopen
#undef fclose
#undef fflush
#undef fgets
#undef fileno
#undef dup
#undef dup2
#undef realloc
#undef free
#undef strerror
#undef snprintf

/* Redefine standard functions used internally to call ones gotten from dlsym.
 * This ensures everything works even if any of these functions were to be
 * mocked out */
#define fprintf(...)    \
    cldm_wrapfunc(fprintf, __VA_ARGS__)
#define fopen(...)      \
    cldm_wrapfunc(fopen, __VA_ARGS__)
#define fclose(...)     \
    cldm_wrapfunc(fclose, __VA_ARGS__)
#define fflush(...)     \
    cldm_wrapfunc(fflush, __VA_ARGS__)
#define fgets(...)      \
    cldm_wrapfunc(fgets, __VA_ARGS__)
#define fileno(...)     \
    cldm_wrapfunc(fileno, __VA_ARGS__)
#define dup(...)        \
    cldm_wrapfunc(dup, __VA_ARGS__)
#define dup2(...)       \
    cldm_wrapfunc(dup2, __VA_ARGS__)
#define realloc(...)    \
    cldm_wrapfunc(realloc, __VA_ARGS__)
#define free(...)       \
    cldm_wrapfunc(free, __VA_ARGS__)
#define strerror(...)   \
    cldm_wrapfunc(strerror, __VA_ARGS__)
#define snprintf(...)   \
    cldm_wrapfunc(snprintf, __VA_ARGS__)


#endif /* CLDM_DLWRP_H */
