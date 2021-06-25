#ifndef CLDM_TOKEN_H
#define CLDM_TOKEN_H

#include "cldm_macro.h"

#define cldm_generic_probe() x,1
#define cldm_probe_void_void() cldm_generic_probe()
#define cldm_probe_1_1() cldm_generic_probe()
#define cldm_probe_0_0() cldm_generic_probe()
#define cldm_probe_noreturn_noreturn() cldm_generic_probe()
#define cldm_probe__Noreturn__Noreturn() cldm_generic_probe()

#define cldm_match_token(token, x) cldm_chk(cldm_cat_expand(cldm_cat_expand(cldm_cat_expand(cldm_probe_,token),_),x)())

#define cldm_token_void(x) cldm_match_token(void, x)
#define cldm_token_0(x) cldm_match_token(0,x)
#define cldm_token_1(x) cldm_match_token(1,x)
#define cldm_token_noreturn(x) (cldm_match_token(_Noreturn,x) | cldm_match_token(noreturn,x))

#define cldm_repeat_token_recurse1(...)
#define cldm_defer_repeat_token_recursion() cldm_repeat_token_recurse
#define cldm_repeat_token_recurse0(n, token)                    \
    ,cldm_block(cldm_defer_repeat_token_recursion)()(cldm_dec(n), token)

#define cldm_repeat_token_recurse(n, token)                     \
    token                                                       \
    cldm_cat_expand(cldm_repeat_token_recurse, cldm_token_0(n))(n, token)

#define cldm_repeat_token(token, n)                             \
    cldm_cat_expand(cldm_expand, n)(cldm_repeat_token_recurse(cldm_dec(n), token))

#endif /* CLDM_TOKEN_H */
