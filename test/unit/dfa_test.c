#include <cldm/cldm.h>
#include <cldm/cldm_dfa.h>

#include <string.h>

TEST(cldm_dfa_simulation_no_branch) {
    struct cldm_dfa dfa;
    ASSERT_TRUE(cldm_dfa_init(&dfa));

    ASSERT_TRUE(cldm_dfa_add_argument(&dfa, "vertical"));

    ASSERT_EQ(cldm_dfa_simulate(&dfa, "vertical"), 8);
    ASSERT_EQ(cldm_dfa_simulate(&dfa, "vert"), -1);
    ASSERT_EQ(cldm_dfa_simulate(&dfa, "vertigo"), -1);
    ASSERT_EQ(cldm_dfa_simulate(&dfa, "poison"), -1);

    cldm_dfa_free(&dfa);
}


TEST(cldm_dfa_simulation_branch) {
    struct cldm_dfa dfa;
    char const *arg0 = "vertical";
    char const *arg1 = "vertigo";
    char const *arg2 = "vertigial";

    ASSERT_TRUE(cldm_dfa_init(&dfa));

    ASSERT_TRUE(cldm_dfa_add_argument(&dfa, arg0));
    ASSERT_TRUE(cldm_dfa_add_argument(&dfa, arg1));
    ASSERT_TRUE(cldm_dfa_add_argument(&dfa, arg2));

    ASSERT_EQ(cldm_dfa_simulate(&dfa, arg0), (int)strlen(arg0));
    ASSERT_EQ(cldm_dfa_simulate(&dfa, arg1), (int)strlen(arg1));
    ASSERT_EQ(cldm_dfa_simulate(&dfa, arg2), (int)strlen(arg2));

    cldm_dfa_free(&dfa);
}
