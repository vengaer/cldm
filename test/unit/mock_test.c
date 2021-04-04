#include <cldm/cldm.h>
#include <cldm/cldm_mock.h>

TEST(cldm_mock_disable) {
    ASSERT_FALSE(cldm_mock_force_disable);

    cldm_mock_disable() {
        ASSERT_TRUE(cldm_mock_force_disable);
    }

    ASSERT_FALSE(cldm_mock_force_disable);

    cldm_mock_force_disable = true;

    cldm_mock_disable() {
        ASSERT_TRUE(cldm_mock_force_disable);
    }

    ASSERT_TRUE(cldm_mock_force_disable);

    cldm_mock_force_disable = false;

    cldm_mock_disable() {
        ASSERT_TRUE(cldm_mock_force_disable);
    }

    ASSERT_FALSE(cldm_mock_force_disable);
}

TEST(cldm_mock_enable) {
    ASSERT_FALSE(cldm_mock_force_disable);

    cldm_mock_enable() {
        ASSERT_FALSE(cldm_mock_force_disable);
    }

    ASSERT_FALSE(cldm_mock_force_disable);
    cldm_mock_force_disable = true;

    cldm_mock_enable() {
        ASSERT_FALSE(cldm_mock_force_disable);
    }

    ASSERT_TRUE(cldm_mock_force_disable);

    cldm_mock_force_disable = false;

    cldm_mock_enable() {
        ASSERT_FALSE(cldm_mock_force_disable);
    }

    ASSERT_FALSE(cldm_mock_force_disable);
}
