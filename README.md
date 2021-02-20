# libmockc

LD-based function mocking in pure C. The interface is inspired by [gmock](https://github.com/google/googletest).

[[_TOC_]]

## Example Usage

Using libmockc, the behavior of any function, including the ones provided by the standard, can be overridden. The one requirement is that the symbol to be mocked is loaded dynamically (i.e. from a shared library). The following shows an example of how the output of `atoi` ([man page](https://man.archlinux.org/man/atoi.3)) may be manipulated at runtime.

```c
#include "lmc.h"

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

## Using libmockc

Building libmockc requires some configuration. Specifically, it must know the signatures of the mocks it is to generate. Once this is done, the build system should handle the rest. The usage can be summarized as follows:

1. Describe functions to be mocked
2. Build libmockc via make
3. Preload the generated shared library
4. Include the `lmc.h` header

The process is described in detail below.

#### Defining the Mocks

In order for libmockc to know the names and the signatures of the functions to mock, these must be specified by the user. To do this, create a file called `mockups.h` in the `libmockc` directory and use the `MOCK_FUNCTION` and `MOCK_FUNCTION_VOID` macro families.

The "signature" of the `MOCK_FUNCTION` macro would be described as:
```c
MOCK_FUNCTION(return_type, function_name [,param_type[,param_type[,...]]]).
```
At most 127 parameters are supported. The `MOCK_FUNCTION_VOID` macro, used for functions returning `void`, is defined similarly, only omitting the return type, i.e. 
```c
MOCK_FUNCTION_VOID(function_name [,param_type[,param_type[,...]]]).
``` 
There are a few considerations that have to made here, [see below](#mocking-gotchas).

The following would generate code necessary to mock the standard `atoi`, `strcpy` and `free` functions.

`libmockc/mockups.h`
```c
#include "lmc.h"

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

This generates the actual shared object, `libmockc.so`.

##### Build Options

By default, the library will expose macros without prefixes (e.g. `MOCK_FUNCTION`, `WILL_ONCE`, etc.). The pollution of the global namespace may be reduced by defining `LMC_PREFIX_ONLY` before including `lmc.h`. This will expose only the [prefix versions](#prefix-macros) of the macros. Similarly, defining `LMC_GMOCK_COMPAT` exposes [aliases](#gmock-like-macros) for the macros that should be familiar to gmock users.

#### Using the Library

The mocking approach relies on preloading the shared object, making ld prioritize symbols defined in the library over the others loaded at runtime. The usage, in a nutshell, would be to build the final binary with the `-lmockc` flag and invoking it with

```sh
LD_PRELOAD=/path/to/libmockc.so /path/to/binary
```

Note that, when preloading the library, the lib prefix should _not_ be abbreviated to a single l ("el") as when passing the flag to the compiler.

##### Mocking User-Defined Functions

The library may be used to mock user-defined functions as well. However, as it relies on the symbol resolution of the runtime linker, symbols generated from these functions may not be linked directly into the binary. What this boils down to that any functions to be mocked have to be compiled and linked into a shared library that is loaded at runtime. 

The following illustrates how the user-defined function `get_resource` may be mocked.

`libmockc/mockups.h`
```c
#include "lmc.h"
MOCK_FUNCTION(int *, get_resource, int);
```

`resource.h`
```c
int *get_resource(int id);
```

`resource.c`
```c
int *get_resource(int id) {
    return &resource_array[id];
}
```

`main.c`
```c
#include "lmc.h"
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
gcc -o a.out main.c -L. -L/path/to/project/root -lresource -lmockc -I/path/to/project/root/libmockc/
LD_PRELOAD=/path/to/project/root/libmockc.so LD_LIBRARY_PATH=. ./a.out
```

###### Building Incorrectly

As mentioned, linking the symbols to be mocked into the actual binary will cause the mocking to fail. As a result, given that the code above is used, the following will _NOT_ work.

```sh
make -C /path/to/project/root
gcc -o a.out main.c resource.c -L. -L/path/to/project/root -lmockc -I/path/to/project/root/libmockc/
LD_PRELOAD=/path/to/project/root/libmockc.so ./a.out
```

## Macro Reference

### Mock Generators

###### `MOCK_FUNCTION(return_type, function_name [,param_type[,param_type[,...]]])`
Generates mocking code for a function called *function_name*, returning *return_type* and taking parameters of the types specified. Any choice of parameter types is valid, including none, **EXCEPT** for a single void parameter. Furthermore, return_type may not be `void` (see `MOCK_FUNCTION_VOID`).

###### `MOCK_FUNCTION0(return_type, function_name, void)`
Like `MOCK_FUNCTION` but allows for explicitly specifying the single `void` parameter. The code generated is identical to that of `MOCK_FUNCTION(return_type, function_name)`.

###### `MOCK_FUNCTION_VOID(function_name [,param_type[,param_type[,...]]])`
Generates mocking code for the function *function_name*, returning `void`. Any choice of parameters except for a single `void` one is supported.

###### `MOCK_FUNCTION_VOID0(function_name, void)`
Like `MOCK_FUNCTION_VOID` but allows for explicitly specifying the single `void` parameter. C.f. `MOCK_FUNCTION0`.

### Behavior Specifiers

###### `EXPECT_CALL(function_name)`
First half of setting up a mocking behavior for the function *function_name*. Combined with `WILL_ONCE` or `WILL_REPEATEDLY` to generate a well-formed statement. Mocking code must have been generated using a `MOCK_FUNCTION` or `MOCK_FUNCTION_VOID` macro.

###### `WILL_ONCE(matcher)`
One of the potential second halves for setting up a mocking behavior, must appear appended to an `EXPECT_CALL` invocation (separated by a period (.)). `WILL_ONCE` causes the next immediate call to the function being mocked to generate a call to the mock instead. Any subsequent call invokes the original, unmocked function.

###### `WILL_REPEATEDLY(matcher)`
One of the potential second halves for setting up a mocking behavior. `WILL_REPEATEDLY` works much like `WILL_ONCE`, the only difference being that all subsequent calls to the function begin mocked will generate calls to the mock instead.

### Matchers

###### `INVOKE(function_name)`
A matcher to be used with a behavior specifier such as `WILL_ONCE`.  Causes any call to the mocked function resulting in a call to the function *function_name* instead. Naturally, the signature of *function_name* and the mocked function must be the compatible.

###### `RETURN(value)`
A matcher causing any call to the mocked function to immediately return *value* instead.

###### `INCREMENT_COUNTER(initial_value)`
A matcher that causes any call to the mocked function to increment and return a counter, in that order. The parameter *start_value* sets the initial value of the counter. The use of `INCREMENT_COUNTER` is well-formed only if the mocked function returns an integral type. Note that that max value of the counter is subject to the limits of the return type (e.g. for a `signed char`, the counter could not be increment past `SCHAR_MAX`).

### Prefix Macros

If `LMC_PREFIX_ONLY` is defined prior to including the header, the default macros are not exposed. Instead, only their prefix counterpart are generated.

###### `LMC_MOCK_FUNCTION(return_type, function_name [,param_type[,param_type[,...]]])`
Prefixed alias for `MOCK_FUNCTION`. 

###### `LMC_MOCK_FUNCTION0(return_type, function_name, void)`
Prefixed alias for `MOCK_FUNCTION0`. 

###### `LMC_MOCK_FUNCTION_VOID(function_name [,param_type[,param_type[,...]]])`
Prefixed alias for `MOCK_FUNCTION_VOID`.

###### `LMC_MOCK_FUNCTION_VOID0(function_name, void)`
Prefixed alias for `MOCK_FUNCTION_VOID0`.

###### `LMC_EXPECT_CALL(function_name)`
Prefixed alias for `EXPECT_CALL`.

###### `LMC_WILL_ONCE(matcher)`
Prefixed alias for `WILL_ONCE`.

###### `LMC_WILL_REPEATEDLY(matcher)`
Prefixed alias for `WILL_REPEATEDLY`.

###### `LMC_INVOKE(function_name)`
Prefixed alias for `INVOKE`.

###### `LMC_RETURN(value)`
Prefixed alias for `RETURN`.

###### `LMC_INCREMENT_COUNTER(initial_value)`
Prefixed alias for `INCREMENT_COUNTER`.

### Gmock-Style Macros

If `LMC_GMOCK_COMPAT` is defined, the header will expose aliases named after their gmock counterpart.

###### `WillOnce(matcher)`
gmock-style alias for `WILL_ONCE`.

###### `WillRepeatedly(matcher)`
gmock-style alias for `WILL_REPEATEDLY`.

###### `Invoke(function_name)`
gmock-style alias for `INVOKE`.

###### `Return(value)`
gmock-style alias for `RETURN`.

###### `IncrementCounter(initial_value)`
gmock-style alias for `INCREMENT_COUNTER`.