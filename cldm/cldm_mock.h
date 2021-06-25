#ifndef CLDM_MOCK_H
#define CLDM_MOCK_H

#include "cldm_byteseq.h"
#include "cldm_cache.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_rtassert.h"
#include "cldm_thread.h"
#include "cldm_token.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { CLDM_MOCK_INFINITE = -1 };

struct cldm_mockinfo {
    void *addr;
    unsigned invocations_offset;
    unsigned opmode_offset;
};

enum cldm_mockop {
    cldm_mockop_invoke,
    cldm_mockop_return,
    cldm_mockop_increment,
    cldm_mockop_return_param,
    cldm_mockop_return_pointee,
    cldm_mockop_assign,
    cldm_mockop_assign_param,
    cldm_mockop_max
};

struct cldm_assign_opdata {
    void *lhs;
    void const *rhs;
    unsigned size;
};

struct cldm_assign_param_opdata {
    void *addr;
    unsigned idx;
    unsigned dstsize;
};

typedef cldm_cachealign(struct cldm_mockinfo *) cldm_aligned_mockinfo;
typedef cldm_cachealign(bool) cldm_aligned_bool;

extern cldm_aligned_mockinfo cldm_mockinfos[CLDM_MAX_THREADS];
extern cldm_aligned_bool cldm_mock_force_disable[CLDM_MAX_THREADS];
extern bool cldm_mock_global_context;

/* Must be called in a sequential context */
void cldm_mock_enable_all(void);
void cldm_mock_disable_all(void);

/* Thread safe */
inline bool cldm_mock_disabled(void) {
    return cldm_mock_force_disable[cldm_thread_id()].data;
}

inline bool cldm_mock_enabled(void) {
    return !cldm_mock_disabled();
}

#define cldm_mock_enable()                                                                                  \
    for(unsigned cldm_cat_expand(cldm_mockenbl_tid,__LINE__) = cldm_thread_id(),                            \
                 cldm_cat_expand(cldm_mockenbl_sav,__LINE__) =                                              \
                    cldm_mock_force_disable[cldm_cat_expand(cldm_mockenbl_tid,__LINE__)].data,              \
                 cldm_cat_expand(cldm_mockenbl_cur,__LINE__) =                                              \
                    (cldm_mock_force_disable[cldm_cat_expand(cldm_mockenbl_tid,__LINE__)].data = false);    \
        !cldm_cat_expand(cldm_mockenbl_cur,__LINE__);                                                       \
        cldm_cat_expand(cldm_mockenbl_cur,__LINE__) =                                                       \
            (cldm_mock_force_disable[cldm_cat_expand(cldm_mockenbl_tid,__LINE__)].data =                    \
                cldm_cat_expand(cldm_mockenbl_sav,__LINE__),true))

#define cldm_mock_disable()                                                                                 \
    for(unsigned cldm_cat_expand(cldm_mockdble_tid,__LINE__) = cldm_thread_id(),                            \
                 cldm_cat_expand(cldm_mockdble_sav,__LINE__) =                                              \
                    cldm_mock_force_disable[cldm_cat_expand(cldm_mockdble_tid,__LINE__)].data,              \
                 cldm_cat_expand(cldm_mockdble_cur,__LINE__) =                                              \
                    (cldm_mock_force_disable[cldm_cat_expand(cldm_mockdble_tid,__LINE__)].data = true);     \
        cldm_cat_expand(cldm_mockdble_cur,__LINE__);                                                        \
        cldm_cat_expand(cldm_mockdble_cur,__LINE__) =                                                       \
            (cldm_mock_force_disable[cldm_cat_expand(cldm_mockdble_tid,__LINE__)].data =                    \
                cldm_cat_expand(cldm_mockdble_sav,__LINE__), false))

/* May only be used in a sequential context */
#define cldm_mock_global()  \
    for(unsigned cldm_cat_expand(cldm_mock_gbl_sav,__LINE__) = cldm_mock_global_context,                    \
                 cldm_cat_expand(cldm_mock_gbl_cur,__LINE__) = (cldm_mock_global_context = true);           \
        cldm_cat_expand(cldm_mock_gbl_cur,__LINE__);                                                        \
        cldm_cat_expand(cldm_mock_gbl_cur,__LINE__) =                                                       \
            (cldm_mock_global_context = cldm_cat_expand(cldm_mock_gbl_sav,__LINE__), false))

#define cldm_mocktail(name)                                     \
    void cldm_cat_expand(cldm_mocktail_,name)(void)

#define cldm_name_parameter(ctr, type)                          \
    type cldm_cat_expand(param,ctr)

#define cldm_mock_name_params(...)                              \
    cldm_map_list(cldm_name_parameter, __VA_ARGS__)

#define cldm_mock_store_single_address(ctr, _)                  \
    paramaddrs[ctr] = (void *)&cldm_cat_expand(param,ctr)

#define cldm_mock_store_paramaddrs(...)                         \
    cldm_map_separate(cldm_mock_store_single_address, ;, __VA_ARGS__)

#define cldm_mock_store_single_size(ctr, _)                     \
    paramsizes[ctr] = (unsigned short)sizeof(cldm_cat_expand(param,ctr))

#define cldm_mock_store_paramsizes(...)                         \
    cldm_map_separate(cldm_mock_store_single_size, ;, __VA_ARGS__)

#define cldm_mock_paramname_single(ctr, _)                      \
    cldm_cat_expand(param, ctr)

#define cldm_mock_paramnames(...)                               \
    cldm_map_list(cldm_mock_paramname_single, __VA_ARGS__)

#define cldm_mock_populate_ctx_single(ctr, name)                                                    \
    [ctr].data = {                                                                                  \
        .info = {                                                                                   \
            .addr = &cldm_mock_ ## name[ctr].data,                                                  \
            .invocations_offset = cldm_offset(struct cldm_mock_ ## name ## _ctx, invocations),      \
            .opmode_offset = cldm_offset(struct cldm_mock_ ## name ## _ctx, opdata) +               \
                             cldm_offset(struct cldm_mock_ ## name ## _opdata, op)                  \
        },                                                                                          \
        .invocations = 0,                                                                           \
        .opdata = {                                                                                 \
            .op = cldm_mockop_invoke,                                                               \
            .un_act.invoke = 0                                                                      \
        }                                                                                           \
    }

#define cldm_mock_populate_ctx(name)    \
    cldm_aligned_mock_ ## name ## _ctx cldm_mock_ ## name[CLDM_MAX_THREADS] = {                     \
        cldm_map_list(cldm_mock_populate_ctx_single, cldm_repeat_token(name, 32))                   \
    }

#define cldm_mock_gen_procctx(name, ...)                                                            \
    struct cldm_mock_ ## name ## _ctx   {                                                           \
        struct cldm_mockinfo info;                                                                  \
        int invocations;                                                                            \
        struct cldm_mock_ ## name ## _opdata {                                                      \
            enum cldm_mockop op;                                                                    \
            union {                                                                                 \
                void(*invoke)(__VA_ARGS__);                                                         \
                unsigned paramidx;                                                                  \
                struct cldm_assign_opdata assign;                                                   \
                struct cldm_assign_param_opdata assign_param;                                       \
            } un_act;                                                                               \
        } opdata;                                                                                   \
    };                                                                                              \
    typedef cldm_cachealign(struct cldm_mock_ ## name ## _ctx) cldm_aligned_mock_ ## name ## _ctx

#define cldm_mock_gen_funcctx(rettype, name, ...)                                                   \
    struct cldm_mock_ ## name ## _ctx   {                                                           \
        struct cldm_mockinfo info;                                                                  \
        int invocations;                                                                            \
        struct cldm_mock_ ## name ## _opdata {                                                      \
            enum cldm_mockop op;                                                                    \
            union {                                                                                 \
                rettype(*invoke)(__VA_ARGS__);                                                      \
                rettype retval;                                                                     \
                unsigned counter;                                                                   \
                unsigned paramidx;                                                                  \
                struct cldm_assign_opdata assign;                                                   \
                struct cldm_assign_param_opdata assign_param;                                       \
            } un_act;                                                                               \
        } opdata;                                                                                   \
    };                                                                                              \
    typedef cldm_cachealign(struct cldm_mock_ ## name ## _ctx) cldm_aligned_mock_ ## name ## _ctx

#ifdef CLDM_GENERATE_SYMBOLS

extern char const *cldm_mockop_strings[cldm_mockop_max + 1];

#define cldm_mockop_str(code)                           \
    cldm_mockop_strings[((!(code / cldm_mockop_max)) * code + (!!(code / cldm_mockop_max)) * cldm_mockop_max)]

#define cldm_mockctx(name)                              \
    cldm_cat_expand(cldm_mock_, name)[thread_id].data

#define cldm_mock_procedure_nullary(rettype, name)                                                                  \
    cldm_mock_gen_procctx(name, void);                                                                              \
    cldm_mock_populate_ctx(name);                                                                                   \
    void name(void) {                                                                                               \
        cldm_static_assert(CLDM_MAX_THREADS == 32);                                                                 \
        unsigned thread_id = cldm_thread_id();                                                                      \
        void(*handle)(void);                                                                                        \
        if(cldm_mock_enabled() && cldm_mockctx(name).invocations) {                                                 \
            if(cldm_mockctx(name).invocations != CLDM_MOCK_INFINITE) {                                              \
                --cldm_mockctx(name).invocations;                                                                   \
            }                                                                                                       \
            switch(cldm_mockctx(name).opdata.op) {                                                                  \
                case cldm_mockop_invoke:                                                                            \
                    cldm_mockctx(name).opdata.un_act.invoke();                                                      \
                    return;                                                                                         \
                case cldm_mockop_return:                                                                            \
                    return;                                                                                         \
                case cldm_mockop_assign:                                                                            \
                    cldm_memcpy(cldm_mockctx(name).opdata.un_act.assign.lhs,                                        \
                                cldm_mockctx(name).opdata.un_act.assign.rhs,                                        \
                                cldm_mockctx(name).opdata.un_act.assign.size);                                      \
                    return;                                                                                         \
                default:                                                                                            \
                    cldm_rtassert(0, "Invalid action '%s' for nullary procedure",                                   \
                                 cldm_mockop_str(cldm_mockctx(name).opdata.op));                                    \
            }                                                                                                       \
        }                                                                                                           \
        cldm_mock_disable() {                                                                                       \
            *(void **) &handle = cldm_dlsym_next(cldm_str_expand(name));                                            \
            cldm_rtassert(handle, "%s not found in any of the shared libraries listed in DT_NEEDED",                \
                                  cldm_str_expand(name));                                                           \
            handle();                                                                                               \
            cldm_dlresolve(cldm_str_expand(name));                                                                  \
        }                                                                                                           \
    }                                                                                                               \
    cldm_mocktail(name)

#define cldm_mock_function_nullary(rettype, name)                                                                   \
    cldm_mock_gen_funcctx(rettype, name, void);                                                                     \
    cldm_mock_populate_ctx(name);                                                                                   \
    rettype name(void) {                                                                                            \
        cldm_static_assert(CLDM_MAX_THREADS == 32);                                                                 \
        unsigned thread_id = cldm_thread_id();                                                                      \
        rettype(*handle)(void);                                                                                     \
        rettype rv = (rettype){ 0 };                                                                                \
        if(cldm_mock_enabled() && cldm_mockctx(name).invocations) {                                                 \
            if(cldm_mockctx(name).invocations != CLDM_MOCK_INFINITE) {                                              \
                --cldm_mockctx(name).invocations;                                                                   \
            }                                                                                                       \
            switch(cldm_mockctx(name).opdata.op) {                                                                  \
                case cldm_mockop_invoke:                                                                            \
                    return cldm_mockctx(name).opdata.un_act.invoke();                                               \
                case cldm_mockop_return:                                                                            \
                    return cldm_mockctx(name).opdata.un_act.retval;                                                 \
                case cldm_mockop_increment:                                                                         \
                    ++cldm_mockctx(name).opdata.un_act.counter;                                                     \
                    if(sizeof(rettype) < sizeof(cldm_mockctx(name).opdata.un_act.counter)) {                        \
                        cldm_warn("Reinterpreting counter of %zu bytes in %s as object of size %zu "                \
                                  "may alter its value", sizeof(cldm_mockctx(name).opdata.un_act.counter),          \
                                  cldm_str_expand(name), sizeof(rettype));                                          \
                    }                                                                                               \
                    cldm_memcpy(&rv, &cldm_mockctx(name).opdata.un_act.counter, sizeof(rv));                        \
                    break;                                                                                          \
                case cldm_mockop_assign:                                                                            \
                    cldm_memcpy(cldm_mockctx(name).opdata.un_act.assign.lhs,                                        \
                                cldm_mockctx(name).opdata.un_act.assign.rhs,                                        \
                                cldm_mockctx(name).opdata.un_act.assign.size);                                      \
                    break;                                                                                          \
                default:                                                                                            \
                    cldm_rtassert(0, "Invalid action '%s' for nullary function",                                    \
                                  cldm_mockop_str(cldm_mockctx(name).opdata.op));                                   \
            }                                                                                                       \
            return rv;                                                                                              \
        }                                                                                                           \
        cldm_mock_disable() {                                                                                       \
            *(void **) &handle = cldm_dlsym_next(cldm_str_expand(name));                                            \
            cldm_rtassert(handle, "%s not found in any of the shared libraries listed in DT_NEEDED",                \
                                  cldm_str_expand(name));                                                           \
            rv = handle();                                                                                          \
            cldm_dlresolve(cldm_str_expand(name));                                                                  \
        }                                                                                                           \
        return rv;                                                                                                  \
    }                                                                                                               \
    cldm_mocktail(name)

#define cldm_mock_procedure_variadic(rettype, name, ...)                                                            \
    cldm_mock_gen_procctx(name, __VA_ARGS__);                                                                       \
    cldm_mock_populate_ctx(name);                                                                                   \
    void name(cldm_mock_name_params(__VA_ARGS__)) {                                                                 \
        cldm_static_assert(CLDM_MAX_THREADS == 32);                                                                 \
        void *paramaddrs[cldm_count(__VA_ARGS__)];                                                                  \
        unsigned short paramsizes[cldm_count(__VA_ARGS__)];                                                         \
        cldm_mock_store_paramaddrs(__VA_ARGS__);                                                                    \
        cldm_mock_store_paramsizes(__VA_ARGS__);                                                                    \
        unsigned thread_id = cldm_thread_id();                                                                      \
        void(*handle)(__VA_ARGS__);                                                                                 \
        if(cldm_mock_enabled() && cldm_mockctx(name).invocations) {                                                 \
            if(cldm_mockctx(name).invocations != CLDM_MOCK_INFINITE) {                                              \
                --cldm_mockctx(name).invocations;                                                                   \
            }                                                                                                       \
            switch(cldm_mockctx(name).opdata.op) {                                                                  \
                case cldm_mockop_invoke:                                                                            \
                    cldm_mockctx(name).opdata.un_act.invoke(cldm_mock_paramnames(__VA_ARGS__));                     \
                    break;                                                                                          \
                case cldm_mockop_return:                                                                            \
                    break;                                                                                          \
                case cldm_mockop_assign:                                                                            \
                    cldm_memcpy(cldm_mockctx(name).opdata.un_act.assign.lhs,                                        \
                                cldm_mockctx(name).opdata.un_act.assign.rhs,                                        \
                                cldm_mockctx(name).opdata.un_act.assign.size);                                      \
                    break;                                                                                          \
                case cldm_mockop_assign_param:                                                                      \
                    cldm_rtassert(cldm_mockctx(name).opdata.un_act.assign_param.idx < cldm_arrsize(paramaddrs),     \
                                  "Attempt to access parameter %u in %s which accepts only %zu",                    \
                                  cldm_mockctx(name).opdata.un_act.assign_param.idx + 1, cldm_str_expand(name),     \
                                  cldm_arrsize(paramaddrs));                                                        \
                    if(cldm_mockctx(name).opdata.un_act.assign_param.dstsize <                                      \
                            paramsizes[cldm_mockctx(name).opdata.un_act.assign_param.idx])                          \
                    {                                                                                               \
                        cldm_warn("Reinterpreting block of %hu bytes in %s as object of size %u "                   \
                                  "may alter its value", paramsizes[cldm_mockctx(name).opdata.un_act.paramidx],     \
                                  cldm_str_expand(name), cldm_mockctx(name).opdata.un_act.assign_param.dstsize);    \
                    }                                                                                               \
                    cldm_memcpy(cldm_mockctx(name).opdata.un_act.assign_param.addr,                                 \
                                paramaddrs[cldm_mockctx(name).opdata.un_act.assign_param.idx],                      \
                                cldm_mockctx(name).opdata.un_act.assign_param.dstsize);                             \
                    break;                                                                                          \
                default:                                                                                            \
                    cldm_rtassert(0, "Invalid action '%s' for variadic procedure",                                  \
                                  cldm_mockop_str(cldm_mockctx(name).opdata.op));                                   \
            }                                                                                                       \
            return;                                                                                                 \
        }                                                                                                           \
        cldm_mock_disable() {                                                                                       \
            *(void **) &handle = cldm_dlsym_next(cldm_str_expand(name));                                            \
            cldm_rtassert(handle, "%s not found in any of the shared libraries listed in DT_NEEDED",                \
                                  cldm_str_expand(name));                                                           \
            handle(cldm_mock_paramnames(__VA_ARGS__));                                                              \
            cldm_dlresolve(cldm_str_expand(name));                                                                  \
        }                                                                                                           \
    }                                                                                                               \
    cldm_mocktail(name)

#define cldm_mock_function_variadic(rettype, name, ...)                                                             \
    cldm_mock_gen_funcctx(rettype, name, __VA_ARGS__);                                                              \
    cldm_mock_populate_ctx(name);                                                                                   \
    rettype name(cldm_mock_name_params(__VA_ARGS__)) {                                                              \
        void *paramaddrs[cldm_count(__VA_ARGS__)];                                                                  \
        unsigned short paramsizes[cldm_count(__VA_ARGS__)];                                                         \
        cldm_mock_store_paramaddrs(__VA_ARGS__);                                                                    \
        cldm_mock_store_paramsizes(__VA_ARGS__);                                                                    \
        unsigned thread_id = cldm_thread_id();                                                                      \
        rettype(*handle)(__VA_ARGS__);                                                                              \
        rettype rv = (rettype) { 0 };                                                                               \
        if(cldm_mock_enabled() && cldm_mockctx(name).invocations) {                                                 \
            if(cldm_mockctx(name).invocations != CLDM_MOCK_INFINITE) {                                              \
                --cldm_mockctx(name).invocations;                                                                   \
            }                                                                                                       \
            switch(cldm_mockctx(name).opdata.op) {                                                                  \
                case cldm_mockop_invoke:                                                                            \
                    return cldm_mockctx(name).opdata.un_act.invoke(cldm_mock_paramnames(__VA_ARGS__));              \
                case cldm_mockop_return:                                                                            \
                    return cldm_mockctx(name).opdata.un_act.retval;                                                 \
                case cldm_mockop_increment:                                                                         \
                    ++cldm_mockctx(name).opdata.un_act.counter;                                                     \
                    if(sizeof(rettype) < sizeof(cldm_mockctx(name).opdata.un_act.counter)) {                        \
                        cldm_warn("Reinterpreting counter of %zu bytes in %s as object of size %zu "                \
                                  "may alter its value", sizeof(cldm_mockctx(name).opdata.un_act.counter),          \
                                  cldm_str_expand(name), sizeof(rettype));                                          \
                    }                                                                                               \
                    cldm_memcpy(&rv, &cldm_mockctx(name).opdata.un_act.counter, sizeof(rv));                        \
                    break;                                                                                          \
                case cldm_mockop_return_param:                                                                      \
                    cldm_rtassert(cldm_mockctx(name).opdata.un_act.paramidx < cldm_arrsize(paramaddrs),             \
                                  "Attempt to access parameter %u in %s which accepts only %zu",                    \
                                  cldm_mockctx(name).opdata.un_act.paramidx + 1, cldm_str_expand(name),             \
                                  cldm_arrsize(paramaddrs));                                                        \
                    if(sizeof(rettype) < paramsizes[cldm_mockctx(name).opdata.un_act.paramidx]) {                   \
                        cldm_warn("Reinterpreting block of %hu bytes in %s as object of size %zu "                  \
                                  "may alter its value", paramsizes[cldm_mockctx(name).opdata.un_act.paramidx],     \
                                  cldm_str_expand(name), sizeof(rettype));                                          \
                    }                                                                                               \
                    cldm_memcpy(&rv, paramaddrs[cldm_mockctx(name).opdata.un_act.paramidx],                         \
                                paramsizes[cldm_mockctx(name).opdata.un_act.paramidx]);                             \
                    break;                                                                                          \
                case cldm_mockop_return_pointee:                                                                    \
                    cldm_rtassert(cldm_mockctx(name).opdata.un_act.paramidx < cldm_arrsize(paramaddrs),             \
                                  "Attempt to access parameter %u in %s which accepts only %zu",                    \
                                  cldm_mockctx(name).opdata.un_act.paramidx + 1, cldm_str_expand(name),             \
                                  cldm_arrsize(paramaddrs));                                                        \
                    /* Atrocious hack to circumvent array subscript bug in GCC 11 */                                \
                    cldm_memcpy(&rv, **(rettype ***)cldm_memcpy(&(rettype **) { 0 },                                \
                                &paramaddrs[cldm_mockctx(name).opdata.un_act.paramidx],                             \
                                sizeof(rettype **)), sizeof(rv));                                                   \
                    break;                                                                                          \
                case cldm_mockop_assign:                                                                            \
                    cldm_memcpy(cldm_mockctx(name).opdata.un_act.assign.lhs,                                        \
                                cldm_mockctx(name).opdata.un_act.assign.rhs,                                        \
                                cldm_mockctx(name).opdata.un_act.assign.size);                                      \
                    break;                                                                                          \
                case cldm_mockop_assign_param:                                                                      \
                    cldm_rtassert(cldm_mockctx(name).opdata.un_act.assign_param.idx < cldm_arrsize(paramaddrs),     \
                                  "Attempt to access parameter %u in %s which accepts only %zu",                    \
                                  cldm_mockctx(name).opdata.un_act.assign_param.idx + 1, cldm_str_expand(name),     \
                                  cldm_arrsize(paramaddrs));                                                        \
                    if(cldm_mockctx(name).opdata.un_act.assign_param.dstsize <                                      \
                            paramsizes[cldm_mockctx(name).opdata.un_act.assign_param.idx])                          \
                    {                                                                                               \
                        cldm_warn("Reinterpreting block of %hu bytes in %s as object of size %u "                   \
                                  "may alter its value", paramsizes[cldm_mockctx(name).opdata.un_act.paramidx],     \
                                  cldm_str_expand(name), cldm_mockctx(name).opdata.un_act.assign_param.dstsize);    \
                    }                                                                                               \
                    cldm_memcpy(cldm_mockctx(name).opdata.un_act.assign_param.addr,                                 \
                                paramaddrs[cldm_mockctx(name).opdata.un_act.assign_param.idx],                      \
                                cldm_mockctx(name).opdata.un_act.assign_param.dstsize);                             \
                    break;                                                                                          \
                default:                                                                                            \
                    cldm_rtassert(0, "Invalid action '%s' for variadic function",                                   \
                                  cldm_mockop_str(cldm_mockctx(name).opdata.op));                                   \
            }                                                                                                       \
            return rv;                                                                                              \
        }                                                                                                           \
        cldm_mock_disable() {                                                                                       \
            *(void **) &handle = cldm_dlsym_next(cldm_str_expand(name));                                            \
            cldm_rtassert(handle, "%s not found in shared libraries listed in DT_NEEDED", cldm_str_expand(name));   \
            rv = handle(cldm_mock_paramnames(__VA_ARGS__));                                                         \
            cldm_dlresolve(cldm_str_expand(name));                                                                  \
        }                                                                                                           \
        return rv;                                                                                                  \
    }                                                                                                               \
    cldm_mocktail(name)

#else
#define cldm_mock_procedure_nullary(rettype, name)          cldm_mock_gen_procctx(name, void)
#define cldm_mock_function_nullary(rettype, name)           cldm_mock_gen_funcctx(rettype, name, void)
#define cldm_mock_procedure_variadic(rettype, name, ...)    cldm_mock_gen_procctx(name, __VA_ARGS__)
#define cldm_mock_function_variadic(rettype, name, ...)     cldm_mock_gen_funcctx(rettype, name, __VA_ARGS__)
#endif

#define cldm_mock_varchk_resolve00(...) cldm_mock_function_variadic(__VA_ARGS__)
#define cldm_mock_varchk_resolve10(...) cldm_mock_procedure_variadic(__VA_ARGS__)
#define cldm_mock_varchk_resolve01(...) cldm_mock_function_nullary(cldm_first(__VA_ARGS__), cldm_second(__VA_ARGS__))
#define cldm_mock_varchk_resolve11(...) cldm_mock_procedure_nullary(cldm_first(__VA_ARGS__), cldm_second(__VA_ARGS__))

#define cldm_mock_function_variadic_chk(rettype, name, ...)     \
    cldm_cat_expand(cldm_cat_expand(cldm_mock_varchk_resolve,cldm_token_void(rettype)),cldm_token_void(cldm_first(__VA_ARGS__)))(rettype, name, __VA_ARGS__)

#define cldm_mock_resolve00() function_variadic_chk
#define cldm_mock_resolve01() function_variadic_chk
#define cldm_mock_resolve10() function_nullary
#define cldm_mock_resolve11() procedure_nullary

#define CLDM_MOCK_FUNCTION(rettype, ...)    \
    cldm_cat_expand(cldm_mock_,cldm_cat_expand(cldm_cat_expand(cldm_mock_resolve, cldm_token_1(cldm_count(__VA_ARGS__))), cldm_token_void(rettype))())(rettype, __VA_ARGS__)

#define cldm_mock_lower_bound(tid)          \
    (tid * !cldm_mock_global_context)
#define cldm_mock_upper_bound(tid)          \
    (!cldm_mock_global_context * (tid + 1) + cldm_mock_global_context * cldm_jobs)

#define cldm_set_mockop(memb, val, op)                                                                                          \
    memb = val;                                                                                                                 \
    *(enum cldm_mockop *)(((unsigned char *)cldm_mockinfos[cldm_cat_expand(cldm_wc_tid,__LINE__)].data->addr  +                 \
                                            cldm_mockinfos[cldm_cat_expand(cldm_wc_tid,__LINE__)].data->opmode_offset)) = op

#define cldm_assign2(lhs, rhs)                                                                                                  \
    cldm_set_mockop(assign, ((struct cldm_assign_opdata){ &lhs, &rhs, sizeof(lhs) }), cldm_mockop_assign);                      \
    if(sizeof(lhs) < sizeof(rhs)) {                                                                                             \
        cldm_warn("Assigning block of %zu bytes to block of %zu bytes may alter its value", sizeof(rhs), sizeof(lhs));          \
    }

#define cldm_assign3(lhs, rhs, type) cldm_assign2(lhs, (type){ rhs })

#define CLDM_WHEN_CALLED(name)                                                                                                  \
    do {                                                                                                                        \
        extern cldm_aligned_mock_ ## name ## _ctx cldm_mock_ ## name[CLDM_MAX_THREADS];                                         \
        unsigned cldm_cat_expand(cldm_wc_ttid, __LINE__) = cldm_thread_id();                                                    \
        for(unsigned cldm_cat_expand(cldm_wc_tid,__LINE__) = cldm_mock_lower_bound(cldm_cat_expand(cldm_wc_ttid,__LINE__)),     \
                cldm_cat_expand(cldm_wc_tidmax,__LINE__) = cldm_mock_upper_bound(cldm_cat_expand(cldm_wc_ttid,__LINE__));       \
            cldm_cat_expand(cldm_wc_tid,__LINE__) < cldm_cat_expand(cldm_wc_tidmax,__LINE__);                                   \
            ++cldm_cat_expand(cldm_wc_tid,__LINE__))                                                                            \
        {                                                                                                                       \
            cldm_mockinfos[cldm_cat_expand(cldm_wc_tid,__LINE__)].data =                                                        \
                &cldm_mock_ ## name[cldm_cat_expand(cldm_wc_tid,__LINE__)].data.info;                                           \
            cldm_mock_ ## name[cldm_cat_expand(cldm_wc_tid,__LINE__)].data.opdata.un_act

#define cldm_should(invocations, ...)                                                                                                   \
            __VA_ARGS__;                                                                                                                \
            cldm_rtassert(invocations >= 0 || invocations == CLDM_MOCK_INFINITE);                                                       \
            *(int *)((unsigned char *)cldm_mockinfos[cldm_cat_expand(cldm_wc_tid,__LINE__)].data->addr +                                \
                                      cldm_mockinfos[cldm_cat_expand(cldm_wc_tid,__LINE__)].data->invocations_offset) = invocations;    \
        }                                                                                                                               \
    } while (0)

#define CLDM_SHOULD_ONCE(...)           cldm_should(1, __VA_ARGS__)
#define CLDM_SHOULD_REPEATEDLY(...)     cldm_should(CLDM_MOCK_INFINITE, __VA_ARGS__)
#define CLDM_SHOULD_N_TIMES(n, ...)     cldm_should(n, __VA_ARGS__)
#define CLDM_SHOULD_INVOKE_DEFAULT()    cldm_should(0, cldm_set_mockop(invoke, 0, cldm_mockop_invoke))

#define CLDM_INVOKE(func)               cldm_set_mockop(invoke, func, cldm_mockop_invoke)
#define CLDM_RETURN(val)                cldm_set_mockop(retval, val, cldm_mockop_return)
#define CLDM_RETURN_VOID()              cldm_set_mockop(invoke, 0, cldm_mockop_return)
#define CLDM_RETURN_PARAM(idx)          cldm_set_mockop(paramidx, idx, cldm_mockop_return_param)
#define CLDM_RETURN_POINTEE(idx)        cldm_set_mockop(paramidx, idx, cldm_mockop_return_pointee)
#define CLDM_INCREMENT_COUNTER(init)    cldm_set_mockop(counter, init, cldm_mockop_increment)
#define CLDM_ASSIGN(...)                cldm_overload(cldm_assign,__VA_ARGS__)
#define CLDM_ASSIGN_PARAM(index, lhs)   cldm_set_mockop(assign_param, ((struct cldm_assign_param_opdata){ &lhs, index, sizeof(lhs) }), cldm_mockop_assign_param)

#ifndef CLDM_PREFIX_ONLY
#define MOCK_FUNCTION(...)      CLDM_MOCK_FUNCTION(__VA_ARGS__)
#define WHEN_CALLED(...)        CLDM_WHEN_CALLED(__VA_ARGS__)
#define SHOULD_ONCE(...)        CLDM_SHOULD_ONCE(__VA_ARGS__)
#define SHOULD_REPEATEDLY(...)  CLDM_SHOULD_REPEATEDLY(__VA_ARGS__)
#define SHOULD_N_TIMES(...)     CLDM_SHOULD_N_TIMES(__VA_ARGS__)
#define SHOULD_INVOKE_DEFAULT() CLDM_SHOULD_INVOKE_DEFAULT()
#define INVOKE(...)             CLDM_INVOKE(__VA_ARGS__)
#define RETURN(...)             CLDM_RETURN(__VA_ARGS__)
#define RETURN_VOID()           CLDM_RETURN_VOID()
#define RETURN_PARAM(...)       CLDM_RETURN_PARAM(__VA_ARGS__)
#define RETURN_POINTEE(...)     CLDM_RETURN_POINTEE(__VA_ARGS__)
#define INCREMENT_COUNTER(...)  CLDM_INCREMENT_COUNTER(__VA_ARGS__)
#define ASSIGN(...)             CLDM_ASSIGN(__VA_ARGS__)
#define ASSIGN_PARAM(...)       CLDM_ASSIGN_PARAM(__VA_ARGS__)
#endif

#include "cldmgen.h"

#endif /* CLDM_MOCK_H */
