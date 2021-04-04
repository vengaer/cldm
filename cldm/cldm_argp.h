#ifndef CLDM_ARGP_H
#define CLDM_ARGP_H

#include <stdbool.h>
#include <stddef.h>

struct cldm_args {
    size_t posind;
    bool no_capture;
    bool fail_fast;
    bool help;
    bool version;
};

void cldm_argp_usage(char const *argv0);
void cldm_argp_version(void);
bool cldm_argp_parse(struct cldm_args *restrict args, int argc, char **restrict argv);

#endif /* CLDM_ARGP_H */
