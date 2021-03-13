#include "cldm_dl.h"
#include "cldm_limits.h"
#include "cldm_log.h"
#include "cldm_ntbs.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

static char libcldm_path[CLDM_PATH_MAX / 4];

int cldm_unload(void) {
    char oldenv[CLDM_PATH_MAX / 2];
    char newenv[CLDM_PATH_MAX / 2];
    ssize_t pldsize;
    ssize_t nbytes;
    char *iter;
    char *dst;
    char *preload = getenv("LD_PRELOAD");

    if(!preload) {
        return 0;
    }

    pldsize = cldm_ntbscpy(oldenv, preload, sizeof(oldenv));
    if(pldsize < 0) {
        cldm_err("LD_PRELOAD from environment overflows internal buffer");
        return pldsize;
    }

    cldm_ntbsrepl(oldenv, ' ', ':');

    dst = newenv;
    cldm_for_each_word(iter, oldenv, ':') {
        if(!cldm_ntbs_find_substr(iter, "libcldm.so")) {
            nbytes = cldm_ntbscpy(dst, iter, sizeof(newenv) - (dst - newenv));
            cldm_assert(nbytes >= 0, "This should never be possible");
            dst += nbytes;
            cldm_assert(sizeof(newenv) - (dst - newenv) > 2);
            *dst++ = ':';
            *dst = '\0';
        }
        else {
            *libcldm_path = 1;
            nbytes = cldm_ntbscpy(libcldm_path + 1, iter, sizeof(libcldm_path - 1));
            if(nbytes < 0) {
                cldm_err("Could not store path to libcldm internally");
                return -1;
            }
        }
    }

    if(setenv("LD_PRELOAD", newenv, 1) == -1) {
        cldm_err("Could not overwrite LD_PRELOAD: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int cldm_preload(void) {
    char buffer[CLDM_PATH_MAX / 2];
    char *preload;
    ssize_t pldsize;

    if(!*libcldm_path) {
        return 0;
    }

    if(!libcldm_path[1]) {
        cldm_err("Path to libcldm not found");
        return -1;
    }

    preload = getenv("LD_PRELOAD");
    pldsize = cldm_ntbscpy(buffer, preload, sizeof(buffer));
    if(pldsize < 0) {
        cldm_err("LD_PRELOAD from environment overflows internal buffer");
        return pldsize;
    }

    pldsize = cldm_ntbscpy(buffer + pldsize, libcldm_path + 1, sizeof(buffer) - pldsize);
    if(pldsize < 0) {
        cldm_err("Appending path to libcldm to environment overflows itnernal buffer");
        return pldsize;
    }

    if(setenv("LD_PRELOAD", buffer, 1) == -1) {
        cldm_err("Could not overwrite LD_PRELOAD: %s", strerror(errno));
        return -1;
    }

    return 0;
}

