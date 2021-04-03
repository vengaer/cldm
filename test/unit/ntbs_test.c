#include <cldm/cldm.h>
#include <cldm/cldm_log.h>
#include <cldm/cldm_ntbs.h>

#include <string.h>

#include <sys/types.h>

TEST(cldm_strscpy) {
    enum { BUFSIZE = 64 };
    char src[BUFSIZE];
    char dst[BUFSIZE];

    strcpy(src, "a string");

    ASSERT_EQ(cldm_strscpy(dst, src, sizeof(src)), (ssize_t)strlen(src));
    ASSERT_EQ(strcmp(dst, src), 0);

    ASSERT_LT(cldm_strscpy(dst, src, 4), 0);
    ASSERT_EQ(dst[3], 0);
    ASSERT_EQ(strcmp(dst, "a s"), 0);

    ASSERT_EQ(cldm_strscpy(dst, src, 9), (ssize_t)strlen(src));
    ASSERT_EQ(dst[strlen(src)], 0);
}

TEST(cldm_ntbs_find_substr) {
    char const *str = "a string with a substring";

    ASSERT_EQ(cldm_ntbs_find_substr(str, "substring"), str + 16);
    ASSERT_EQ(cldm_ntbs_find_substr(str, "string"), str + 2);
    ASSERT_EQ(cldm_ntbs_find_substr(str, "automata"), 0);
}

TEST(cldm_ntbscmp) {
    ASSERT_EQ(cldm_ntbscmp("a string", "a string"), 0);
    ASSERT_LT(cldm_ntbscmp("a strin", "a string"), 0);
    ASSERT_GT(cldm_ntbscmp("a string", "a strin"), 0);
    ASSERT_LT(cldm_ntbscmp("between", "the"), 0);
    ASSERT_GT(cldm_ntbscmp("buried", "and"), 0);
    ASSERT_LT(cldm_ntbscmp("", "a"), 0);
}

TEST(cldm_ntbsncmp) {
    ASSERT_EQ(cldm_ntbsncmp("a string", "a str", 3), 0);
    ASSERT_LT(cldm_ntbsncmp("a strin", "a string", 100), 0);
    ASSERT_GT(cldm_ntbsncmp("b", "a", 1), 0);
    ASSERT_EQ(cldm_ntbsncmp("a string", "a string w", strlen("a string")), 0);
}

TEST(cldm_for_each_word) {
    char const *str = "a string with spaces";
    char const *words[] = {
        "a",
        "string",
        "with",
        "spaces"
    };
    char *iter;
    unsigned idx = 0;

    cldm_for_each_word(iter, str) {
        ASSERT_EQ(strcmp(words[idx++], iter), 0);
    }
    ASSERT_EQ(idx, 4);
    str = "a;string;with;spaces";
    idx = 0;
    cldm_for_each_word(iter, str, ';') {
        ASSERT_EQ(strcmp(words[idx++], iter), 0);
    }
    ASSERT_EQ(idx, 4);
}
