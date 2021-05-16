#include "cldm_cache.h"
#include "cldm_dl.h"
#include "cldm_hash.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_ntbs.h"
#include "cldm_rtassert.h"
#include "cldm_thread.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#define CLDM_SOLIB "libcldm.so"

enum { CLDM_MAX_SODEPS = 32 };
enum { CLDM_DLBUF_SIZE = 1024 };
enum { CLDM_DLNAME_MAX_SIZE = 32 };
enum { CLDM_INITIAL_DLLOOKUP_SIZE = 32 };
enum { CLDM_MAX_SYMNAME_SIZE = 128 };

struct cldm_dlentry {
    char symname[CLDM_MAX_SYMNAME_SIZE];
    void *addr;
    unsigned loadidx;
    bool resolved;
};

struct cldm_dllookup {
    struct cldm_ht ht;
    struct cldm_dlentry *dlentries;
    size_t capacity;
};

struct cldm_dltab {
    void **addr;
    char const *name;
};

int   (*cldm_explicit_open)(char const *, int);
int   (*cldm_explicit_close)(int);
int   (*cldm_explicit_fstat)(int, struct stat *);
void *(*cldm_explicit_mmap)(void *, size_t, int, int, int, off_t);
void *(*cldm_explicit_memcpy)(void *restrict, void const *restrict, size_t);
int   (*cldm_explicit_munmap)(void *, size_t);
int   (*cldm_explicit_strcmp)(char const *, char const *);
int   (*cldm_explicit_strncmp)(char const *, char const *, size_t);
char *(*cldm_explicit_strerror)(int);
int   (*cldm_explicit_fprintf)(FILE *restrict, char const *restrict, ...);

static unsigned ndlhandles;
static void *dlhandles[CLDM_MAX_SODEPS];
static char dlnames[CLDM_DLNAME_MAX_SIZE][CLDM_MAX_SODEPS];
static cldm_cachealign(struct cldm_dllookup) dllookups[CLDM_MAX_THREADS];

static bool cldm_dllookup_init(unsigned thread_id) {
    dllookups[thread_id].data.ht = cldm_ht_init();
    dllookups[thread_id].data.dlentries = malloc(CLDM_INITIAL_DLLOOKUP_SIZE * sizeof(*dllookups[thread_id].data.dlentries));
    if(!dllookups[thread_id].data.dlentries) {
        return false;
    }
    dllookups[thread_id].data.capacity = CLDM_INITIAL_DLLOOKUP_SIZE;
    return true;
}

static bool cldm_dllookup_ensure_capacity(unsigned thread_id) {
    void *addr;
    if(cldm_ht_size(&dllookups[thread_id].data.ht) < dllookups[thread_id].data.capacity) {
        return true;
    }

    addr = realloc(dllookups[thread_id].data.dlentries, 2 * dllookups[thread_id].data.capacity * sizeof(*dllookups[thread_id].data.dlentries));
    if(!addr) {
        return false;
    }
    dllookups[thread_id].data.dlentries = addr;
    dllookups[thread_id].data.capacity *= 2u;
    return true;
}

int cldm_dlmap_explicit(void) {
    struct cldm_dltab *iter;
    char const **libc;
    void *handle;
    int status;
    char *err;

    /* Cannot use sprintf and the like since they may be mocked. This should
     * cover libc versions on more or less any system */
    char const *libcvers[] = {
        "libc.so.0",  "libc.so.1",  "libc.so.2",  "libc.so.3",
        "libc.so.4",  "libc.so.5",  "libc.so.6",  "libc.so.7",
        "libc.so.8",  "libc.so.9",  "libc.so.10", "libc.so.11",
        "libc.so.12", "libc.so.13", "libc.so.14", "libc.so.15",
        "libc.so.16", "libc.so.17", "libc.so.18", "libc.so.19",
        "libc.so.20", "libc.so.21", "libc.so.22", "libc.so.23",
        "libc.so.24", "libc.so.25", "libc.so.26", "libc.so.27",
        "libc.so.28", "libc.so.29", "libc.so.30", "libc.so.31"
    };

    struct cldm_dltab lookup[] = {
        { (void **)&cldm_explicit_open,     "open"     },
        { (void **)&cldm_explicit_close,    "close"    },
        { (void **)&cldm_explicit_fstat,    "fstat"    },
        { (void **)&cldm_explicit_mmap,     "mmap"     },
        { (void **)&cldm_explicit_memcpy,   "memcpy"   },
        { (void **)&cldm_explicit_munmap,   "munmap"   },
        { (void **)&cldm_explicit_strcmp,   "strcmp"   },
        { (void **)&cldm_explicit_strncmp,  "strncmp"  },
        { (void **)&cldm_explicit_strerror, "strerror" },
        { (void **)&cldm_explicit_fprintf,  "fprintf"  }
    };

    cldm_for_each(libc, libcvers) {
        handle = dlopen(*libc, RTLD_LAZY);
        if(handle) {
            break;
        }
    }

    if(!handle) {
        cldm_err("Could not open libc.so (tried versions 0-%u)", cldm_arrsize(libcvers) - 1u);
        return 1;
    }

    status = 1;
    (void)dlerror();
    cldm_for_each(iter, lookup) {
        *iter->addr = dlsym(handle, iter->name);
        err = dlerror();
        if(err) {
            cldm_log("Could not load %s from %s: %s", iter->name, *libc, err);
            goto epilogue;
        }
    }

    status = 0;
epilogue:
    dlclose(handle);
    return status;
}

int cldm_dlmap_needed(struct cldm_elfmap const *map) {
    char buffer[CLDM_DLBUF_SIZE];
    ssize_t nbytes;

    nbytes = cldm_elf_read_needed(map, buffer, sizeof(buffer));
    if(nbytes < 0) {
        return -1;
    }

    for(unsigned i = 0; i < (unsigned)nbytes; i += strlen(buffer + i) + 1) {
        if(cldm_explicit_strncmp(CLDM_SOLIB, buffer + i, cldm_strlitlen(CLDM_SOLIB)) != 0) {
            if(ndlhandles < CLDM_MAX_SODEPS) {
                dlhandles[ndlhandles] = dlopen(buffer + i, RTLD_LAZY);

                if(!dlhandles[ndlhandles]) {
                    cldm_err("Could not load needed shared library %s", buffer + i);
                    cldm_dlfree();
                    return -1;
                }
                if(cldm_strscpy(dlnames[ndlhandles], buffer + i, sizeof(dlnames[i])) < 0) {
                    cldm_warn("Name of shared library %s truncated", buffer + i);
                }
                ++ndlhandles;
            }
            else {
                cldm_warn("Max number of loaded shared libraries reached, ignoring %s", buffer + i);
            }
        }
    }

    return ndlhandles;
}

void cldm_dlfree(void) {
    for(unsigned i = 0; i < ndlhandles; i++) {
        if(dlclose(dlhandles[i])) {
            cldm_warn("Could not close shared library %s", dlnames[i]);
        }
    }

    for(unsigned i = 0; i < cldm_jobs; i++) {
        if(dllookups[i].data.capacity) {
            free(dllookups[i].data.dlentries);
            cldm_ht_free(&dllookups[i].data.ht);
        }
    }
}

void *cldm_dlsym_next(char const *symname) {
    struct cldm_ht_entry *htentry;
    struct cldm_dlentry *dlentry;
    unsigned thread_id;
    unsigned probeidx;
    void *sym;

    thread_id = cldm_thread_id();

    if(!dllookups[thread_id].data.capacity) {
        if(!cldm_dllookup_init(thread_id)) {
            cldm_err("Could not allocate memory for symbol caching");
            return 0;
        }
    }
    else if(!cldm_dllookup_ensure_capacity(thread_id)) {
        cldm_err("Could not increase size of symbol cache");
        return 0;
    }

    (void)dlerror();

    probeidx = 0;
    htentry = cldm_ht_find(&dllookups[thread_id].data.ht, &cldm_ht_mkentry_str(symname));
    if(htentry) {
        /* Access struct by address of its first member */
        dlentry = (void *)htentry->key;
        if(dlentry->resolved) {
            sym = dlentry->addr;
            goto epilogue;
        }
        probeidx = dlentry->loadidx + 1u;
    }

    for(; probeidx < ndlhandles; ++probeidx) {
        sym = dlsym(dlhandles[probeidx], symname);
        if(!dlerror()) {
            break;
        }
    }

    if(htentry) {
        dlentry->addr = sym;
        dlentry->loadidx = probeidx;
        goto epilogue;
    }

    dlentry = &dllookups[thread_id].data.dlentries[cldm_ht_size(&dllookups[thread_id].data.ht)];
    if(cldm_strscpy(dlentry->symname, symname, sizeof(dlentry->symname)) < 0) {
        cldm_err("Name of symbol %s overflows internal buffer", symname);
        return 0;
    }
    dlentry->addr = sym;
    dlentry->loadidx = probeidx;
    dlentry->resolved = false;

    if(!cldm_ht_insert(&dllookups[thread_id].data.ht, &cldm_ht_mkentry_str(dlentry->symname))) {
        cldm_err("Could not cache symbol %s", symname);
        return 0;
    }

epilogue:
    if(!sym) {
        cldm_err("Lookup of symbol %s failed", symname);
    }

    return sym;
}

void cldm_dlresolve(char const *symname) {
    struct cldm_ht_entry *htentry;
    htentry = cldm_ht_find(&dllookups[cldm_thread_id()].data.ht, &cldm_ht_mkentry_str(symname));
    cldm_rtassert(htentry);
    ((struct cldm_dlentry *)htentry->key)->resolved = true;
}
