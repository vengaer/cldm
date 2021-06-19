#ifndef AVX2_STRCMP_FUZZ_H
#define AVX2_STRCMP_FUZZ_H

#include <stddef.h>
#include <stdint.h>

int avx2_strcmp_fuzz(uint8_t const *data, size_t size);

#endif /* AVX2_STRCMP_FUZZ_H */
