#define CLDM_GENERATE_SYMBOLS
#include "cldm.h"

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
struct cldm_mock_info *mockinfo;

static void cldm_replace(char to, char from, char *buffer) {
    char *c = strchr(buffer, from);

    while(c) {
        *c = to;
        c = strchr(c + 1, from);
    }
}

void cldm_close_dlhandle(void) {
    if(dlhandle) {
        dlclose(dlhandle);
        dlhandle = 0;
    }
}

static void *cldm_search_so(char const *libname, char const *symname) {
    dlhandle = dlopen(libname, RTLD_LAZY);
    if(!dlhandle) {
        fprintf(stderr, "Could not open shared library %s\n", libname);
        return 0;
    }

    dlerror();
    return dlsym(dlhandle, symname);
}

void *cldm_dllookup(char const *symname) {
    char buffer[PATH_SIZE];
    char const *path = getenv("LD_LIBRARY_PATH");

    cldm_assert((size_t)snprintf(buffer, sizeof(buffer), "%s", path) < sizeof(buffer), "LD_LIBRARY_PATH %s overflows buffer", path);
    cldm_replace(' ', ':', buffer);

    char *iter;
    void *sym = 0;

    struct {
        regex_t rgx;
        bool compiled;
    } sopattern = { .compiled = false }, cldmpattern = { .compiled = false };

    DIR *dirhandle = 0;
    struct dirent *dir;

    /* Try libc first */
    if(!sym) {
        char const *libc_path = cldm_str_expand(CLDM_LIBC);
        struct stat sb;
        if(stat(libc_path, &sb) == 0) {
            sym = cldm_search_so(libc_path, symname);
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

    if(regcomp(&cldmpattern.rgx, "libcldm\\.so(\\.[0-9]+)?", REG_EXTENDED)) {
        fputs("Shared object exclusion regex could not be compiled\n", stderr);
        goto cleanup;
    }
    cldmpattern.compiled = true;

    cldm_for_each_word(iter, buffer) {
        dirhandle = opendir(iter);

        cldm_assert(dirhandle, "Could not open directory %s", iter);
        while((dir = readdir(dirhandle))) {

            if(regexec(&sopattern.rgx, dir->d_name, 0, 0, 0) == 0) {
                if(regexec(&cldmpattern.rgx, dir->d_name, 0, 0, 0) == 0) {
                    continue;
                }

                sym = cldm_search_so(dir->d_name, symname);

                if(sym) {
                    goto cleanup;
                }

                cldm_close_dlhandle();
            }
        }

        closedir(dirhandle);
        dirhandle = 0;
    }

cleanup:
    if(sopattern.compiled) {
        regfree(&sopattern.rgx);
    }
    if(cldmpattern.compiled) {
        regfree(&cldmpattern.rgx);
    }
    if(dirhandle) {
        closedir(dirhandle);
    }
    cldm_assert(sym, "Lookup of symbol %s failed", symname);
    return sym;
}
