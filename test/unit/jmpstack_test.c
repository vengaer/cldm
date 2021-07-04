#include <cldm/cldm.h>
#include <cldm/cldm_jmpstack.h>
#include <cldm/cldm_macro.h>

#include <setjmp.h>
#include <string.h>

TEST(cldm_jmpstack_push) {
    struct cldm_jmpstack stack;
    jmp_buf buf0;
    jmp_buf buf1;

    stack = cldm_jmpstack_init();

    setjmp(buf0);
    setjmp(buf1);

    cldm_jmpstack_push(&stack, &buf0);
    ASSERT_EQ(cldm_jmpstack_top(&stack), &buf0);

    cldm_jmpstack_push(&stack, &buf1);
    ASSERT_EQ(cldm_jmpstack_top(&stack), &buf1);

    cldm_jmpstack_free(&stack);
}

TEST(cldm_jmpstack_pop) {
    struct cldm_jmpstack stack;
    jmp_buf buf0;
    jmp_buf buf1;

    stack = cldm_jmpstack_init();

    setjmp(buf0);
    setjmp(buf1);

    cldm_jmpstack_push(&stack, &buf0);
    cldm_jmpstack_push(&stack, &buf1);
    ASSERT_EQ(cldm_jmpstack_top(&stack), &buf1);
    cldm_jmpstack_pop(&stack);
    ASSERT_EQ(cldm_jmpstack_top(&stack), &buf0);

    cldm_jmpstack_free(&stack);
}

TEST(cldm_jmpstack_size) {
    struct cldm_jmpstack stack;
    jmp_buf buf0;
    jmp_buf buf1;

    stack = cldm_jmpstack_init();

    setjmp(buf0);
    setjmp(buf1);

    ASSERT_EQ(cldm_jmpstack_size(&stack), 0);
    cldm_jmpstack_push(&stack, &buf0);
    ASSERT_EQ(cldm_jmpstack_size(&stack), 1);
    cldm_jmpstack_push(&stack, &buf1);
    ASSERT_EQ(cldm_jmpstack_size(&stack), 2);
    cldm_jmpstack_pop(&stack);
    ASSERT_EQ(cldm_jmpstack_size(&stack), 1);
    cldm_jmpstack_pop(&stack);
    ASSERT_EQ(cldm_jmpstack_size(&stack), 0);

    cldm_jmpstack_free(&stack);
}

TEST(cldm_jmpstack_resize) {
    struct cldm_jmpstack stack;
    jmp_buf bufs[cldm_arrsize(stack.s_un.stat) << (1u + 1)];

    stack = cldm_jmpstack_init();
    for(unsigned i = 0; i < cldm_arrsize(bufs); i++) {
        cldm_jmpstack_push(&stack, &bufs[i]);
        ASSERT_EQ(cldm_jmpstack_top(&stack), &bufs[i]);
        ASSERT_EQ(cldm_jmpstack_size(&stack), i + 1u);
    }
    ASSERT_GT(cldm_jmpstack_capacity(&stack), cldm_arrsize(stack.s_un.stat));
    cldm_jmpstack_free(&stack);
}
