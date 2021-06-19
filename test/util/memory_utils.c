#include "memory_utils.h"

size_t alignment(void const *addr);
size_t pgdistance(void const *addr);
char asciicvt(uint8_t byte);
void hexdump(char const *restrict bufname, uint8_t const *restrict data, size_t size, FILE *restrict fp);
