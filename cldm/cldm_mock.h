#ifndef CLDM_MOCK_H
#define CLDM_MOCK_H

#include "cldm_cache.h"
#include "cldm_dl.h"
#include "cldm_macro.h"
#include "cldm_rtassert.h"
#include "cldm_thread.h"

#include "cldm_params.h"
#include "cldm_arglist.h"
#include "cldm_argframe.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef cldm_cachealign(struct cldm_mockinfo *) cldm_aligned_mockinfo;
typedef cldm_cachealign(bool) cldm_aligned_bool;

extern cldm_aligned_mockinfo mockinfos[CLDM_MAX_THREADS];
extern cldm_aligned_bool cldm_mock_force_disable[CLDM_MAX_THREADS];

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

#define cldm_mock_function2(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function3(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function4(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function5(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function6(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function7(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function8(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function9(...)   cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function10(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function11(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function12(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function13(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function14(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function15(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function16(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function17(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function18(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function19(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function20(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function21(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function22(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function23(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function24(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function25(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function26(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function27(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function28(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function29(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function30(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function31(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function32(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function33(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function34(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function35(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function36(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function37(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function38(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function39(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function40(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function41(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function42(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function43(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function44(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function45(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function46(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function47(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function48(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function49(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function50(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function51(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function52(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function53(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function54(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function55(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function56(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function57(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function58(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function59(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function60(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function61(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function62(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function63(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function64(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function65(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function66(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function67(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function68(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function69(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function70(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function71(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function72(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function73(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function74(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function75(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function76(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function77(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function78(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function79(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function80(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function81(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function82(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function83(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function84(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function85(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function86(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function87(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function88(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function89(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function90(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function91(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function92(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function93(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function94(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function95(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function96(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function97(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function98(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function99(...)  cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function100(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function101(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function102(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function103(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function104(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function105(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function106(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function107(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function108(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function109(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function110(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function111(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function112(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function113(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function114(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function115(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function116(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function117(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function118(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function119(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function120(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function121(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function122(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function123(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function124(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function125(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function126(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function127(...) cldm_mock_function(__VA_ARGS__)
#define cldm_mock_function128(...) cldm_mock_function(__VA_ARGS__)

#define cldm_mock_function0_1(rettype, name) CLDM_MOCK_FUNCTION(rettype, name)
#define cldm_mock_function0_2(rettype, name, _) CLDM_MOCK_FUNCTION(rettype, name)

#define cldm_mock_function_void0_1(name) CLDM_MOCK_FUNCTION_VOID(name)
#define cldm_mock_function_void0_2(name, _) CLDM_MOCK_FUNCTION_VOID(name)

#define cldm_will(invocations, ...)                                                                                                     \
        __VA_ARGS__;                                                                                                                    \
        *(int *)((unsigned char *)mockinfos[cldm_cat_expand(cldm_exp_tid,__LINE__)].data->addr +                                        \
                                  mockinfos[cldm_cat_expand(cldm_exp_tid,__LINE__)].data->invocations_offset) = invocations;            \
        cldm_rtassert((*(int *)((unsigned char *)mockinfos[cldm_cat_expand(cldm_exp_tid,__LINE__)].data->addr +                         \
                                                 mockinfos[cldm_cat_expand(cldm_exp_tid,__LINE__)].data->invocations_offset)) > -2);    \
    } while (0)

#define cldm_set_opmode(mode)                                                                   \
    enum cldm_opmode *cldm_cat_expand(cldm_opdata, __LINE__) =                                  \
        (enum cldm_opmode *)((unsigned char*)mockinfos[cldm_thread_id()].data->addr +           \
                                             mockinfos[cldm_thread_id()].data->opmode_offset);  \
    *cldm_cat_expand(cldm_opdata, __LINE__) = mode

#define cldm_setop(field, value, mode)      \
    opdata.field = value;                   \
    cldm_set_opmode(mode)

#define cldm_assign2(lhs, rhs)              \
    cldm_setop(act.assign_ctx, ((struct cldm_assign_ctx){ &lhs, &rhs, sizeof(lhs) }), cldm_op_assign)

#define cldm_assign3(lhs, rhs, type)        \
    cldm_assign2(lhs, (type){ rhs })

struct cldm_mockinfo {
    void *addr;
    unsigned invocations_offset;
    unsigned opmode_offset;
};

struct cldm_assign_ctx {
    void *lhs;
    void const *rhs;
    unsigned argsize;
};

struct cldm_assignarg_ctx {
    void *addr;
    unsigned argidx;
    unsigned argsize;
};

enum cldm_opmode {
    cldm_op_invoke,
    cldm_op_return,
    cldm_op_increment,
    cldm_op_retarg,
    cldm_op_retpointee,
    cldm_op_assign,
    cldm_op_assignarg
};

#define cldm_generate_mock_ctx(utype, rettype, name, ...)                                           \
    struct cldm_mock_ ## name ## _ctx {                                                             \
        struct cldm_mockinfo info;                                                                  \
        int invocations;                                                                            \
        struct cldm_mock_ ## name ## _opdata{                                                       \
            enum cldm_opmode mode;                                                                  \
            union {                                                                                 \
                rettype(*invoke)(__VA_ARGS__);                                                      \
                utype retval;                                                                       \
                utype counter;                                                                      \
                unsigned argindex;                                                                  \
                struct cldm_assign_ctx assign_ctx;                                                  \
                struct cldm_assignarg_ctx assignarg_ctx;                                            \
            } act;                                                                                  \
        } opdata;                                                                                   \
    };                                                                                              \
    typedef cldm_cachealign(struct cldm_mock_ ## name ## _ctx) cldm_aligned_mock_ ## name ## _ctx

#define cldm_populate_mockctx_single(idx, name)                                                     \
    {                                                                                               \
        .info = {                                                                                   \
            .addr = &cldm_mock_ ## name[idx].data,                                                  \
            .invocations_offset = cldm_offset(struct cldm_mock_ ## name ## _ctx, invocations),      \
            .opmode_offset = cldm_offset(struct cldm_mock_ ## name ## _ctx, opdata) +               \
                             cldm_offset(struct cldm_mock_ ## name ## _opdata, mode)                \
        },                                                                                          \
        .invocations = 0,                                                                           \
        .opdata = {                                                                                 \
            .mode = cldm_op_invoke,                                                                 \
            .act.invoke = 0                                                                         \
        }                                                                                           \
    }

#define cldm_populate_mockctx(name)                     \
    [0].data  = cldm_populate_mockctx_single(0, name),  \
    [1].data  = cldm_populate_mockctx_single(1, name),  \
    [2].data  = cldm_populate_mockctx_single(2, name),  \
    [3].data  = cldm_populate_mockctx_single(3, name),  \
    [4].data  = cldm_populate_mockctx_single(4, name),  \
    [5].data  = cldm_populate_mockctx_single(5, name),  \
    [6].data  = cldm_populate_mockctx_single(6, name),  \
    [7].data  = cldm_populate_mockctx_single(7, name),  \
    [8].data  = cldm_populate_mockctx_single(8, name),  \
    [9].data  = cldm_populate_mockctx_single(9, name),  \
    [10].data = cldm_populate_mockctx_single(10, name), \
    [11].data = cldm_populate_mockctx_single(11, name), \
    [12].data = cldm_populate_mockctx_single(12, name), \
    [13].data = cldm_populate_mockctx_single(13, name), \
    [14].data = cldm_populate_mockctx_single(14, name), \
    [15].data = cldm_populate_mockctx_single(15, name), \
    [16].data = cldm_populate_mockctx_single(16, name), \
    [17].data = cldm_populate_mockctx_single(17, name), \
    [18].data = cldm_populate_mockctx_single(18, name), \
    [19].data = cldm_populate_mockctx_single(19, name), \
    [20].data = cldm_populate_mockctx_single(20, name), \
    [21].data = cldm_populate_mockctx_single(21, name), \
    [22].data = cldm_populate_mockctx_single(22, name), \
    [23].data = cldm_populate_mockctx_single(23, name), \
    [24].data = cldm_populate_mockctx_single(24, name), \
    [25].data = cldm_populate_mockctx_single(25, name), \
    [26].data = cldm_populate_mockctx_single(26, name), \
    [27].data = cldm_populate_mockctx_single(27, name), \
    [28].data = cldm_populate_mockctx_single(28, name), \
    [29].data = cldm_populate_mockctx_single(29, name), \
    [30].data = cldm_populate_mockctx_single(30, name), \
    [31].data = cldm_populate_mockctx_single(31, name)

#ifdef CLDM_GENERATE_SYMBOLS
#define cldm_mock_function(rvinit, call_prefix, retstatement, utype, rettype, name, ...)                                            \
    cldm_generate_mock_ctx(utype, rettype, name, __VA_ARGS__);                                                                      \
    cldm_aligned_mock_ ## name ## _ctx cldm_mock_ ## name[CLDM_MAX_THREADS] = {                                                     \
        cldm_populate_mockctx(name)                                                                                                 \
    };                                                                                                                              \
    rettype name(cldm_genparams(__VA_ARGS__)) {                                                                                     \
        rvinit;                                                                                                                     \
        unsigned thread_id = cldm_thread_id();                                                                                      \
        void const *argaddrs[cldm_count(__VA_ARGS__)];                                                                              \
        cldm_argframe_populate(argaddrs, __VA_ARGS__);                                                                              \
        if(cldm_mock_enabled() && cldm_mock_ ## name[thread_id].data.invocations) {                                                 \
            if(cldm_mock_ ## name[thread_id].data.invocations != -1) {                                                              \
                --cldm_mock_ ## name[thread_id].data.invocations;                                                                   \
            }                                                                                                                       \
            switch(cldm_mock_ ## name[thread_id].data.opdata.mode) {                                                                \
                case cldm_op_invoke:                                                                                                \
                    call_prefix cldm_mock_ ## name[thread_id].data.opdata.act.invoke(cldm_arglist(cldm_count(__VA_ARGS__)));        \
                    break;                                                                                                          \
                case cldm_op_return:                                                                                                \
                    call_prefix cldm_mock_ ## name[thread_id].data.opdata.act.retval;                                               \
                    break;                                                                                                          \
                case cldm_op_increment:                                                                                             \
                    call_prefix ++cldm_mock_ ## name[thread_id].data.opdata.act.counter;                                            \
                    break;                                                                                                          \
                case cldm_op_retarg:                                                                                                \
                    cldm_rtassert(cldm_mock_ ## name[thread_id].data.opdata.act.argindex < cldm_arrsize(argaddrs),                  \
                                 "Attempt to access parameter %u in function taking only %zu",                                      \
                                 cldm_mock_ ## name[thread_id].data.opdata.act.argindex + 1, cldm_arrsize(argaddrs));               \
                    call_prefix *(utype *)argaddrs[cldm_mock_ ## name[thread_id].data.opdata.act.argindex];                         \
                    break;                                                                                                          \
                case cldm_op_retpointee:                                                                                            \
                    cldm_rtassert(cldm_mock_ ## name[thread_id].data.opdata.act.argindex < cldm_arrsize(argaddrs),                  \
                                 "Attempt to access parameter %u in function taking only %zu",                                      \
                                 cldm_mock_ ## name[thread_id].data.opdata.act.argindex + 1, cldm_arrsize(argaddrs));               \
                    call_prefix **(utype **)argaddrs[cldm_mock_ ## name[thread_id].data.opdata.act.argindex];                       \
                    break;                                                                                                          \
                case cldm_op_assign:                                                                                                \
                    memcpy(cldm_mock_ ## name[thread_id].data.opdata.act.assign_ctx.lhs,                                            \
                           cldm_mock_ ## name[thread_id].data.opdata.act.assign_ctx.rhs,                                            \
                           cldm_mock_ ## name[thread_id].data.opdata.act.assign_ctx.argsize);                                       \
                    break;                                                                                                          \
                case cldm_op_assignarg:                                                                                             \
                    cldm_rtassert(cldm_mock_ ## name[thread_id].data.opdata.act.assignarg_ctx.argidx < cldm_arrsize(argaddrs),      \
                                 "Attempt to access parameter %u in function taking only %zu",                                      \
                                 cldm_mock_ ## name[thread_id].data.opdata.act.assignarg_ctx.argidx + 1, cldm_arrsize(argaddrs));   \
                    memcpy(cldm_mock_ ## name[thread_id].data.opdata.act.assignarg_ctx.addr,                                        \
                           argaddrs[cldm_mock_ ## name[thread_id].data.opdata.act.assignarg_ctx.argidx],                            \
                           cldm_mock_ ## name[thread_id].data.opdata.act.assignarg_ctx.argsize);                                    \
                    break;                                                                                                          \
                default:                                                                                                            \
                    cldm_rtassert(0, "Invalid opmode %d", cldm_mock_ ## name[thread_id].data.opdata.mode);                          \
            }                                                                                                                       \
            retstatement;                                                                                                           \
        }                                                                                                                           \
        rettype(* cldm_handle_ ## name)(__VA_ARGS__);                                                                               \
        cldm_mock_disable() {                                                                                                       \
            *(void **) (& cldm_handle_ ## name) = cldm_dlsym_next(#name);                                                           \
            cldm_rtassert(cldm_handle_ ## name);                                                                                    \
            call_prefix cldm_handle_ ## name(cldm_arglist(cldm_count(__VA_ARGS__)));                                                \
        }                                                                                                                           \
        retstatement;                                                                                                               \
    }                                                                                                                               \
    void cldm_trailing_ ## name (void)

#define cldm_mock_function1(rvinit, call_prefix, retstatement, utype, rettype, name)                        \
    cldm_generate_mock_ctx(utype, rettype, name, void);                                                     \
    cldm_aligned_mock_ ## name ## _ctx cldm_mock_ ## name[CLDM_MAX_THREADS] = {                             \
        cldm_populate_mockctx(name)                                                                         \
    };                                                                                                      \
    rettype name(void) {                                                                                    \
        rvinit;                                                                                             \
        unsigned thread_id = cldm_thread_id();                                                              \
        if(cldm_mock_enabled() && cldm_mock_ ## name[thread_id].data.invocations) {                         \
            if(cldm_mock_ ## name[thread_id].data.invocations != -1) {                                      \
                --cldm_mock_ ## name[thread_id].data.invocations;                                           \
            }                                                                                               \
            switch(cldm_mock_ ## name[thread_id].data.opdata.mode) {                                        \
                    case cldm_op_invoke:                                                                    \
                    call_prefix cldm_mock_ ## name[thread_id].data.opdata.act.invoke();                     \
                    break;                                                                                  \
                case cldm_op_return:                                                                        \
                    call_prefix cldm_mock_ ## name[thread_id].data.opdata.act.retval;                       \
                    break;                                                                                  \
                case cldm_op_increment:                                                                     \
                    call_prefix ++cldm_mock_ ## name[thread_id].data.opdata.act.counter;                    \
                    break;                                                                                  \
                case cldm_op_assign:                                                                        \
                    memcpy(cldm_mock_ ## name[thread_id].data.opdata.act.assign_ctx.lhs,                    \
                           cldm_mock_ ## name[thread_id].data.opdata.act.assign_ctx.rhs,                    \
                           cldm_mock_ ## name[thread_id].data.opdata.act.assign_ctx.argsize);               \
                    break;                                                                                  \
                default:                                                                                    \
                    cldm_rtassert(0, "Invalid opmode %d", cldm_mock_ ## name[thread_id].data.opdata.mode);  \
            }                                                                                               \
            retstatement;                                                                                   \
        }                                                                                                   \
        rettype(* cldm_handle_ ## name)(void);                                                              \
        cldm_mock_disable() {                                                                               \
            *(void **) (& cldm_handle_ ## name) = cldm_dlsym_next(#name);                                   \
            cldm_rtassert(cldm_handle_ ## name);                                                            \
            call_prefix cldm_handle_ ## name ();                                                            \
        }                                                                                                   \
        retstatement;                                                                                       \
    }                                                                                                       \
    void cldm_trailing_ ## name (void)
#else

#define cldm_mock_function(rvinit, call_prefix, retstatement, utype, rettype, name, ...)    \
    cldm_generate_mock_ctx(utype, rettype, name, __VA_ARGS__)

#define cldm_mock_function1(rvinit, call_prefix, retstatement, utype, rettype, name)        \
    cldm_generate_mock_ctx(utype, rettype, name, void)
#endif

#define CLDM_MOCK_FUNCTION(rettype, ...)                                                                                \
    cldm_cat_expand(cldm_mock_function, cldm_count(__VA_ARGS__))(rettype cldm_cat_expand(cldm_rv_,__LINE__);            \
                                                                 memset(&cldm_cat_expand(cldm_rv_,__LINE__), 0,         \
                                                                        sizeof(cldm_cat_expand(cldm_rv_,__LINE__))),    \
                                                                 cldm_cat_expand(cldm_rv_, __LINE__) =,                 \
                                                                 return cldm_cat_expand(cldm_rv_, __LINE__),            \
                                                                 rettype, rettype, __VA_ARGS__)

#define CLDM_MOCK_FUNCTION0(rettype, ...)   \
    cldm_cat_expand(cldm_mock_function0_, cldm_count(__VA_ARGS__))(rettype, __VA_ARGS__)


#define CLDM_MOCK_FUNCTION_VOID(...)    \
    cldm_cat_expand(cldm_mock_function, cldm_count(__VA_ARGS__))((void)0, (void), return, int, void, __VA_ARGS__)

#define CLDM_MOCK_FUNCTION_VOID0(...)   \
    cldm_cat_expand(cldm_mock_function_void0_, cldm_count(__VA_ARGS__))(__VA_ARGS__)

#define CLDM_EXPECT_CALL(name)                                                          \
    do {                                                                                \
        extern cldm_aligned_mock_ ## name ## _ctx cldm_mock_ ## name[CLDM_MAX_THREADS]; \
        unsigned cldm_cat_expand(cldm_exp_tid,__LINE__) = cldm_thread_id();             \
        mockinfos[cldm_cat_expand(cldm_exp_tid,__LINE__)].data =                        \
            &cldm_mock_ ## name[cldm_cat_expand(cldm_exp_tid,__LINE__)].data.info;      \
        cldm_mock_ ## name[cldm_cat_expand(cldm_exp_tid,__LINE__)].data

#define CLDM_WILL_ONCE(...) cldm_will(1, __VA_ARGS__)
#define CLDM_WILL_REPEATEDLY(...) cldm_will(-1, __VA_ARGS__)
#define CLDM_WILL_N_TIMES(n, ...) cldm_will(n, __VA_ARGS__)
#define CLDM_WILL_INVOKE_DEFAULT() cldm_will(0, opdata.act.invoke = cldm_op_invoke)

#define CLDM_INVOKE(func)            cldm_setop(act.invoke, func, cldm_op_invoke)
#define CLDM_RETURN(value)           cldm_setop(act.retval, value, cldm_op_return)
#define CLDM_RETURN_ARG(index)       cldm_setop(act.argindex, index, cldm_op_retarg)
#define CLDM_RETURN_POINTEE(index)   cldm_setop(act.argindex, index, cldm_op_retpointee)
#define CLDM_INCREMENT_COUNTER(init) cldm_setop(act.counter, init, cldm_op_increment)
#define CLDM_ASSIGN(...)             cldm_overload(cldm_assign,__VA_ARGS__)
#define CLDM_ASSIGN_ARG(index, lhs)  cldm_setop(act.assignarg_ctx, ((struct cldm_assignarg_ctx){ &lhs, index, sizeof(lhs) }), cldm_op_assignarg)


#ifndef CLDM_PREFIX_ONLY
#define MOCK_FUNCTION(...)       CLDM_MOCK_FUNCTION(__VA_ARGS__)
#define MOCK_FUNCTION0(...)      CLDM_MOCK_FUNCTION0(__VA_ARGS__)
#define MOCK_FUNCTION_VOID(...)  CLDM_MOCK_FUNCTION_VOID(__VA_ARGS__)
#define MOCK_FUNCTION_VOID0(...) CLDM_MOCK_FUNCTION_VOID0(__VA_ARGS__)
#define EXPECT_CALL(...)         CLDM_EXPECT_CALL(__VA_ARGS__)
#define WILL_ONCE(...)           CLDM_WILL_ONCE(__VA_ARGS__)
#define WILL_REPEATEDLY(...)     CLDM_WILL_REPEATEDLY(__VA_ARGS__)
#define WILL_N_TIMES(...)        CLDM_WILL_N_TIMES(__VA_ARGS__)
#define WILL_INVOKE_DEFAULT()    CLDM_WILL_INVOKE_DEFAULT()
#define INVOKE(...)              CLDM_INVOKE(__VA_ARGS__)
#define RETURN(...)              CLDM_RETURN(__VA_ARGS__)
#define RETURN_ARG(...)          CLDM_RETURN_ARG(__VA_ARGS__)
#define RETURN_POINTEE(...)      CLDM_RETURN_POINTEE(__VA_ARGS__)
#define INCREMENT_COUNTER(...)   CLDM_INCREMENT_COUNTER(__VA_ARGS__)
#define ASSIGN(...)              CLDM_ASSIGN(__VA_ARGS__)
#define ASSIGN_ARG(...)          CLDM_ASSIGN_ARG(__VA_ARGS__)
#endif

#ifdef CLDM_GMOCK_COMPAT
#define WillOnce(...)         CLDM_WILL_ONCE(__VA_ARGS__)
#define WillRepeatedly(...)   CLDM_WILL_REPEATEDLY(__VA_ARGS__)
#define WillNTimes(...)       CLDM_WILL_N_TIMES(__VA_ARGS__)
#define WillInvokeDefault()   CLDM_WILL_INVOKE_DEFAULT()
#define Invoke(...)           CLDM_INVOKE(__VA_ARGS__)
#define Return(...)           CLDM_RETURN(__VA_ARGS__)
#define ReturnArg(...)        CLDM_RETURN_ARG(__VA_ARGS__)
#define ReturnPointee(...)    CLDM_RETURN_POINTEE(__VA_ARGS__)
#define IncrementCounter(...) CLDM_INCREMENT_COUNTER(__VA_ARGS__)
#define Assign(...)           CLDM_ASSIGN(__VA_ARGS__)
#define AssignArg(...)        CLDM_ASSIGN_ARG(__VA_ARGS__)
#endif

#include "cldmgen.h"

#endif /* CLDM_MOCK_H */
