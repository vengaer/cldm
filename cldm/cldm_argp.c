#include "cldm_algo.h"
#include "cldm_argp.h"
#include "cldm_log.h"
#include "cldm_ntbs.h"
#include "cldm_macro.h"
#include "cldm_rtassert.h"

#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

enum { CLDM_ARGP_OPTINDENT = 2 };
enum { CLDM_ARGP_ADJ = 40 };

static char cldm_argp_short[] = { 'h', 'V', 'x', 's' };
static char const *cldm_argp_long[] = { "help", "version", "fail-fast", "no-capture" };
static char const *cldm_argp_posarg = "[FILE]...";
static char const *cldm_argp_swdesc[] = {
    "Print this help message and exit",
    "Print version and exit",
    "Exit after first failed test, if any"
};
static char const *cldm_argp_posdesc = "Run the tests specified in the space-separated list FILE..., all files must have been compiled ínto the binary";
static struct cldm_nfa_state2 **cldm_argp_states;

enum cldm_nfa_action {
    cldm_nfa_reject,
    cldm_nfa_accept,
    cldm_nfa_pending
};

enum cldm_nfa_metachar {
    cldm_nfa_epsilon = 128,
    cldm_nfa_end,
    cldm_nfa_any
};

struct cldm_nfa_edge {
    unsigned char c;
    enum cldm_nfa_action *end;
};

enum cldm_nfa_argtype {
    cldm_argtype_none,
    cldm_argtype_short,
    cldm_argtype_long,
    cldm_argtype_posparam,
    cldm_argtype_divider
};

enum {
    CLDM_SHOPT_NONE = 0,
};

/* Emulate flexible array member in static context */
#define cldm_nfa_genstate(size)                     \
    struct cldm_cat_expand(cldm_nfa_state, size) {  \
        enum cldm_nfa_action action;                \
        enum cldm_nfa_argtype argtype;              \
        unsigned char shopt;                        \
        struct cldm_nfa_edge edges[size / 2];       \
    }

/* Define structs */
cldm_nfa_genstate(2);
cldm_nfa_genstate(4);
cldm_nfa_genstate(6);
cldm_nfa_genstate(8);
cldm_nfa_genstate(10);
cldm_nfa_genstate(12);
cldm_nfa_genstate(14);
cldm_nfa_genstate(16);

#define cldm_nfa_state(size)    \
    static struct cldm_cat_expand(cldm_nfa_state, size)

#define cldm_nfa_transition(ch, state)  \
    { .c = (ch), .end = &(cldm_cat_expand(cldm_nfa_state_,state)).action }

#define cldm_nfa_defstate(name, act, atype, opt, ...)                                   \
    cldm_nfa_state(cldm_count(__VA_ARGS__,,)) cldm_cat_expand(cldm_nfa_state_,name) = { \
        .action = act,                                                                  \
        .argtype = atype,                                                               \
        .shopt = opt,                                                                   \
        .edges = {                                                                      \
            __VA_ARGS__,                                                                \
            cldm_nfa_transition(cldm_nfa_any, reject)                                   \
        }                                                                               \
    }

cldm_nfa_state(14) cldm_nfa_state_sh_help;
cldm_nfa_state(14) cldm_nfa_state_sh_version;
cldm_nfa_state(14) cldm_nfa_state_sh_fail_fast;
cldm_nfa_state(14) cldm_nfa_state_sh_no_capture;

cldm_nfa_defstate(reject,        cldm_nfa_reject, cldm_argtype_none, CLDM_SHOPT_NONE, cldm_nfa_transition(cldm_nfa_any, reject));
cldm_nfa_defstate(accept,        cldm_nfa_accept, cldm_argtype_none, CLDM_SHOPT_NONE, cldm_nfa_transition(cldm_nfa_any, accept));

/* state -s */
cldm_nfa_defstate(sh_no_capture, cldm_nfa_accept,  cldm_argtype_short, 's',
    cldm_nfa_transition('s',          sh_no_capture),
    cldm_nfa_transition('V',          sh_version),
    cldm_nfa_transition('x',          sh_fail_fast),
    cldm_nfa_transition('h',          sh_help),
    cldm_nfa_transition(cldm_nfa_end, accept),
    cldm_nfa_transition(cldm_nfa_any, reject)
);

/* state -h */
cldm_nfa_defstate(sh_help,       cldm_nfa_accept,  cldm_argtype_short, 'h',
    cldm_nfa_transition('s',          sh_no_capture),
    cldm_nfa_transition('V',          sh_version),
    cldm_nfa_transition('x',          sh_fail_fast),
    cldm_nfa_transition('h',          sh_help),
    cldm_nfa_transition(cldm_nfa_end, accept),
    cldm_nfa_transition(cldm_nfa_any, reject)
);

/* state -V */
cldm_nfa_defstate(sh_version,    cldm_nfa_accept,  cldm_argtype_short, 'V',
    cldm_nfa_transition('s',          sh_no_capture),
    cldm_nfa_transition('V',          sh_version),
    cldm_nfa_transition('x',          sh_fail_fast),
    cldm_nfa_transition('h',          sh_help),
    cldm_nfa_transition(cldm_nfa_end, accept),
    cldm_nfa_transition(cldm_nfa_any, reject)
);

/* state -x */
cldm_nfa_defstate(sh_fail_fast,  cldm_nfa_accept,  cldm_argtype_short, 'x',
    cldm_nfa_transition('s',          sh_no_capture),
    cldm_nfa_transition('V',          sh_version),
    cldm_nfa_transition('x',          sh_fail_fast),
    cldm_nfa_transition('h',          sh_help),
    cldm_nfa_transition(cldm_nfa_end, accept),
    cldm_nfa_transition(cldm_nfa_any, reject)
);

/* Positional parameter */
cldm_nfa_defstate(posparam,      cldm_nfa_pending, cldm_argtype_posparam, CLDM_SHOPT_NONE,
    cldm_nfa_transition(cldm_nfa_end, accept),
    cldm_nfa_transition(cldm_nfa_any, posparam)
);

/* state --help */
cldm_nfa_defstate(lo_help,       cldm_nfa_pending, cldm_argtype_long, 'h',
    cldm_nfa_transition(cldm_nfa_end, accept)
);

/* state --hel */
cldm_nfa_defstate(lo_hel,        cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('p',          lo_help)
);

/* state --he */
cldm_nfa_defstate(lo_he,         cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('l',          lo_hel)
);

/* state --h */
cldm_nfa_defstate(lo_h,          cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('e',          lo_he)
);

/* state --fail-fast */
cldm_nfa_defstate(lo_fail_fast, cldm_nfa_pending, cldm_argtype_long, 'x',
    cldm_nfa_transition(cldm_nfa_end, accept)
);

/* state --fail-fas */
cldm_nfa_defstate(lo_fail_fas,   cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('t',          lo_fail_fast)
);

/* state --fail-fa */
cldm_nfa_defstate(lo_fail_fa,    cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('s',          lo_fail_fas)
);

/* state --fail-f */
cldm_nfa_defstate(lo_fail_f,     cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('a',          lo_fail_fa)
);

/* state --fail- */
cldm_nfa_defstate(lo_fail_,      cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('f',          lo_fail_f)
);

/* state --fail */
cldm_nfa_defstate(lo_fail,       cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('-',          lo_fail_)
);

/* state --fai */
cldm_nfa_defstate(lo_fai,        cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('l',          lo_fail)
);

/* state --fa */
cldm_nfa_defstate(lo_fa,         cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('i',          lo_fai)
);

/* state --f */
cldm_nfa_defstate(lo_f,          cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('a',          lo_fa)
);

/* state --version */
cldm_nfa_defstate(lo_version,    cldm_nfa_pending, cldm_argtype_long, 'V',
    cldm_nfa_transition(cldm_nfa_end, accept)
);

/* state --versio */
cldm_nfa_defstate(lo_versio,     cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('n',          lo_version)
);

/* state --versi */
cldm_nfa_defstate(lo_versi,      cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('o',          lo_versio)
);

/* state --vers */
cldm_nfa_defstate(lo_vers,       cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('i',          lo_versi)
);

/* state --ver */
cldm_nfa_defstate(lo_ver,        cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('s',          lo_vers)
);

/* state --ve */
cldm_nfa_defstate(lo_ve,         cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('r',          lo_ver)
);

/* state --v */
cldm_nfa_defstate(lo_v,          cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('e',          lo_ve)
);

/* state --no-capture */
cldm_nfa_defstate(lo_no_capture, cldm_nfa_pending, cldm_argtype_long, 's',
    cldm_nfa_transition(cldm_nfa_end, accept)
);

/* state --no-captur */
cldm_nfa_defstate(lo_no_captur,  cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('e',          lo_no_capture)
);

/* state --no-captu */
cldm_nfa_defstate(lo_no_captu,   cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('r',          lo_no_captur)
);

/* state --no-capt */
cldm_nfa_defstate(lo_no_capt,    cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('u',          lo_no_captu)
);

/* state --no-cap */
cldm_nfa_defstate(lo_no_cap,     cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('t',          lo_no_capt)
);

/* state --no-ca */
cldm_nfa_defstate(lo_no_ca,      cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('p',          lo_no_cap)
);

/* state --no-c */
cldm_nfa_defstate(lo_no_c,       cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('a',          lo_no_ca)
);

/* state --no- */
cldm_nfa_defstate(lo_no_,        cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('c',          lo_no_c)
);

/* state --no */
cldm_nfa_defstate(lo_no,         cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('-',          lo_no_)
);

/* state --n */
cldm_nfa_defstate(lo_n,          cldm_nfa_pending, cldm_argtype_long, CLDM_SHOPT_NONE,
    cldm_nfa_transition('o',          lo_no)
);

/* state -- */
cldm_nfa_defstate(double_dash,   cldm_nfa_pending, cldm_argtype_divider, CLDM_SHOPT_NONE,
    cldm_nfa_transition('n',          lo_n),
    cldm_nfa_transition('h',          lo_h),
    cldm_nfa_transition('f',          lo_f),
    cldm_nfa_transition('v',          lo_v),
    cldm_nfa_transition(cldm_nfa_end, accept)
);

/* state - */
cldm_nfa_defstate(single_dash,   cldm_nfa_pending, cldm_argtype_short, CLDM_SHOPT_NONE,
    cldm_nfa_transition('s',          sh_no_capture),
    cldm_nfa_transition('V',          sh_version),
    cldm_nfa_transition('x',          sh_fail_fast),
    cldm_nfa_transition('h',          sh_help),
    cldm_nfa_transition('-',          double_dash),
    cldm_nfa_transition(cldm_nfa_end, reject),
    cldm_nfa_transition(cldm_nfa_any, reject)
);

cldm_nfa_defstate(start,         cldm_nfa_pending, cldm_argtype_none, CLDM_SHOPT_NONE,
    cldm_nfa_transition('-',              single_dash),
    cldm_nfa_transition(cldm_nfa_epsilon, posparam)
);

static bool cldm_nfa_edge_match(char const **restrict p, struct cldm_nfa_edge const *restrict edge) {
    switch(edge->c) {
        case cldm_nfa_any:
            ++(*p);
            return true;
        case cldm_nfa_end:
            return !(**p);
        case cldm_nfa_epsilon:
            return true;
        default:
            break;
    }

    if(**p == (char)edge->c) {
        ++(*p);
        return true;
    }

    return false;
}

static void cldm_argp_setarg(int opt, struct cldm_args *args) {
    switch(opt) {
        case 'h':
            args->help = true;
            break;
        case 'V':
            args->version = true;
            break;
        case 's':
            args->no_capture = true;
            break;
        case 'x':
            args->fail_fast = true;
            break;
        default:
            cldm_rtassert(false, "Invalid argument index %d", opt);
    }
}

static inline bool cldm_nfa_accept_shopt(struct cldm_nfa_state2 const *state) {
    return state->action == cldm_nfa_accept && state->argtype == cldm_argtype_short;
}

static inline bool cldm_nfa_valid_state(struct cldm_nfa_state2 const *state) {
    return state->action == cldm_nfa_pending || cldm_nfa_accept_shopt(state);
}

static struct cldm_nfa_state2 *cldm_nfa_simulate(struct cldm_args *restrict args, char **restrict input) {
    /* (Ab)use the fact that a struct may alias with its first member */
    union {
        enum cldm_nfa_action *action;
        struct cldm_nfa_state2 *state;
    } s_un;
    static bool treat_as_posparam = false;

    struct cldm_nfa_state2 *state;
    char const *p;

    state = 0;
    s_un.action = treat_as_posparam ? &cldm_nfa_state_posparam.action : &cldm_nfa_state_start.action;
    p = input[0];

    while(cldm_nfa_valid_state(s_un.state)) {
        /* Try each edge */
        for(unsigned i = 0;; i++) {
            if(cldm_nfa_edge_match(&p, &s_un.state->edges[i])) {
                state = s_un.state;
                s_un.action = s_un.state->edges[i].end;
                break;
            }
        }
        /* Accept if valid short option */
        if(cldm_nfa_accept_shopt(s_un.state)) {
            cldm_argp_setarg(s_un.state->shopt, args);
        }
    }

    if(!*s_un.action) {
        if(state->argtype == cldm_argtype_short) {
            cldm_err("Invalid option -- '%c'", *(p - 1));
        }
        else {
            cldm_err("Invalid option '%s'", input[0]);
        }

        return 0;
    }

    /* Arg is '--', remaining args are positional parameters */
    if(state->argtype == cldm_argtype_divider) {
        treat_as_posparam = true;
    }
    /* Valid switch */
    else if(state->argtype != cldm_argtype_posparam) {
        cldm_argp_setarg(state->shopt, args);
    }

    return state;
}

/* Predicate for cldm_stable_partition */
static bool cldm_argp_is_posparam(void const *param) {
    (void)param;
    static unsigned i = 0;
    return cldm_argp_states[i++]->argtype != cldm_argtype_posparam;
}

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

void cldm_argp_version(void) {
    cldm_log("cldm version " cldm_str_expand(CLDM_VERSION));
    cldm_log("Report bugs to vilhelm.engstrom@tuta.io\n");
}

bool cldm_argp_parse(struct cldm_args *restrict args, int argc, char **restrict argv) {
    bool success;
    ssize_t posind;

    success = false;
    *args = (struct cldm_args){ 0 };

    /* For telling positional parameters and switches apart when partitioning */
    cldm_argp_states = malloc((argc - 1) * sizeof(*cldm_argp_states));

    if(!cldm_argp_states) {
        cldm_err("Could not allocate memory for positional parameters");
        return false;
    }

    for(int i = 0; i < argc - 1; i++) {
        cldm_argp_states[i] = cldm_nfa_simulate(args, &argv[i + 1]);
        if(!cldm_argp_states[i]) {
            goto epilogue;
        }
    }

    /* Partition s.t. positional parameters are at the end of argv */
    posind = cldm_stable_partition(&argv[1], sizeof(*argv), argc - 1, cldm_argp_is_posparam);

    /* Partition failure */
    if(posind < 0) {
        goto epilogue;
    }

    /* Account for argv[0] */
    args->posind = (size_t)posind + 1;

    success = true;
epilogue:
    free(cldm_argp_states);

    return success;
}
