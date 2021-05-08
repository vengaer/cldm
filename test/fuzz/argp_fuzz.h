#ifndef ARGP_FUZZ_H
#define ARGP_FUZZ_H

#include <stdint.h>
#include <stddef.h>

int argp_fuzz(uint8_t const *data, size_t size);

#endif /* ARGP_FUZZ_H */
