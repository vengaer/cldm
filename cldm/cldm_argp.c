#include "cldm_argp.h"
#include "cldm_byteseq.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_nfa.h"
#include "cldm_rtassert.h"
#include "cldm_thread.h"

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

enum { CLDM_ARGP_SWITCH_MAX_SIZE = 64 };

enum { CLDM_ARGP_LONG_SWITCH_INDENT = 13 };
enum { CLDM_ARGP_DOC_INDENT = 48 };
enum { CLDM_ARGP_BUFSIZE = 256 };

enum cldm_argp_paramtype {
    cldm_argp_positional,
    cldm_argp_switch_short,
    cldm_argp_switch_long
};

struct cldm_argp_ctx;

typedef bool(*cldm_argp_handle)(struct cldm_argp_ctx *restrict , char const *restrict );

struct cldm_argp_ctx {
    cldm_argp_handle assign_pending;
    struct cldm_args *args;
    int index;
    bool treat_as_posparam;
    bool pending_arg;
};

struct cldm_argp_param {
    char p_short;
    char const *p_long;
    char const *p_argument;
    cldm_argp_handle handle;
};

static bool cldm_argp_set_verbose(struct cldm_argp_ctx *restrict, char const *restrict);
static bool cldm_argp_set_version(struct cldm_argp_ctx *restrict, char const *restrict);
static bool cldm_argp_set_help(struct cldm_argp_ctx *restrict, char const *restrict);
static bool cldm_argp_set_fail_fast(struct cldm_argp_ctx *restrict, char const *restrict);
static bool cldm_argp_set_capture(struct cldm_argp_ctx *restrict, char const *restrict);
static bool cldm_argp_set_capture_none(struct cldm_argp_ctx *restrict, char const *restrict);
static bool cldm_argp_set_redirect(struct cldm_argp_ctx *restrict, char const *restrict);
static bool cldm_argp_set_posidx(struct cldm_argp_ctx *restrict, char const *restrict);
static bool cldm_argp_set_jobs(struct cldm_argp_ctx *restrict, char const *restrict);

static struct cldm_argp_param cldm_argp_params[] = {
    { 'v', "--verbose",           0,        cldm_argp_set_verbose      },
    { 'V', "--version",           0,        cldm_argp_set_version      },
    { 'h', "--help",              0,        cldm_argp_set_help         },
    {  0,  "--usage",             0,        cldm_argp_set_help         },
    { 'x', "--fail-fast",         0,        cldm_argp_set_fail_fast    },
    { 'c', "--capture",           "STREAM", cldm_argp_set_capture      },
    { 's', "--capture-none",      0,        cldm_argp_set_capture_none },
    { 'd', "--redirect-captures", "FILE",   cldm_argp_set_redirect     },
    { 'j', "--jobs",              "JOBS",   cldm_argp_set_jobs         },
    {  0,  "--",                  0,        cldm_argp_set_posidx       }
};

static char const *cldm_argp_params_doc[] = {
    "Enable verbose output",
    "Print version and exit",
    "Print this help message and exit",
    "Same as --help",
    "Exit as soon as a test fails",
    "Capture output from STREAM and print it once all tests have finished. Valid options are 'none', 'stdout', 'stderr' and 'all'",
    "Same as --capture=none",
    "Redirect captures to FILE instead of printing to console. May be combined with --capture=STREAM",
    "Run tests concurrently using JOBS threads, max number is " cldm_str_expand(CLDM_MAX_THREADS)
};

static char const *cldm_argp_positional_doc = "[IDENTIFIER]... is a list of test identifiers and may contain both file and test names, in any order.\n"
                                              "If provided, cldm will run only tests identified by members of the list. File parameters are expected\n"
                                              "to contain only basenames, full paths are not supported.";

static char const *cldm_argp_bug_address = "vilhelm.engstrom@tuta.io";

static char const *cldm_argp_valid_captures[] = {
    "none",
    "stdout",
    "stderr",
    "all"
};

static bool cldm_argp_set_verbose(struct cldm_argp_ctx *restrict ctx, char const *restrict optarg) {
    ctx->args->verbose = true;
    return optarg;
}

static bool cldm_argp_set_version(struct cldm_argp_ctx *restrict ctx, char const *restrict optarg) {
    ctx->args->version = true;
    return optarg;
}

static bool cldm_argp_set_help(struct cldm_argp_ctx *restrict ctx, char const *restrict optarg) {
    ctx->args->help = true;
    return optarg;
}

static bool cldm_argp_set_fail_fast(struct cldm_argp_ctx *restrict ctx, char const *restrict optarg) {
    ctx->args->fail_fast = true;
    return optarg;
}

static bool cldm_argp_set_capture(struct cldm_argp_ctx *restrict ctx, char const *restrict optarg) {
    bool success;
    char const **iter;

    if(!*optarg) {
        ctx->pending_arg = true;
        ctx->assign_pending = cldm_argp_set_capture;
        return true;
    }

    success = false;
    cldm_for_each(iter, cldm_argp_valid_captures) {
        if(strcmp(*iter, optarg) == 0) {
            ctx->args->capture = iter - cldm_argp_valid_captures;
            success = true;
            break;
        }
    }

    if(!success) {
        cldm_err("Invalid argument '%s' for --capture", optarg);
    }

    return success;
}

static bool cldm_argp_set_capture_none(struct cldm_argp_ctx *restrict ctx, char const *restrict optarg) {
    ctx->args->capture = cldm_capture_none;
    return optarg;
}

static bool cldm_argp_set_redirect(struct cldm_argp_ctx *restrict ctx, char const *restrict optarg) {
    if(!*optarg) {
        ctx->pending_arg = true;
        ctx->assign_pending = cldm_argp_set_redirect;
        return true;
    }

    ctx->args->redirect = optarg;
    return true;
}

static bool cldm_argp_set_jobs(struct cldm_argp_ctx *restrict ctx, char const *restrict optarg) {
    char *end;
    unsigned long n;

    if(!*optarg) {
        ctx->pending_arg = true;
        ctx->assign_pending = cldm_argp_set_jobs;
        return true;
    }

    n = strtoul(optarg, &end, 0);

    if(*end) {
        cldm_err("Invalid argument '%s' for --jobs, expected number", optarg);
        return false;
    }

    if(!n) {
        cldm_err("Invalid argument '%lu' for --jobs, need a non-zero number", n);
        return false;
    }

    if(n > CLDM_MAX_THREADS) {
        cldm_warn("Number of jobs exceeds maximum %d and will be clamped", CLDM_MAX_THREADS);
        ctx->args->jobs = CLDM_MAX_THREADS;
        return true;
    }

    ctx->args->jobs = n;
    return true;
}

static bool cldm_argp_set_posidx(struct cldm_argp_ctx *restrict ctx, char const *restrict optarg) {
    ctx->treat_as_posparam = true;
    return optarg;
}


static bool cldm_argp_partition(struct cldm_argp_ctx *restrict ctx, unsigned char const *restrict posflags, int argc, char **restrict argv) {
    unsigned lidx;
    unsigned hidx;
    char **tmp;

    tmp = malloc(2 * argc * sizeof(*tmp));
    if(!tmp) {
        cldm_err("Could not malloc %zu bytes for partitioning", 2 * argc * sizeof(*tmp));
        return false;
    }

    lidx = 0;
    hidx = argc;

    for(int i = 0; i < argc - 1; i++) {
        if(posflags[i / CHAR_BIT] & (1 << (i % CHAR_BIT))) {
            tmp[hidx++] = argv[i + 1];
        }
        else {
            tmp[lidx++] = argv[i + 1];
        }
    }

    memcpy(&argv[1], tmp, lidx * sizeof(*tmp));
    memcpy(&argv[lidx + 1], &tmp[argc], (hidx - argc) * sizeof(*tmp));
    ctx->args->posparams = &argv[lidx + 1];
    ctx->args->nposparams = argc - lidx - 1;

    free(tmp);
    return true;
}

static inline enum cldm_argp_paramtype cldm_argp_paramtype(char const *arg) {
    return (arg[0] == '-') + (arg[0] == '-' && arg[1] == '-');
}

static bool cldm_argp_parse_short_switch(struct cldm_argp_ctx *restrict ctx, char const *restrict sw) {
    struct cldm_argp_param *param;
    for(++sw; *sw; ++sw) {
        cldm_for_each(param, cldm_argp_params) {
            if(param->p_short == *sw) {
                if(!param->handle(ctx, sw + 1)) {
                    return false;
                }

                if(param->p_argument) {
                    return true;
                }
                break;
            }
        }
        if(!param) {
            cldm_err("Invalid option -- '%c'", *sw);
            return false;
        }
    }
    return true;
}

static bool cldm_argp_parse_long_switch(struct cldm_argp_ctx *restrict ctx, struct cldm_nfa const *restrict nfa, char const *restrict sw) {
    struct cldm_argp_param *param;
    int len;

    len = cldm_nfa_simulate(nfa, sw);
    if(len == -1) {
        cldm_err("Unrecognized option: '%s'", sw);
        return false;
    }

    cldm_for_each(param, cldm_argp_params) {
        if(strncmp(param->p_long, sw, len + !sw[len]) == 0) {
            if(param->p_argument) {
                if(sw[len] != '=' || (sw[len] && !sw[len + 1])) {
                    cldm_err("Option '%s' requires an argument", param->p_long);
                    return false;
                }
                ++len;
            }
            if(!param->handle(ctx, &sw[len])) {
                return false;
            }
            break;
        }
    }
    cldm_rtassert(param);
    return true;
}

bool cldm_argp_parse(struct cldm_args *restrict args, int argc, char **restrict argv) {
    unsigned char posflags[CLDM_ARGP_MAX_PARAMS / CHAR_BIT] = { 0 };
    struct cldm_argp_param *param;
    struct cldm_argp_ctx ctx;
    struct cldm_nfa nfa;
    bool success;

    success = false;

    *args = (struct cldm_args) {
        .posparams = &argv[argc],
        .nposparams = 0u,
        .jobs = 1u,
        .capture = cldm_capture_all
    };

    if(argc > CLDM_ARGP_MAX_PARAMS) {
        cldm_err("At most %d command line parameters are supported", CLDM_ARGP_MAX_PARAMS);
        return false;
    }

    ctx = (struct cldm_argp_ctx) {
        .args = args,
        .index = 1,
        .treat_as_posparam = false
    };

    if(!cldm_nfa_init(&nfa)) {
        return false;
    }

    cldm_for_each(param, cldm_argp_params) {
        if(param->p_long && !cldm_nfa_add_argument(&nfa, param->p_long)) {
            cldm_err("Error while adding parameter %s to nfa", param->p_long);
            goto epilogue;
        }
    }

    for(int i = 1; i < argc; i++) {
        if(ctx.pending_arg) {
            if(!ctx.assign_pending(&ctx, argv[i])) {
                goto epilogue;
            }
            ctx.pending_arg = false;
            continue;
        }
        switch(!ctx.treat_as_posparam * cldm_argp_paramtype(argv[i])) {
            case cldm_argp_positional:
                posflags[(i - 1) / CHAR_BIT] |= (1 << ((i - 1) % CHAR_BIT));
                break;
            case cldm_argp_switch_short:
                if(!cldm_argp_parse_short_switch(&ctx, argv[i])) {
                    goto epilogue;
                }
                break;
            case cldm_argp_switch_long:
                if(!cldm_argp_parse_long_switch(&ctx, &nfa, argv[i])) {
                    goto epilogue;
                }
                break;
            default:
                cldm_rtassert(0, "Invalid parameter type");
        }
    }

    if(ctx.pending_arg) {
        cldm_err("Option '%s' requires an argument", argv[argc - 1]);
        goto epilogue;
    }

    if(!cldm_argp_partition(&ctx, posflags, argc, argv)) {
        goto epilogue;
    }

    success = true;
epilogue:
    cldm_nfa_free(&nfa);

    if(!success) {
        cldm_argp_usage(argv[0]);
    }
    return success;
}

void cldm_argp_usage(char const *argv0) {
    char buffer[CLDM_ARGP_BUFSIZE];
    char tmp[CLDM_ARGP_BUFSIZE];
    struct cldm_argp_param *param;
    char const **doc;
    size_t tmppos;
    size_t bufpos;

    cldm_static_assert(cldm_arrsize(cldm_argp_params) - 1 == cldm_arrsize(cldm_argp_params_doc), "Size mismatch between parameters and doc strings");

    cldm_log("cldm -- unit test and mocking framework\n");
    cldm_log("Usage:\n %s [OPTION]... [IDENTIFIER]...\n", cldm_basename(argv0));
    cldm_log("Options:");

    cldm_for_each_zip(doc, param, cldm_argp_params_doc, cldm_argp_params) {
        tmp[0] = '\0';
        if(param->p_short) {
            tmppos = (size_t)snprintf(tmp, sizeof(tmp), "  -%c", param->p_short);
            cldm_rtassert(tmppos < sizeof(tmp), "Overflow while formatting usage");
            if(param->p_argument) {
                tmppos += (size_t)snprintf(tmp + tmppos, sizeof(tmp) - tmppos, " %s", param->p_argument);
                cldm_rtassert(tmppos < sizeof(tmp), "Overflow while formatting usage");
            }

            cldm_rtassert(tmppos + 1 < sizeof(tmp), "Overflow while formatting usage");
            tmp[tmppos++] = ',';
            tmp[tmppos] = '\0';
        }

        bufpos = (size_t)snprintf(buffer, sizeof(buffer), "%-*s", CLDM_ARGP_LONG_SWITCH_INDENT, tmp);
        cldm_rtassert(bufpos < sizeof(buffer), "Overflow while formatting usage");

        if(param->p_long) {
            tmppos = (size_t)snprintf(tmp, sizeof(tmp), "%s", param->p_long);
            cldm_rtassert(tmppos < sizeof(tmp), "Overflow while formatting usage");
            if(param->p_argument) {
                tmppos += (size_t)snprintf(tmp + tmppos, sizeof(tmp) - tmppos, "=%s", param->p_argument);
                cldm_rtassert(tmppos < sizeof(tmp), "Overflow while formatting usage");
            }
        }
        bufpos = (size_t)snprintf(buffer + bufpos, sizeof(buffer) - bufpos, "%s", tmp);

        cldm_log("%-*s%s", CLDM_ARGP_DOC_INDENT, buffer, *doc);
    }

    cldm_log("\n%s", cldm_argp_positional_doc);

}
void cldm_argp_version(void) {
    cldm_log("cldm -- version " cldm_str_expand(CLDM_VERSION));
    cldm_log("Report bugs to %s", cldm_argp_bug_address);
}
