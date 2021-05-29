#include <cldm/cldm.h>

#include <pthread.h>

TEST(lock_guard) {
    pthread_mutex_t mutex;
    ASSERT_EQ(pthread_mutex_init(&mutex, 0), 0);
    ASSERT_EQ(pthread_mutex_trylock(&mutex), 0);
    pthread_mutex_unlock(&mutex);
    cldm_lock_guard(&mutex, pthread_mutex_lock, pthread_mutex_unlock) {
        ASSERT_NE(pthread_mutex_trylock(&mutex), 0);
    }
    ASSERT_EQ(pthread_mutex_trylock(&mutex), 0);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
}
