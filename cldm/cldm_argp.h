#ifndef CLDM_ARGP_H
#define CLDM_ARGP_H

#include "cldm_io.h"

#include <stdbool.h>
#include <stddef.h>

struct cldm_args {
    size_t posidx;
    enum cldm_capture capture;
    bool fail_fast;
    bool help;
    bool version;
    bool verbose;
};

void cldm_argp_usage(char const *argv0);
void cldm_argp_version(void);
bool cldm_argp_parse(struct cldm_args *restrict args, int argc, char **restrict argv);

#endif /* CLDM_ARGP_H */
