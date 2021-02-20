#define LMC_GENERATE_SYMBOLS
#include "lmc.h"

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
struct lmc_mock_info *mockinfo;

static void lmc_replace(char to, char from, char *buffer) {
    char *c = strchr(buffer, from);

    while(c) {
        *c = to;
        c = strchr(c + 1, from);
    }
}

void lmc_close_dlhandle(void) {
    if(dlhandle) {
        dlclose(dlhandle);
        dlhandle = 0;
    }
}

static void *lmc_search_so(char const *libname, char const *symname) {
    dlhandle = dlopen(libname, RTLD_LAZY);
    if(!dlhandle) {
        fprintf(stderr, "Could not open shared library %s\n", libname);
        return 0;
    }

    dlerror();
    return dlsym(dlhandle, symname);
}

static void *lmc_dllookup(char const *symname) {
    char buffer[PATH_SIZE];
    char const *path = getenv("LD_LIBRARY_PATH");

    lmc_assert((size_t)snprintf(buffer, sizeof(buffer), "%s", path) < sizeof(buffer), "LD_LIBRARY_PATH overflows buffer");
    lmc_replace(' ', ':', buffer);

    char *iter;
    void *sym = 0;

    struct {
        regex_t rgx;
        bool compiled;
    } sopattern = { .compiled = false }, lmcpattern = { .compiled = false };

    DIR *dirhandle = 0;
    struct dirent *dir;

    /* Try libc first */
    if(!sym) {
        char const *libc_path = lmc_str_expand(LMC_LIBC);
        struct stat sb;
        if(stat(libc_path, &sb) == 0) {
            sym = lmc_search_so(libc_path, symname);
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

    if(regcomp(&lmcpattern.rgx, "libmockc\\.so(\\.[0-9]+)?", REG_EXTENDED)) {
        fputs("Shared object exclusion regex could not be compiled\n", stderr);
        goto cleanup;
    }
    lmcpattern.compiled = true;

    lmc_for_each_word(iter, buffer) {
        dirhandle = opendir(iter);

        lmc_assert(dirhandle, "Could not open directory");
        while((dir = readdir(dirhandle))) {

            if(regexec(&sopattern.rgx, dir->d_name, 0, 0, 0) == 0) {
                if(regexec(&lmcpattern.rgx, dir->d_name, 0, 0, 0) == 0) {
                    continue;
                }

                sym = lmc_search_so(dir->d_name, symname);

                if(sym) {
                    goto cleanup;
                }

                lmc_close_dlhandle();
            }
        }

        closedir(dirhandle);
        dirhandle = 0;
    }

cleanup:
    if(sopattern.compiled) {
        regfree(&sopattern.rgx);
    }
    if(lmcpattern.compiled) {
        regfree(&lmcpattern.rgx);
    }
    if(dirhandle) {
        closedir(dirhandle);
    }
    lmc_assert(sym, "Dynamic symbol lookup error");
    return sym;
}

void *lmc_symbol(char const *symname) {
    return lmc_dllookup(symname);
}
