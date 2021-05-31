#include <cldm/cldm.h>
#include <cldm/cldm_macro.h>
#include <cldm/cldm_token.h>

TEST(token_void) {
    ASSERT_TRUE(cldm_token_void(void));
    ASSERT_FALSE(cldm_token_void(int));
    ASSERT_FALSE(cldm_token_void(void *));
}

TEST(token_0) {
    ASSERT_TRUE(cldm_token_0(0));
    ASSERT_FALSE(cldm_token_0(1));
    ASSERT_FALSE(cldm_token_0(asdf));
}

TEST(token_1) {
    ASSERT_TRUE(cldm_token_1(1));
    ASSERT_FALSE(cldm_token_1(0));
    ASSERT_FALSE(cldm_token_1(asdf));
}

TEST(repeat_token) {
#define m(_, v)   ASSERT_EQ(v,30)
    cldm_map_separate(m, ;, cldm_repeat_token(30, 8));
#undef m
}
