#include "argp_fuzz.h"
#include "avx2_memcmp_fuzz.h"
#include "avx2_memcpy_fuzz.h"
#include "avx2_memset_fuzz.h"
#include "avx2_memswp_fuzz.h"
#include "avx2_scan_lt_fuzz.h"
#include "avx2_strlen_fuzz.h"
#include "avx2_strscpy_fuzz.h"
#include "hash_fuzz.h"
#include "rbtree_fuzz.h"

#include <cldm/cldm_config.h>
#include <cldm/cldm_macro.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cldm_fuzzentry {
    char const *env;
    int(*entry)(uint8_t const *, size_t);
};

static void usage(void);

#ifndef CLDM_HAS_AVX2
static int avx2_err(uint8_t const *data, size_t size) {
    (void)data;
    (void)size;
    fputs("cldm compiled without AVX2 support\n", stderr);
    usage();
    abort();
}
#endif

static struct cldm_fuzzentry const entrypoints[] = {
#ifdef CLDM_HAS_AVX2
    { "avx2_memcmp",  avx2_memcmp_fuzz  },
    { "avx2_memcpy",  avx2_memcpy_fuzz  },
    { "avx2_memset",  avx2_memset_fuzz  },
    { "avx2_memswp",  avx2_memswp_fuzz  },
    { "avx2_scan_lt", avx2_scan_lt_fuzz },
    { "avx2_strlen",  avx2_strlen_fuzz  },
    { "avx2_strscpy", avx2_strscpy_fuzz },
#else
    { "avx2_memcmp",  avx2_err          },
    { "avx2_memcpy",  avx2_err          },
    { "avx2_memset",  avx2_err          },
    { "avx2_memswp",  avx2_err          },
    { "avx2_scan_lt", avx2_err         },
    { "avx2_strlen",  avx2_err          },
    { "avx2_strscpy", avx2_err         },
#endif
    { "rbtree",       rbtree_fuzz       },
    { "argp",         argp_fuzz         },
    { "hash",         hash_fuzz         }
};

static void usage(void) {
    struct cldm_fuzzentry const *iter;
    puts("cldm -- fuzzing\n");
    puts("The fuzzing target is read from the CLDM_FUZZTARGET environment variable\nValid options are:");
    cldm_for_each(iter, entrypoints) {
        printf("  %s\n", iter->env);
    }
    puts("");
}

int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size) {
    char *target;
    struct cldm_fuzzentry const *iter;
    int(*entry)(uint8_t const *, size_t);

    target = getenv("CLDM_FUZZTARGET");

    if(!target) {
        fputs("CLDM_FUZZTARGET not set\n", stderr);
        usage();
        abort();
    }

    entry = 0;

    cldm_for_each(iter, entrypoints) {
        if(strcmp(iter->env, target) == 0) {
            entry = iter->entry;
            break;
        }
    }

    if(!entry) {
        fprintf(stderr, "Invalid fuzz target %s\n", target);
        abort();
    }

    return entry(data, size);
}
