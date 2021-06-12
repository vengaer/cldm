# cldm - Unit Test and Mocking Framework for C

**NOTE** There is no stable of release of cldm as of yet. Both the innards and interface may change quickly. As such, neither is this readme complete nor are any guarantees made that it is kept up to date until such a time that a release is made.

Unit test and mocking framework in pure C. Requires a POSIX-compatible compiler. 

[[_TOC_]]

## Example Usage

Using cldm, the behavior of any function, including the ones provided by the standard, can in theory be overridden. The one requirement is that the symbol to be mocked is loaded dynamically (i.e. from a shared library). The following shows an example of how the output of `atoi` ([man page](https://man.archlinux.org/man/atoi.3)) may be manipulated at runtime.

```c
#include <cldm/cldm.h>

#include <stdlib.h>

TEST(mocking_example) {
    WHEN_CALLED(atoi).SHOULD_ONCE(RETURN(8));
    ASSERT_EQ(atoi("2"), 8); /* Passes due to the mock */
    ASSERT_EQ(atoi("2"), 2); /* Default behavior restored */

    WHEN_CALLED(atoi).SHOULD_REPEATEDLY(RETURN(7));
    ASSERT_EQ(atoi("2"), 7); /* Passes thanks to the mock */
    ASSERT_EQ(atoi("6"), 7); /* Ditto */
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
At most 127 parameters are supported[^1].

The following would generate code necessary to mock `atoi` provided by libc.

`cldm/mockups.h`
```c
#include <cldm/cldm.h>

MOCK_FUNCTION(int, atoi, char const *);
```

The path to the function describing the mocks may be modified by passing `MOCKUPS=<new path>` to `make` when building.

#### Building

Once the mockups file has been created, building is as simple as running

```sh
make
```

This generates the actual shared object, `libcldm.so`, and the static library `libcldm_main.a`. The latter provides a `main` function suitable for use with cldm and is expected to be linked into the test binary.

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
#include <cldm/cldm.h>
#include "resource.h"


TEST(user_defined) {
    int i = 12;
    WHEN_CALLED(get_resource).SHOULD_ONCE(RETURN(&i));
    int *res = get_resource(0);
    ASSERT_EQ(res, &i);
}
```

`on the shell`
```sh
make -C /path/to/project/root
gcc -shared -fPIC -o libresource.so resource.c
gcc -o a.out main.c -L. -L/path/to/project/root -lresource -lcldm -lcldm_main 
LD_PRELOAD=/path/to/project/root/libcldm.so LD_LIBRARY_PATH=. ./a.out
```

Here, `path/to/project/root` refers to the root of the cldm project.

###### Building Incorrectly

As mentioned, linking the symbols to be mocked into the actual binary will cause the mocking to fail. As a result, given that the code above is used, the following will _NOT_ work.

```sh
make -C /path/to/project/root
gcc -o a.out main.c resource.c -L/path/to/project/root -lcldm -lcldm_main 
LD_PRELOAD=/path/to/project/root/libcldm.so ./a.out
```
[^1]: Technically, the number is `min(127,#max_macro_params_supported_by_compiler - 2)`. A standard-compliant compiler must support at least 127 macro parameters.
