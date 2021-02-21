#define CMOCK_GENERATE_SYMBOLS
#include "cmock.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <regex.h>
#include <sys/stat.h>

enum { PATH_SIZE = 2048 };

static void *dlhandle;
struct cmock_mock_info *mockinfo;

static void cmock_replace(char to, char from, char *buffer) {
    char *c = strchr(buffer, from);

    while(c) {
        *c = to;
        c = strchr(c + 1, from);
    }
}

void cmock_close_dlhandle(void) {
    if(dlhandle) {
        dlclose(dlhandle);
        dlhandle = 0;
    }
}

static void *cmock_search_so(char const *libname, char const *symname) {
    dlhandle = dlopen(libname, RTLD_LAZY);
    if(!dlhandle) {
        fprintf(stderr, "Could not open shared library %s\n", libname);
        return 0;
    }

    dlerror();
    return dlsym(dlhandle, symname);
}

void *cmock_dllookup(char const *symname) {
    char buffer[PATH_SIZE];
    char const *path = getenv("LD_LIBRARY_PATH");

    cmock_assert((size_t)snprintf(buffer, sizeof(buffer), "%s", path) < sizeof(buffer), "LD_LIBRARY_PATH %s overflows buffer", path);
    cmock_replace(' ', ':', buffer);

    char *iter;
    void *sym = 0;

    struct {
        regex_t rgx;
        bool compiled;
    } sopattern = { .compiled = false }, cmockpattern = { .compiled = false };

    DIR *dirhandle = 0;
    struct dirent *dir;

    /* Try libc first */
    if(!sym) {
        char const *libc_path = cmock_str_expand(CMOCK_LIBC);
        struct stat sb;
        if(stat(libc_path, &sb) == 0) {
            sym = cmock_search_so(libc_path, symname);
        }
    }

    if(sym) {
        goto cleanup;
    }

    if(regcomp(&sopattern.rgx, ".+\\.so(\\.[0-9]+)?", REG_EXTENDED)) {
        fputs("Shared object match regex could not be compiled\n", stderr);
        goto cleanup;
    }
    sopattern.compiled = true;

    if(regcomp(&cmockpattern.rgx, "libcmock\\.so(\\.[0-9]+)?", REG_EXTENDED)) {
        fputs("Shared object exclusion regex could not be compiled\n", stderr);
        goto cleanup;
    }
    cmockpattern.compiled = true;

    cmock_for_each_word(iter, buffer) {
        dirhandle = opendir(iter);

        cmock_assert(dirhandle, "Could not open directory %s", iter);
        while((dir = readdir(dirhandle))) {

            if(regexec(&sopattern.rgx, dir->d_name, 0, 0, 0) == 0) {
                if(regexec(&cmockpattern.rgx, dir->d_name, 0, 0, 0) == 0) {
                    continue;
                }

                sym = cmock_search_so(dir->d_name, symname);

                if(sym) {
                    goto cleanup;
                }

                cmock_close_dlhandle();
            }
        }

        closedir(dirhandle);
        dirhandle = 0;
    }

cleanup:
    if(sopattern.compiled) {
        regfree(&sopattern.rgx);
    }
    if(cmockpattern.compiled) {
        regfree(&cmockpattern.rgx);
    }
    if(dirhandle) {
        closedir(dirhandle);
    }
    cmock_assert(sym, "Lookup of symbol %s failed", symname);
    return sym;
}
