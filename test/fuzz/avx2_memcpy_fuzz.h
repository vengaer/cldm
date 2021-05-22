#ifndef AVX2_MEMCPY_FUZZ_H
#define AVX2_MEMCPY_FUZZ_H

#include <stddef.h>
#include <stdint.h>

int avx2_memcpy_fuzz(uint8_t const *data, size_t size);

#endif /* AVX2_MEMCPY_FUZZ_H */
