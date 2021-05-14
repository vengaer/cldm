#include "cldm_thread.h"

unsigned cldm_jobs;
pthread_t cldm_threads[CLDM_MAX_THREADS - 1];

int (*cldm_mutex_init)(pthread_mutex_t *restrict, pthread_mutexattr_t const *restrict);
int (*cldm_mutex_destroy)(pthread_mutex_t *);
int (*cldm_mutex_lock)(pthread_mutex_t *);
int (*cldm_mutex_unlock)(pthread_mutex_t *);

/* Dummies used in single-threaded contexts to avoid locking overhead */
static int cldm_dummy_mutex_init(pthread_mutex_t *restrict mutex, pthread_mutexattr_t const *restrict attr) {
    (void)attr;
    return !mutex;
}

static int cldm_dummy_mutex_destroy(pthread_mutex_t *mutex) {
    return !mutex;
}

static int cldm_dummy_mutex_lock(pthread_mutex_t *mutex) {
    return !mutex;
}

static int cldm_dummy_mutex_unlock(pthread_mutex_t *mutex) {
    return !mutex;
}

void cldm_thread_init(unsigned jobs) {
    cldm_jobs = jobs;

    if(jobs == 1) {
        cldm_mutex_init = cldm_dummy_mutex_init;
        cldm_mutex_destroy = cldm_dummy_mutex_destroy;
        cldm_mutex_lock = cldm_dummy_mutex_lock;
        cldm_mutex_unlock = cldm_dummy_mutex_unlock;
    }
    else {
        cldm_mutex_init = pthread_mutex_init;
        cldm_mutex_destroy = pthread_mutex_destroy;
        cldm_mutex_lock = pthread_mutex_lock;
        cldm_mutex_unlock = pthread_mutex_unlock;
    }
}

unsigned cldm_thread_id(void) {
    pthread_t ptid;
    ptid = pthread_self();
    for(unsigned i = 1; i < cldm_jobs; i++) {
        if(pthread_equal(ptid, cldm_threads[i - 1])) {
            return i;
        }
    }
    return 0;
}
