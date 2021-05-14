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

#define cldm_mutex_guard(mutex)                                                     \
    for(int cldm_cat_expand(cldm_mtx_gd,__LINE__) = (cldm_mutex_lock(mutex), 0);    \
        !cldm_cat_expand(cldm_mtx_gd,__LINE__);                                     \
        cldm_cat_expand(cldm_mtx_gd,__LINE__) = (cldm_mutex_unlock(mutex),1))

#endif /* CLDM_THREAD_H */
