#ifndef CLDM_MEM_H
#define CLDM_MEM_H

#include "cldm_macro.h"

#include <stddef.h>

#define cldm_alignof(type)  \
    (size_t)&(((struct cldm_cat_expand(cldm_align,__LINE__) { char b; type t; } *)0)->t)



#endif /* CLDM_MEM_H */
