#ifndef CLDM_RUNNER_H
#define CLDM_RUNNER_H

#include "cldm_elf.h"

int cldm_collect_and_run(struct cldm_elfmap const *restrict map, bool fail_fast, char **restrict files, size_t nfiles);

#endif /* CLDM_RUNNER_H */
