#ifndef CLDM_MEM_H
#define CLDM_MEM_H

#include "cldm_macro.h"

#include <stddef.h>

enum { CLDM_PAGE_SIZE = 4096 };

void *cldm_mmove(void *dst, void const* restrict src, size_t size);
void *cldm_mcpy(void *restrict dst, void const* restrict src, size_t size);
void *cldm_mset(void *dst, int c, size_t size);


#endif /* CLDM_MEM_H */
