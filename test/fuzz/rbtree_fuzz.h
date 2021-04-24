#ifndef RBTREE_FUZZ_H
#define RBTREE_FUZZ_H

#include <stddef.h>
#include <stdint.h>

int rbtree_fuzz(uint8_t const *data, size_t size);

#endif /* RBTREE_FUZZ_H */
