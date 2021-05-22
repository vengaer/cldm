#ifndef AVX2_MEMCMP_FUZZ_H
#define AVX2_MEMCMP_FUZZ_H

#include <stddef.h>
#include <stdint.h>

int avx2_memcmp_fuzz(uint8_t const *data, size_t size);

#endif /* AVX2_MEMCMP_FUZZ_H */
