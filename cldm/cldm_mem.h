#ifndef CLDM_MEM_H
#define CLDM_MEM_H

#include "cldm_macro.h"

#include <stddef.h>

void *cldm_mmove(void *dst, void const* restrict src, size_t size);
void *cldm_mcpy(void *restrict dst, void const* restrict src, size_t size);


#endif /* CLDM_MEM_H */
