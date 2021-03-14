#ifndef CLDM_DL_H
#define CLDM_DL_H

#include "cldm_assert.h"

#include <sys/types.h>

int cldm_unload(void);
int cldm_preload(void);

void *cldm_dlsym_next(char const *symname);

#endif /* CLDM_DL_H */
