#include "hash_fuzz.h"

#include <cldm/cldm_algo.h>
#include <cldm/cldm_hash.h>

#include <stdio.h>
#include <stdlib.h>

static int qcmp(void const *l, void const *r) {
    return *(unsigned short *)r - *(unsigned short *)l;
}

void report_failure(char const *restrict pass, struct cldm_ht const *restrict ht, unsigned short value) {
    fprintf(stderr, "%s failure:\n", pass);
    fprintf(stderr, "Value: %hu\n", value);
    fprintf(stderr, "Size: %zu\n", cldm_ht_size(ht));
}

int hash_fuzz(uint8_t const *data, size_t size) {
    unsigned short *unique;
    unsigned short *input;
    size_t nunique;
    bool crash;

    struct cldm_ht_entry *unique_entries;
    struct cldm_ht_entry *entries;
    struct cldm_ht ht;

    if(size < sizeof(*input)) {
        return 0;
    }

    crash = true;
    size = size / sizeof(*input) * sizeof(*input);

    unique = 0;
    entries = 0;
    unique_entries = 0;
    input = malloc(size);
    if(!input) {
        fputs("malloc failure\n", stderr);
        goto epilogue;
    }

    unique = malloc(size);
    if(!unique) {
        fputs("malloc failure\n", stderr);
        goto epilogue;
    }

    ht = cldm_ht_init();

    memcpy(input, data, size);
    size /= sizeof(*input);

    entries = malloc(size * sizeof(*entries));

    if(!entries) {
        fputs("malloc failure\n", stderr);
        goto epilogue;
    }

    unique_entries = malloc(size * sizeof(*entries));

    if(!unique_entries) {
        fputs("malloc failure\n", stderr);
        goto epilogue;
    }

    for(unsigned i = 0; i < size; i++) {
        entries[i] = cldm_ht_mkentry(input[i]);
    }

    for(unsigned i = 0; i < size; i++) {
        if(!cldm_ht_insert(&ht, &entries[i])) {
            report_failure("Insertion", &ht, input[i]);
            goto epilogue;
        }

        for(unsigned j = 0; j <= i; j++) {
            if(!cldm_ht_find(&ht, &entries[j])) {
                report_failure("Find during insertion", &ht, input[j]);
                goto epilogue;
            }
        }
    }

    memcpy(unique, input, size * sizeof(*input));
    qsort(unique, size, sizeof(*unique), qcmp);
    nunique = cldm_uniq(unique, unique, sizeof(*unique), size, qcmp);

    for(unsigned i = 0; i < nunique; i++) {
        unique_entries[i] = cldm_ht_mkentry(unique[i]);
    }

    for(unsigned i = 0; i < nunique; i++) {
        if(!cldm_ht_remove(&ht, &unique_entries[i])) {
            report_failure("Removal", &ht, unique[i]);
            goto epilogue;
        }
        for(unsigned j = i + 1; j < nunique; j++) {
            if(!cldm_ht_find(&ht, &unique_entries[j])) {
                report_failure("Find during removal", &ht, unique[j]);
                goto epilogue;
            }
        }
    }

    crash = false;
epilogue:
    if(input) {
        free(input);
    }
    if(unique) {
        free(unique);
    }
    if(entries) {
        free(entries);
    }
    if(unique_entries) {
        free(unique_entries);
    }
    cldm_ht_free(&ht);
    if(crash) {
        abort();
    }
    return 0;
}
