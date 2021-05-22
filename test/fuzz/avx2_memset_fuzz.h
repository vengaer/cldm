#ifndef AVX2_MEMSET_FUZZ_H
#define AVX2_MEMSET_FUZZ_H

#include <stdint.h>
#include <stddef.h>

int avx2_memset_fuzz(uint8_t const *data, size_t size);

#endif /* AVX2_MEMSET_FUZZ_H */
