#include <cldm/cldm.h>
#include <cldm/cldm_argp.h>
#include <cldm/cldm_io.h>

#include <stddef.h>
#include <string.h>

TEST(cldm_argp_parse_help) {
    struct cldm_args args;
    char progname[64];
    char shortsw[64];
    char longsw[64];

    strcpy(progname, "progname");
    strcpy(shortsw, "-h");
    strcpy(longsw, "--help");

    char *shortopt[] = {
        progname,
        shortsw,
        0
    };
    char *longopt[] = {
        progname,
        longsw,
        0
    };

    ASSERT_TRUE(cldm_argp_parse(&args, 2, shortopt));
    ASSERT_TRUE(args.help);

    args = (struct cldm_args) { 0 };
    ASSERT_TRUE(cldm_argp_parse(&args, 2, longopt));
    ASSERT_TRUE(args.help);

    strcpy(longsw, "--usage");
    args = (struct cldm_args) { 0 };
    ASSERT_TRUE(cldm_argp_parse(&args, 2, longopt));
    ASSERT_TRUE(args.help);
}

TEST(cldm_argp_parse_verbose) {
    struct cldm_args args;
    char progname[64];
    char shortsw[64];
    char longsw[64];

    strcpy(progname, "progname");
    strcpy(shortsw, "-v");
    strcpy(longsw, "--verbose");

    char *shortopt[] = {
        progname,
        shortsw,
        0
    };
    char *longopt[] = {
        progname,
        longsw,
        0
    };

    ASSERT_TRUE(cldm_argp_parse(&args, 2, shortopt));
    ASSERT_TRUE(args.verbose);

    args = (struct cldm_args) { 0 };
    ASSERT_TRUE(cldm_argp_parse(&args, 2, longopt));
    ASSERT_TRUE(args.verbose);
}

TEST(cldm_argp_parse_version) {
    struct cldm_args args;
    char progname[64];
    char shortsw[64];
    char longsw[64];

    strcpy(progname, "progname");
    strcpy(shortsw, "-V");
    strcpy(longsw, "--version");

    char *shortopt[] = {
        progname,
        shortsw,
        0
    };
    char *longopt[] = {
        progname,
        longsw,
        0
    };

    ASSERT_TRUE(cldm_argp_parse(&args, 2, shortopt));
    ASSERT_TRUE(args.version);

    args = (struct cldm_args) { 0 };
    ASSERT_TRUE(cldm_argp_parse(&args, 2, longopt));
    ASSERT_TRUE(args.version);
}

TEST(cldm_argp_parse_fail_fast) {
    struct cldm_args args;
    char progname[64];
    char shortsw[64];
    char longsw[64];

    strcpy(progname, "progname");
    strcpy(shortsw, "-x");
    strcpy(longsw, "--fail-fast");

    char *shortopt[] = {
        progname,
        shortsw,
        0
    };
    char *longopt[] = {
        progname,
        longsw,
        0
    };

    ASSERT_TRUE(cldm_argp_parse(&args, 2, shortopt));
    ASSERT_TRUE(args.fail_fast);

    args = (struct cldm_args) { 0 };
    ASSERT_TRUE(cldm_argp_parse(&args, 2, longopt));
    ASSERT_TRUE(args.fail_fast);
}

TEST(cldm_argp_parse_capture) {
    struct cldm_args args;
    char progname[64];
    char opt0[64];
    char opt1[64];
    char const **it0;
    enum cldm_capture *it1;

    char const *capture_opts[] = {
        "none",
        "stdout",
        "stderr",
        "all"
    };
    enum cldm_capture captures[] = {
        cldm_capture_none,
        cldm_capture_stdout,
        cldm_capture_stderr,
        cldm_capture_all
    };

    strcpy(progname, "progname");

    char *opts[4];

    cldm_for_each_zip(it0, it1, capture_opts, captures) {
        ASSERT_LT((size_t)snprintf(opt0, sizeof(opt0), "-c%s", *it0), sizeof(opt0));
        opt1[0] = '\0';

        opts[0] = progname;
        opts[1] = opt0;
        opts[2] = opt1;
        opts[3] = 0;

        args = (struct cldm_args) { 0 };
        ASSERT_TRUE(cldm_argp_parse(&args, 2, opts));
        ASSERT_EQ(args.capture, *it1);

        strcpy(opt0, "-c");
        strcpy(opt1, *it0);

        opts[0] = progname;
        opts[1] = opt0;
        opts[2] = opt1;
        opts[3] = 0;

        args = (struct cldm_args) { 0 };
        ASSERT_TRUE(cldm_argp_parse(&args, 3, opts));
        ASSERT_EQ(args.capture, *it1);
    }
}

TEST(cldm_argp_parse_capture_none) {
    struct cldm_args args;
    char progname[64];
    char shortsw[64];
    char longsw[64];

    strcpy(progname, "progname");
    strcpy(shortsw, "-s");
    strcpy(longsw, "--capture-none");

    char *shortopt[] = {
        progname,
        shortsw,
        0
    };
    char *longopt[] = {
        progname,
        longsw,
        0
    };

    ASSERT_TRUE(cldm_argp_parse(&args, 2, shortopt));
    ASSERT_EQ(args.capture, cldm_capture_none);

    args = (struct cldm_args) { 0 };
    ASSERT_TRUE(cldm_argp_parse(&args, 2, longopt));
    ASSERT_EQ(args.capture, cldm_capture_none);
}

TEST(cldm_argp_parse_redirect) {
    struct cldm_args args;
    char progname[64];
    char opt0[64];
    char opt1[64];

    char *opts[] = {
        progname,
        opt0,
        opt1,
        0
    };

    strcpy(progname, "progname");
    strcpy(opt0, "-dout.log");
    opt1[0] = '\0';

    ASSERT_TRUE(cldm_argp_parse(&args, 2, opts));
    ASSERT_STREQ(args.redirect, "out.log");

    strcpy(opt0, "-d");
    strcpy(opt1, "out.log");

    ASSERT_TRUE(cldm_argp_parse(&args, 3, opts));
    ASSERT_STREQ(args.redirect, "out.log");

    strcpy(opt0, "--redirect-captures=out.log");
    opt1[0] = '\0';

    ASSERT_TRUE(cldm_argp_parse(&args, 2, opts));
    ASSERT_STREQ(args.redirect, "out.log");
}

TEST(cldm_argp_parse_positional_parameter) {
    struct cldm_args args;
    char progname[64];
    char opt0[64];
    char opt1[64];

    char *opts[] = {
        progname,
        opt0,
        opt1,
        0
    };

    strcpy(progname, "progname");
    strcpy(opt0, "-v");
    strcpy(opt1, "file.c");
    ASSERT_TRUE(cldm_argp_parse(&args, 3, opts));
    ASSERT_EQ(args.posparams, &opts[2]);
    ASSERT_EQ(args.nposparams, 1u);
}

TEST(cldm_argp_parse_positional_parameter_double_dash) {
    struct cldm_args args;
    char progname[64];
    char opt0[64];
    char opt1[64];
    char opt2[64];
    char opt3[64];

    char *opts[] = {
        progname,
        opt0,
        opt1,
        opt2,
        opt3,
        0
    };

    strcpy(progname, "progname");
    strcpy(opt0, "-v");
    strcpy(opt1, "--");
    strcpy(opt2, "-h");
    strcpy(opt3, "--version");
    ASSERT_TRUE(cldm_argp_parse(&args, 5, opts));
    ASSERT_EQ(args.posparams, &opts[3]);
    ASSERT_EQ(args.nposparams, 2);
    ASSERT_STREQ(*args.posparams, opt2);
}
