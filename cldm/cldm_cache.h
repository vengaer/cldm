#ifndef CLDM_CACHE_H
#define CLDM_CACHE_H

/* Consistent at least since Intel Core 2/Nehalem and AMD K8/K10 */
enum { CLDM_L1_CACHE_LINE_SIZE = 64 };

#define cldm_cachealign(type)                                                                           \
    union  {                                                                                            \
        type data;                                                                                      \
        unsigned char align[(sizeof(type) + CLDM_L1_CACHE_LINE_SIZE - 1) & -CLDM_L1_CACHE_LINE_SIZE];   \
    }

#endif /* CLDM_CACHE_H */
