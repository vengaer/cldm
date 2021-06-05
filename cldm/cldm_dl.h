#ifndef CLDM_DL_H
#define CLDM_DL_H

#include "cldm_elf.h"
#include "cldm_macro.h"

#include <stddef.h>
#include <stdio.h>

#include <dlfcn.h>
#include <sys/mman.h>

struct stat;

/* Handles to functions in libc.so used to avoid clashes with mocks during initialization */
extern int   (*cldm_explicit_open)(char const *, int);
extern int   (*cldm_explicit_close)(int);
extern int   (*cldm_explicit_fstat)(int, struct stat *);
extern void *(*cldm_explicit_mmap)(void *, size_t, int, int, int, off_t);
extern int   (*cldm_explicit_munmap)(void *, size_t);
extern int   (*cldm_explicit_strcmp)(char const *, char const *);
extern int   (*cldm_explicit_strncmp)(char const *, char const *, size_t);
extern char *(*cldm_explicit_strerror)(int);
extern int   (*cldm_explicit_fprintf)(FILE *restrict, char const *restrict, ...);

/* Must be called in a sequential context */
int cldm_dlmap_explicit(void);
int cldm_dlmap_needed(struct cldm_elfmap const *map);
void cldm_dlfree(void);

/* Thread safe */

/* Lookup symbol in first shared library apart from
 * libcldm that defines it. A lookup sequence must follow the
 * pattern
 * *(void **)symhandle = cldm_dlsym_next(name);
 * // Invoke symhandle at least once
 * cldm_dlresolve(name);    // Mark lookup successful
 */
void *cldm_dlsym_next(char const *symname);
void cldm_dlresolve(char const *symname);

#endif /* CLDM_DL_H */
