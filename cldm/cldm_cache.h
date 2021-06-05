#ifndef CLDM_CACHE_H
#define CLDM_CACHE_H

#include "cldm_config.h"

#define cldm_cachealign(type)                                                                           \
    union  {                                                                                            \
        type data;                                                                                      \
        unsigned char align[(sizeof(type) + CLDM_L1_DCACHE_LINESIZE - 1) & -CLDM_L1_DCACHE_LINESIZE];   \
    }

#endif /* CLDM_CACHE_H */
