#ifndef CLDM_NORETURN_H
#define CLDM_NORETURN_H

#include "cldm_cache.h"
#include "cldm_config.h"
#include "cldm_jmpstack.h"
#include "cldm_thread.h"

#include <setjmp.h>

typedef cldm_cachealign(struct cldm_jmpstack) cldm_aligned_jmpstack;

extern cldm_aligned_jmpstack cldm_jmpstacks[CLDM_MAX_THREADS];

#ifdef CLDM_HAS_NORETURN
#define cldm_noreturn_spec _Noreturn
#else
#define cldm_noreturn_spec
#endif

#define CLDM_NORETURN_CALL()                                                                                        \
    if(!setjmp((cldm_jmpstack_push(&cldm_jmpstack[cldm_thread_id[]], &(jmp_buf){ 0 }),*cldm_jmpstack_top(cldm_jmpstacks[cldm_thread_id()]))))

#ifndef CLDM_PREFIX_ONLY
#define NORETURN_CALL() CLDM_NORETURN_CALL()
#endif

#endif /* CLDM_NORETURN_H */
