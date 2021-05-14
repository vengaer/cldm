#include <cldm/cldm.h>
#include <cldm/cldm_mock.h>
#include <cldm/cldm_thread.h>

TEST(cldm_mock_disable) {
    unsigned thread_id = cldm_thread_id();
    ASSERT_FALSE(cldm_mock_disabled());

    cldm_mock_disable() {
        ASSERT_TRUE(cldm_mock_disabled());
    }

    ASSERT_FALSE(cldm_mock_disabled());

    cldm_mock_force_disable[thread_id].data = true;

    cldm_mock_disable() {
        ASSERT_TRUE(cldm_mock_disabled());
    }

    ASSERT_TRUE(cldm_mock_disabled());

    cldm_mock_force_disable[thread_id].data = false;

    cldm_mock_disable() {
        ASSERT_TRUE(cldm_mock_disabled());
    }

    ASSERT_FALSE(cldm_mock_disabled());
}

TEST(cldm_mock_enable) {
    unsigned thread_id = cldm_thread_id();
    ASSERT_FALSE(cldm_mock_disabled());

    cldm_mock_enable() {
        ASSERT_FALSE(cldm_mock_disabled());
    }

    ASSERT_FALSE(cldm_mock_disabled());
    cldm_mock_force_disable[thread_id].data = true;

    cldm_mock_enable() {
        ASSERT_FALSE(cldm_mock_disabled());
    }

    ASSERT_TRUE(cldm_mock_disabled());

    cldm_mock_force_disable[thread_id].data = false;

    cldm_mock_enable() {
        ASSERT_FALSE(cldm_mock_disabled());
    }

    ASSERT_FALSE(cldm_mock_disabled());
}
