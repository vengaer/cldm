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

extern int   (*cldm_dl_fprintf)(FILE *restrict, char const *restrict , ...);
extern FILE *(*cldm_dl_fopen)(char const *restrict, char const *restrict);
extern int   (*cldm_dl_fclose)(FILE *);
extern int   (*cldm_dl_fflush)(FILE *);
extern char *(*cldm_dl_fgets)(char *, int, FILE *);
extern int   (*cldm_dl_fileno)(FILE *);
extern int   (*cldm_dl_dup)(int);
extern int   (*cldm_dl_dup2)(int, int);
extern void *(*cldm_dl_realloc)(void *, size_t);
extern void  (*cldm_dl_free)(void *);
extern char *(*cldm_dl_strerror)(int);
extern int   (*cldm_dl_snprintf)(char *restrict , size_t, char const *restrict, ...);

void *cldm_dlsym_next(char const *symname);

int cldm_dlgentab(struct cldm_elfmap const *map);
int cldm_dlclose(void);

#endif /* CLDM_DL_H */
