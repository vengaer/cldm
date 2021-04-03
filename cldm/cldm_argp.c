#include "cldm_argp.h"
#include "cldm_log.h"
#include "cldm_ntbs.h"

#include <string.h>

enum { CLDM_ARGP_OPTINDENT = 2 };
enum { CLDM_ARGP_ADJ = 40 };

static char cldm_argp_short[] = { 'h', 'V', 'v', 'x' };
static char const *cldm_argp_long[] = { "help", "version", "verbose", "fail-fast" };
static char const *cldm_argp_posarg = "[FILE]...";
static char const *cldm_argp_swdesc[] = {
    "Print this help message and exit",
    "Print version and exit",
    "Enable verbose output",
    "Exit after first failed test, if any"
};
static char const *cldm_argp_posdesc = "Run the tests specified in the space-separatred list FILE..., all files must have been compiled ínto the binary";

void cldm_argp_usage(char const *argv0) {
    char const *basename = cldm_basename(argv0);
    char *it0;
    char const **it1;

    cldm_log_raw("%s", basename);
    cldm_for_each_zip(it0, it1, cldm_argp_short, cldm_argp_long) {
        cldm_log_raw(" [-%c|--%s]", *it0, *it1);
    }

    cldm_log(" %s\n", cldm_argp_posarg);

    for(unsigned i = 0; i < cldm_arrsize(cldm_argp_short); i++) {
        cldm_log("%-*s-%c, --%-*s %s", CLDM_ARGP_OPTINDENT, "", cldm_argp_short[i], CLDM_ARGP_ADJ, cldm_argp_long[i], cldm_argp_swdesc[i]);
    }
    cldm_log("%-*s%-*s%s", CLDM_ARGP_OPTINDENT, "", CLDM_ARGP_ADJ + (int)sizeof("- , --"), cldm_argp_posarg, cldm_argp_posdesc);
}
