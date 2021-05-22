#ifndef AVX2_SCAN_LT_FUZZ_H
#define AVX2_SCAN_LT_FUZZ_H

#include <stddef.h>
#include <stdint.h>

int avx2_scan_lt_fuzz(uint8_t const *data, size_t size);

#endif /* AVX2_SCAN_LT_FUZZ_H */
