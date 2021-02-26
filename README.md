# cldm - Linker-Based Function Mocking

Function mocking framework in pure C. Requires a C99-compatible compiler. The interface is inspired by [gmock](https://github.com/google/googletest).

[[_TOC_]]

## Example Usage

Using cldm, the behavior of any function, including the ones provided by the standard, can be overridden. The one requirement is that the symbol to be mocked is loaded dynamically (i.e. from a shared library). The following shows an example of how the output of `atoi` ([man page](https://man.archlinux.org/man/atoi.3)) may be manipulated at runtime.

```c
#include "cldm.h"

#include <assert.h>
#include <stdlib.h>

int main(void) {
    EXPECT_CALL(atoi).WILL_ONCE(RETURN(8));
    assert(atoi("2") == 8); /* Passes due to the mock */
    assert(atoi("2") == 2); /* Default behavior restored */

    EXPECT_CALL(atoi).WILL_REPEATEDLY(RETURN(7));
    assert(atoi("2") == 7); /* Passes due to the mock */
    assert(atoi("6") == 7); /* Ditto */

    return 0;
}

```

## Using cldm

Building cldm requires some configuration. Specifically, the signatures of the mocks to be generated need to be known. Once these have been specified, the build system handles the rest. The procedure can be summarized as follows:

1. Define functions to be mocked
2. Build cldm via make
3. Preload the generated shared library
4. Include the `cldm.h` header

The entire procedure is described in detail below.

#### Defining the Mocks

In order for cldm to know the signatures of the functions to mock, these must be specified by the user. To do this, create a file called `mockups.h` in the `cldm` directory and use the `MOCK_FUNCTION` and `MOCK_FUNCTION_VOID` macro families.

The "signature" of the `MOCK_FUNCTION` macro is as follows:
```c
MOCK_FUNCTION(return_type, function_name [,param_type[,param_type[,...]]]).
```
At most 127 parameters are supported. The `MOCK_FUNCTION_VOID` macro, used for functions returning `void`, is defined similarly, only omitting the return type, i.e. 
```c
MOCK_FUNCTION_VOID(function_name [,param_type[,param_type[,...]]]).
``` 
There are a few considerations that have to made here, [see below](#mocking-gotchas).

The following would generate code necessary to mock the standard `atoi`, `strcpy` and `free` functions.

`cldm/mockups.h`
```c
#include "cldm.h"

MOCK_FUNCTION(int, atoi, char const *);
MOCK_FUNCTION(char *, strcpy, char *, char const *);

/* Functions returning void must be declared using the MOCK_FUNCTION_VOID macro family */
MOCK_FUNCTION_VOID(free, void *);

```

The path to the function describing the mocks may be modified by passing `MOCKUPS=<new path>` to `make` when building.

##### Mocking Gotchas

At the time of writing, both functions returning `void` and ones taking no parameters (i.e. accepting `void`) must be handled differently. For the former, the `MOCK_FUNCTION` macro cannot be used. Instead, the `MOCK_FUNCTION_VOID` macro mentioned above is the way to go. What has not been described is how to handle functions taking no parameters. For this, there are two options. Either omit the `void` parameter type as in

```c
MOCK_FUNCTION(int, getchar);
```

or, alternatively, use the `MOCK_FUNCTION0` macro, i.e.

```c
MOCK_FUNCTION0(int, getchar, void);
```

The trailing 0 tells the library that the parameter type should not be used for generating parameter names. As far as code generation is concerned, the two options are entirely identical. As may be expected, a `MOCK_FUNCTION_VOID0` macro is provided for generating functions both returning and taking `void`, e.g.

```c
MOCK_FUNCTION_VOID0(void, abort, void);
```

Also, see the notes on user-defined functions [here](#mocking-user-defined-functions).

#### Building

Once the mockups file has been created, building is as simple as running

```sh
make
```

This generates the actual shared object, `libcldm.so`.

##### Build Options

By default, the library exposes macros without prefixes (e.g. `MOCK_FUNCTION`, `WILL_ONCE`, etc.). The pollution of the global namespace may be reduced by defining `CLDM_PREFIX_ONLY` before including `cldm.h`. This will expose only the [prefix versions](#prefix-macros) of the macros. Similarly, defining `CLDM_GMOCK_COMPAT` exposes [aliases](#gmock-style-macros) for the macros that should be familiar to gmock users.

#### Using the Library

The mocking approach relies on preloading the shared object, making ld prioritize symbols defined in the library over others loaded at runtime. The usage, in a nutshell, would be to build the final binary with the `-lcldm` flag and invoking it with

```sh
LD_PRELOAD=/path/to/libcldm.so /path/to/binary
```

Note that, when preloading the library, the lib prefix should _not_ be abbreviated to a single l ("el") as when passing the flag to the compiler.

##### Mocking User-Defined Functions

The library may be used to mock user-defined functions as well. However, as it relies on the symbol resolution of the runtime linker, symbols generated from these functions may not be linked directly into the binary. What this boils down to that any functions to be mocked have to be compiled and linked into a shared library that is loaded at runtime. 

The following illustrates how the user-defined function `get_resource` may be mocked.

`cldm/mockups.h`
```c
#include "cldm.h"
MOCK_FUNCTION(int *, get_resource, int);
```

`resource.h`
```c
int *get_resource(int id);
```

`resource.c`
```c
static int resource_array[256];

int *get_resource(int id) {
    return &resource_array[id];
}
```

`main.c`
```c
#include "cldm.h"
#include "resource.h"

#include <assert.h>

int main(void) {
    int i = 12;
    EXPECT_CALL(get_resource).WILL_ONCE(RETURN(&i));
    int *res = get_resource(0);
    assert(res == &i);

    return 0;
}
```

`on the shell`
```sh
make -C /path/to/project/root
gcc -shared -fPIC -o libresource.so resource.c
gcc -o a.out main.c -L. -L/path/to/project/root -lresource -lcldm -I/path/to/project/root/cldm/
LD_PRELOAD=/path/to/project/root/libcldm.so LD_LIBRARY_PATH=. ./a.out
```

Here, `path/to/project/root` refers to the root of the cldm project.

###### Building Incorrectly

As mentioned, linking the symbols to be mocked into the actual binary will cause the mocking to fail. As a result, given that the code above is used, the following will _NOT_ work.

```sh
make -C /path/to/project/root
gcc -o a.out main.c resource.c -L/path/to/project/root -lcldm -I/path/to/project/root/cldm/
LD_PRELOAD=/path/to/project/root/libcldm.so ./a.out
```

## Macro Reference

The macros can be subdivided into three categories - mock generators, behavior specifiers and actions. Mock generators are used for generating the code required to perform the actual function mocking. This generated code is linked into the library. Behavior specifiers and actions are combined in order to specify the runtime behavior of the mocked function.

### Mock Generators

###### `MOCK_FUNCTION(return_type, function_name [,param_type[,param_type[,...]]])`
Generates mocking code for a function called *function_name*, returning an instance of *return_type* and taking parameters of the *param_type*s specified. Any choice of type and number of parameters is valid, including none, **except** for a single void parameter. Furthermore, *return_type* may not be `void`, here `MOCK_FUNCTION_VOID` should be used instead.

###### `MOCK_FUNCTION0(return_type, function_name, void)`
Like `MOCK_FUNCTION` but allows for explicitly specifying the single `void` parameter for functions taking no parameters. The code generated is identical to that of `MOCK_FUNCTION(return_type, function_name)`.

###### `MOCK_FUNCTION_VOID(function_name [,param_type[,param_type[,...]]])`
Generates mocking code for the function *function_name*, returning `void`. Any choice of parameters except for a single `void` one is supported.

###### `MOCK_FUNCTION_VOID0(function_name, void)`
Like `MOCK_FUNCTION_VOID` but allows for explicitly specifying the single `void` parameter. C.f. `MOCK_FUNCTION0`.

### Behavior Specifiers

###### `EXPECT_CALL(function_name)`
First half of setting up a mocking behavior for the function *function_name*. Combined with `WILL_ONCE` or `WILL_REPEATEDLY` to generate a well-formed statement. Mocking code for *function_name* must have been generated using a `MOCK_FUNCTION` or `MOCK_FUNCTION_VOID` macro.

###### `WILL_ONCE(action)`
One of the potential second halves for setting up a mocking behavior, must appear appended to an `EXPECT_CALL` invocation (separated by a period (.)). `WILL_ONCE` causes the next immediate call to the function being mocked to generate a call described by the *action* parameter instead. Any subsequent call invokes the original, unmocked function.

###### `WILL_REPEATEDLY(action)`
One of the potential second halves for setting up a mocking behavior. `WILL_REPEATEDLY` works much like `WILL_ONCE`, the only difference being that all subsequent calls to the function begin mocked will generate calls to the mock instead.

###### `WILL_N_TIMES(n, action)`
Works much like `WILL_ONCE` but specifies that the *n* next function calls should generate a call described by the *action*. After *n* calls have been made, the default behavior of the function is restored.

###### `WILL_INVOKE_DEFAULT()`
Specifies that any subsequent calls to the function to be mocked should invoke the actual function instead of generating calls to the mock. May be used to undo the effects of `WILL_REPEATEDLY`.

### Actions

###### `INVOKE(function_name)`
An action to be used with a behavior specifier such as `WILL_ONCE`.  Causes any attempted call to the mocked function to result in a call to the function *function_name* instead. Naturally, the signature of *function_name* and the mocked function must be the compatible.

###### `RETURN(value)`
An action causing any call to the mocked function to immediately return *value* instead.

###### `RETURN_ARG(n)`
Returns the *n*th 0-indexed function parameter. The behavior of this action is well-defined only if the return type and the type of the *n*th parameter are the same.

###### `RETURN_POINTEE(n)`
Dereferences the *n*th 0-indexed function parameter and returns the resulting value. The behavior is well-defined only if the *n*th parameter is a pointer to an instance of the return type.

###### `INCREMENT_COUNTER(initial_value)`
An action that causes any call to the mocked function to increment and return a counter, in that order. The parameter *initial_value* specified the initial value of said counter. The use of `INCREMENT_COUNTER` is well-formed only if the mocked function returns an integral type. Note that that max value of the counter is subject to the limits of the return type (e.g. for a `signed char`, the counter could not be increment past `SCHAR_MAX`).

###### `ASSIGN(lhs, rhs [,type])`
Causes the function call to result in the equivalent of `lhs = rhs`. For the two-parameter version, `lhs` and `rhs` must both be lvalues. If wanting to pass an rvalue as `rhs`, provide a third parameter specifying the type `rhs` should be interpreted as. As an example, `EXPECT_CALL(foo).WILL_ONCE(ASSIGN(var, 10, int));` will assign the rvalue 10, interpreted as an `int`, to the variable `var`.

For the two-parameter version of the action to be well-defined, both `lhs` and `rhs` must be of the same type. If they are not, the three-parameter version can be used to force the type of `rhs`, e.g.

```c
int main(void) {
    long long i;
    int j = 12;
    EXPECT_CALL(foo).WILL_ONCE(ASSIGN(i, j, long long)); /* OK, j converted to a long long */
    EXPECT_CALL(foo).WILL_ONCE(ASSIGN(i, j)); /* Undefined behavior, j intepreted as a long long */
                                              /* despite being an int */
}
```

###### `ASSIGN_ARG(n, lhs)`
Assigns the *n*th zero-indexed parameter to *lhs* as if `lhs = args[n]` where `args[n]` indicates the *n*th parameter. This action is well-defined only if *lhs* and the *n*th parameter are of the same type.

### Prefix Macros

If `CLDM_PREFIX_ONLY` is defined prior to including the header, the default macros are not exposed. Instead, only their prefix counterparts are generated.

###### `CLDM_MOCK_FUNCTION(return_type, function_name [,param_type[,param_type[,...]]])`
Prefixed alias for [`MOCK_FUNCTION`](#mock_functionreturn_type-function_name-param_typeparam_type). 

###### `CLDM_MOCK_FUNCTION0(return_type, function_name, void)`
Prefixed alias for [`MOCK_FUNCTION0`](#mock_function0return_type-function_name-void). 

###### `CLDM_MOCK_FUNCTION_VOID(function_name [,param_type[,param_type[,...]]])`
Prefixed alias for [`MOCK_FUNCTION_VOID`](#mock_function_voidfunction_name-param_typeparam_type).

###### `CLDM_MOCK_FUNCTION_VOID0(function_name, void)`
Prefixed alias for [`MOCK_FUNCTION_VOID0`](#mock_function_void0function_name-void).

###### `CLDM_EXPECT_CALL(function_name)`
Prefixed alias for [`EXPECT_CALL`](expect_callfunction_name).

###### `CLDM_WILL_ONCE(action)`
Prefixed alias for [`WILL_ONCE`](#will_onceaction).

###### `CLDM_WILL_REPEATEDLY(action)`
Prefixed alias for [`WILL_REPEATEDLY`](#will_repeatedlyaction).

###### `CLDM_WILL_N_TIMES(n, action)`
Prefixed alias for [`WILL_N_TIMES`](#will_n_timesn-action).

###### `CLDM_WILL_INVOKE_DEFAULT()`
Prefixed alias for [`WILL_INVOKE_DEFAULT`](#will_invoke_default).

###### `CLDM_INVOKE(function_name)`
Prefixed alias for [`INVOKE`](#invokefunction_name).

###### `CLDM_RETURN(value)`
Prefixed alias for [`RETURN`](#returnvalue).

###### `CLDM_RETURN_ARG(n)`
Prefixed alias for [`RETURN_ARG`](#return_argn).

###### `CLDM_RETURN_POINTEE(n)`
Prefixed alias for [`RETURN_POINTEE`](#return_pointeen).

###### `CLDM_INCREMENT_COUNTER(initial_value)`
Prefixed alias for [`INCREMENT_COUNTER`](#increment_counterinitial_value).

###### `CLDM_ASSIGN(lhs, rhs [,type])`
Prefixed alias for [`ASSIGN`](#assignlhs-rhs-type).

###### `CLDM_ASSIGN(n, lhs)`
Prefixed alias for [`ASSIGN_ARG`](#assign_argn-lhs).

### Gmock-Style Macros

If `CLDM_GMOCK_COMPAT` is defined, the header exposes aliases named according to the "gmock-style".

###### `WillOnce(action)`
gmock-style alias for [`WILL_ONCE`](#will_onceaction).

###### `WillRepeatedly(action)`
gmock-style alias for [`WILL_REPEATEDLY`](#will_repeatedlyaction).

###### `WillNTimes(n, action)`
gmock-style alias for [`WILL_N_TIMES`](#will_n_timesn-action).

###### `WillInvokeDefault()`
gmock-style alias for [`WILL_INVOKE_DEFAULT`](#will_invoke_default).

###### `Invoke(function_name)`
gmock-style alias for [`INVOKE`](#invokefunction_name).

###### `Return(value)`
gmock-style alias for [`RETURN`](#returnvalue).

###### `ReturnArg(n)`
gmock-style alias for [`RETURN_ARG`](#return_argn).

###### `ReturnPointee(n)`
gmock-style alias for [`RETURN_POINTEE`](#return_pointeen).

###### `IncrementCounter(initial_value)`
gmock-style alias for [`INCREMENT_COUNTER`](#increment_counterinitial_value).

###### `Assign(lhs, rhs [,type])`
gmock-style alias for [`ASSIGN`](#assignlhs-rhs-type).

###### `AssignArg(n, lhs)`
gmock-style alias for [`ASSIGN_ARG`](#assign_argn-lhs).

