#include <cldm/cldm.h>
#include <cldm/cldm_dfa.h>

#include <string.h>

TEST(cldm_dfa_simulation_no_branch) {
    struct cldm_dfa dfa;
    int c;
    ASSERT_TRUE(cldm_dfa_init(&dfa));

    ASSERT_TRUE(cldm_dfa_add_argument(&dfa, "vertical", 'v'));

    ASSERT_EQ(cldm_dfa_simulate(&dfa, "vertical", &c), 8);
    ASSERT_EQ(c, 'v');
    ASSERT_EQ(cldm_dfa_simulate(&dfa, "vert", &c), -1);
    ASSERT_EQ(c, 0);
    ASSERT_EQ(cldm_dfa_simulate(&dfa, "vertigo", &c), -1);
    ASSERT_EQ(c, 0);
    ASSERT_EQ(cldm_dfa_simulate(&dfa, "poison", &c), -1);
    ASSERT_EQ(c, 0);

    cldm_dfa_free(&dfa);
}


TEST(cldm_dfa_simulation_branch) {
    struct cldm_dfa dfa;
    char const *arg0 = "vertical";
    char const *arg1 = "vertigo";
    char const *arg2 = "vertigial";

    int c;

    ASSERT_TRUE(cldm_dfa_init(&dfa));

    ASSERT_TRUE(cldm_dfa_add_argument(&dfa, arg0, '0'));
    ASSERT_TRUE(cldm_dfa_add_argument(&dfa, arg1, '1'));
    ASSERT_TRUE(cldm_dfa_add_argument(&dfa, arg2, '2'));

    ASSERT_EQ(cldm_dfa_simulate(&dfa, arg0, &c), (int)strlen(arg0));
    ASSERT_EQ(c, '0');
    ASSERT_EQ(cldm_dfa_simulate(&dfa, arg1, &c), (int)strlen(arg1));
    ASSERT_EQ(c, '1');
    ASSERT_EQ(cldm_dfa_simulate(&dfa, arg2, &c), (int)strlen(arg2));
    ASSERT_EQ(c, '2');

    cldm_dfa_free(&dfa);
}
