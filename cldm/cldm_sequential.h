#ifndef CLDM_SEQUENTIAL_H
#define CLDM_SEQUENTIAL_H

#include "cldm_argp.h"
#include "cldm_elf.h"

int cldm_sequential_run(struct cldm_elfmap const *restrict map, struct cldm_args const *restrict args);

#endif /* CLDM_SEQUENTIAL_H */
