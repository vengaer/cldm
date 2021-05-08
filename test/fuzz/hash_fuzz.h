#ifndef HASH_FUZZ_H
#define HASH_FUZZ_H

#include <stdint.h>
#include <stddef.h>

int hash_fuzz(uint8_t const *data, size_t size);

#endif /* HASH_FUZZ_H */
