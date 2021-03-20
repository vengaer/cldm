#ifndef CLDM_ELFDUMP_H
#define CLDM_ELFDUMP_H

#include "cldm_elf.h"

int cldm_elfdump_strtab(struct cldm_elfmap const *restrict map, char const *restrict section);
int cldm_elfdump_needed(struct cldm_elfmap const *map);
void cldm_elfdump_sections(struct cldm_elfmap const *map);

#endif /* CLDM_ELFDUMP_H */
