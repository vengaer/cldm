#ifndef CLDM_DL_H
#define CLDM_DL_H

#include "cldm_elf.h"
#include "cldm_rtassert.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

void *cldm_dlsym_next(char const *symname);
int cldm_dlclose(void);

#endif /* CLDM_DL_H */
