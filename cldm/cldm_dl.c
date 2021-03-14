#include "cldm_dl.h"
#include "cldm_elf.h"
#include "cldm_limits.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_ntbs.h"

#include <dlfcn.h>

static void *cldm_libchandle;

struct cldm_dlfunc {
    char const *name;
    void **addr;
};

bool cldm_dl_mapped;

int   (*cldm_dl_fprintf)(FILE *restrict, char const *restrict , ...);
FILE *(*cldm_dl_fopen)(char const *restrict, char const *restrict);
int   (*cldm_dl_fclose)(FILE *);
int   (*cldm_dl_fflush)(FILE *);
char *(*cldm_dl_fgets)(char *, int, FILE *);
int   (*cldm_dl_fileno)(FILE *);
int   (*cldm_dl_dup)(int);
int   (*cldm_dl_dup2)(int, int);

static struct cldm_dlfunc cldm_funcmap[8];

static void cldm_init_funcmap(void) {
    cldm_funcmap[0]  = (struct cldm_dlfunc) { "fprintf", (void **)&cldm_dl_fprintf };

    cldm_funcmap[1]  = (struct cldm_dlfunc) { "fopen",   (void **)&cldm_dl_fopen   };
    cldm_funcmap[2]  = (struct cldm_dlfunc) { "fclose",  (void **)&cldm_dl_fclose  };
    cldm_funcmap[3]  = (struct cldm_dlfunc) { "fflush",  (void **)&cldm_dl_fflush  };
    cldm_funcmap[4]  = (struct cldm_dlfunc) { "fgets",   (void **)&cldm_dl_fgets   };
    cldm_funcmap[5]  = (struct cldm_dlfunc) { "fileno",  (void **)&cldm_dl_fileno  };

    cldm_funcmap[6]  = (struct cldm_dlfunc) { "dup",     (void **)&cldm_dl_dup     };
    cldm_funcmap[7]  = (struct cldm_dlfunc) { "dup2",    (void **)&cldm_dl_dup2    };
}

void *cldm_dlsym_next(char const *symname) {
    char *err;
    void *sym;
    (void)dlerror();

    sym = dlsym(RTLD_NEXT, symname);

    err = dlerror();
    if(err) {
        cldm_err("%s", err);
        return 0;
    }

    return sym;
}

int cldm_dlgentab(char const *progname) {
    char buffer[CLDM_PATH_MAX];
    struct cldm_dlfunc *iter;
    char *err;
    int status;
    struct cldm_elfmap map;
    ssize_t nbytes;
    char const *libcname;

    status = -1;

    if(cldm_map_elf(&map, progname)) {
        cldm_err("Could not map %s", progname);
        return -1;
    }

    nbytes = cldm_elf_read_needed(&map, buffer, sizeof(buffer));
    if(nbytes < 0) {
        cldm_err("Could not list needed shared objects");
        return -1;
    }

    cldm_unmap_elf(&map);

    for(ssize_t i = 0; i < nbytes; i += cldm_ntbslen(buffer + i) + 1) {
        if(cldm_ntbs_find_substr(buffer + i, "libc.so")) {
            libcname = buffer + i;
            break;
        }
    }

    cldm_libchandle = dlopen(libcname, RTLD_LAZY);
    if(!cldm_libchandle) {
        cldm_err("%s", dlerror());
        return -1;
    }

    cldm_init_funcmap();

    (void) dlerror();
    cldm_for_each(iter, cldm_funcmap) {
        *iter->addr = dlsym(cldm_libchandle, iter->name);
        err = dlerror();
        if(err) {
            cldm_err("%s", err);
            goto epilogue;
        }
    }

    cldm_dl_mapped = true;
    status = 0;

epilogue:
    if(status) {
        dlclose(cldm_libchandle);
    }

    return status;;
}

int cldm_dlclose(void) {
    return dlclose(cldm_libchandle);
}
