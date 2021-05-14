#ifndef CLDM_AUXPROCS_H
#define CLDM_AUXPROCS_H

#include "cldm_type.h"


struct cldm_auxprocs {
    cldm_setup_handle local_setup;
    cldm_setup_handle global_setup;
    cldm_teardown_handle local_teardown;
    cldm_teardown_handle global_teardown;
};

#define cldm_local_setup_ident          \
    cldm_local_setup
#define cldm_local_teardown_ident       \
    cldm_local_teardown
#define cldm_global_setup_ident         \
    cldm_global_setup
#define cldm_global_teardown_ident      \
    cldm_global_teardown

#define CLDM_LOCAL_SETUP()              \
    void cldm_expand(cldm_local_setup_ident)(void)

#define CLDM_LOCAL_TEARDOWN()           \
    void cldm_expand(cldm_local_teardown_ident)(void)

#define CLDM_GLOBAL_SETUP()             \
    void cldm_expand(cldm_global_setup_ident)(void)

#define CLDM_GLOBAL_TEARDOWN()          \
    void cldm_expand(cldm_global_teardown_ident)(void)

#define CLDM_UNBOUND_SETUP(name)        \
    void name(void)

#define CLDM_UNBOUND_TEARDOWN(name)     \
    void name(void)

#ifndef CLDM_PREFIX_ONLY
#define LOCAL_SETUP()         CLDM_LOCAL_SETUP()
#define LOCAL_TEARDOWN()      CLDM_LOCAL_TEARDOWN()

#define GLOBAL_SETUP()        CLDM_GLOBAL_SETUP()
#define GLOBAL_TEARDOWN()     CLDM_GLOBAL_TEARDOWN()

#define UNBOUND_SETUP(...)    CLDM_UNBOUND_SETUP(__VA_ARGS__)
#define UNBOUND_TEARDOWN(...) CLDM_UNBOUND_TEARDOWN(__VA_ARGS__)
#endif


#endif /* CLDM_AUXPROCS_H */
