#ifndef CLDM_MEM_H
#define CLDM_MEM_H

#include "cldm_macro.h"

#include <stddef.h>

#define cldm_alignof(type)  \
    (size_t)&(((struct cldm_cat_expand(cldm_align,__LINE__) { char b; type t; } *)0)->t)

void *cldm_mmove(void *dst, void const* restrict src, size_t size);
void *cldm_mcpy(void *restrict dst, void const* restrict src, size_t size);


#endif /* CLDM_MEM_H */
