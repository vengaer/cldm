#ifndef CLDM_THREAD_H
#define CLDM_THREAD_H

#include "cldm_macro.h"

#include <pthread.h>

#define CLDM_MAX_THREADS 32

extern unsigned cldm_jobs;
extern pthread_t cldm_threads[CLDM_MAX_THREADS - 1];

void cldm_thread_init(unsigned jobs);
unsigned cldm_thread_id(void);

extern int (*cldm_mutex_init)(pthread_mutex_t *restrict, pthread_mutexattr_t const *restrict);
extern int (*cldm_mutex_destroy)(pthread_mutex_t *);
extern int (*cldm_mutex_lock)(pthread_mutex_t *);
extern int (*cldm_mutex_unlock)(pthread_mutex_t *);

#define cldm_lock_guard(mutex, lock, unlock)    \
    for(int cldm_cat_expand(cldm_lock_gd,__LINE__) = (lock(mutex),0);   \
        !cldm_cat_expand(cldm_lock_gd,__LINE__);                        \
        cldm_cat_expand(cldm_lock_gd,__LINE__) = (unlock(mutex),1))

#define cldm_mutex_guard(mutex) cldm_lock_guard(mutex, cldm_mutex_lock, cldm_mutex_unlock)
#define cldm_rwlock_rdguard(lock) cldm_lock_guard(lock, pthread_rwlock_rdlock, pthread_rwlock_unlock)
#define cldm_rwlock_wrguard(lock) cldm_lock_guard(lock, pthread_rwlock_wrlock, pthread_rwlock_unlock)

#endif /* CLDM_THREAD_H */
