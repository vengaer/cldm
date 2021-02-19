#define LMC_GMOCK_COMPAT
#include "src/lmc.h"

#include <stdio.h>

MOCK_FUNCTION_VOID(foo, int, char const*);
MOCK_FUNCTION_VOID(foobar);
MOCK_FUNCTION_VOID0(barbaz);
MOCK_FUNCTION_VOID0(barbaz1, void);

MOCK_FUNCTION(int, bar, int, char const*);
MOCK_FUNCTION(int, baz);
MOCK_FUNCTION0(char, foobaz);
MOCK_FUNCTION0(char, foobaz1, void);

void mfoo(int i, char const* c) {
    printf("Params: %d %s\n", i, c);
}

int mbar(int i, char const *c) {
    printf("Params: %d %s\n", i, c);
    return i;
}

int mbaz(void) {
    puts("mbaz");
    return 0;
}

void mfoobar(void) {
    puts("mfoobar");
}

char mfoobaz(void) {
    puts("mfoobaz");
    return 'c';
}

int main(void) {
    EXPECT_CALL(foo).WillRepeatedly(Invoke(mfoo));
    EXPECT_CALL(bar).WillRepeatedly(Invoke(mbar));

    foo(5, "string");
    foo(5, "string");

    bar(5, "string");

    EXPECT_CALL(bar).WillRepeatedly(Return(5));
    printf("%d\n", bar(2, "str"));
    EXPECT_CALL(bar).WillRepeatedly(Return(8));
    printf("%d\n", bar(2, "str"));

    EXPECT_CALL(bar).WillRepeatedly(IncrementCounter(0));
    printf("%d\n", bar(2, "str"));
    printf("%d\n", bar(2, "str"));
    printf("%d\n", bar(2, "str"));
    printf("%d\n", bar(2, "str"));

    EXPECT_CALL(baz).WillRepeatedly(IncrementCounter(2));
    printf("%d\n", baz());
    printf("%d\n", baz());
    printf("%d\n", baz());
    printf("%d\n", baz());

    EXPECT_CALL(baz).WillRepeatedly(Invoke(mbaz));
    baz();

    EXPECT_CALL(foobar).WillRepeatedly(Invoke(mfoobar));
    foobar();

    EXPECT_CALL(foobaz).WillRepeatedly(Invoke(mfoobaz));
    foobaz();
    foobaz();

#if 0
    int j = 12;
    EXPECT_CALL(foobaz).WillOnce(Assign(j, 18));
    foobaz();
    printf("%d\n", j);
#endif
}
