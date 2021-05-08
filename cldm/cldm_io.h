#ifndef CLDM_IO_H
#define CLDM_IO_H

#include "cldm_macro.h"

#include <stdio.h>
#include <unistd.h>

enum cldm_capture {
    cldm_capture_none,
    cldm_capture_stdout,
    cldm_capture_stderr,
    cldm_capture_all
};

extern FILE *cldm_stdout;
extern FILE *cldm_stderr;

int cldm_io_redirect2(FILE *restrict fp, char const *restrict path);
int cldm_io_redirect3(FILE *restrict fp, char const *restrict path, FILE *restrict replacement);
int cldm_io_restore(FILE *fp, int fd);

int cldm_io_capture_stdout(void);
int cldm_io_capture_stderr(void);

int cldm_io_dump_captured_stdout(void);
int cldm_io_dump_captured_stderr(void);

int cldm_io_restore_stdout(void);
int cldm_io_restore_stderr(void);

int cldm_io_remove_captured_stdout(void);
int cldm_io_remove_captured_stderr(void);

#define cldm_io_redirect(...)   \
    cldm_overload(cldm_io_redirect, __VA_ARGS__)

#endif /* CLDM_IO_H */
