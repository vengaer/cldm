#include <cldm/cldm.h>
#include <cldm/cldm_nfa.h>

#include <string.h>

TEST(cldm_nfa_simulation_no_branch) {
    struct cldm_nfa nfa;
    ASSERT_TRUE(cldm_nfa_init(&nfa));

    ASSERT_TRUE(cldm_nfa_add_argument(&nfa, "vertical"));

    ASSERT_EQ(cldm_nfa_simulate(&nfa, "vertical"), 8);
    ASSERT_EQ(cldm_nfa_simulate(&nfa, "vert"), -1);
    ASSERT_EQ(cldm_nfa_simulate(&nfa, "vertigo"), -1);
    ASSERT_EQ(cldm_nfa_simulate(&nfa, "poison"), -1);

    cldm_nfa_free(&nfa);
}


TEST(cldm_nfa_simulation_branch) {
    struct cldm_nfa nfa;
    char const *arg0 = "vertical";
    char const *arg1 = "vertigo";
    char const *arg2 = "vertigial";

    ASSERT_TRUE(cldm_nfa_init(&nfa));

    ASSERT_TRUE(cldm_nfa_add_argument(&nfa, arg0));
    ASSERT_TRUE(cldm_nfa_add_argument(&nfa, arg1));
    ASSERT_TRUE(cldm_nfa_add_argument(&nfa, arg2));

    ASSERT_EQ(cldm_nfa_simulate(&nfa, arg0), (int)strlen(arg0));
    ASSERT_EQ(cldm_nfa_simulate(&nfa, arg1), (int)strlen(arg1));
    ASSERT_EQ(cldm_nfa_simulate(&nfa, arg2), (int)strlen(arg2));

    cldm_nfa_free(&nfa);
}
