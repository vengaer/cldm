#ifndef CLDM_PARALLEL_H
#define CLDM_PARALLEL_H

#include "cldm_argp.h"
#include "cldm_elf.h"

int cldm_parallel_run(struct cldm_elfmap const *restrict map, struct cldm_args const *restrict args);

#endif /* CLDM_PARALLEL_H */
