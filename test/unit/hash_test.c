#include <cldm/cldm.h>
#include <cldm/cldm_algo.h>
#include <cldm/cldm_hash.h>
#include <cldm/cldm_macro.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

TEST(cldm_ht_insert_static) {
    enum { SIZE = 8 };
    struct cldm_ht_entry *it0;
    char **it1;

    struct cldm_ht_entry entries[SIZE];

    char *strings[SIZE] = {
        "string 0",
        "aenima",
        "eon",
        "hash",
        "foo",
        "lat",
        "man",
        "tortoise"
    };

    struct cldm_ht ht = cldm_ht_init();

    cldm_for_each_zip(it0, it1, entries, strings) {
        *it0 = cldm_ht_mkentry_str(*it1);
    }

    cldm_for_each(it0, entries) {
        ASSERT_TRUE(cldm_ht_insert(&ht, it0));
    }
    cldm_ht_free(&ht);
}

TEST(cldm_ht_find_static) {
    enum { SIZE = 8 };
    struct cldm_ht_entry *it0;
    char **it1;

    struct cldm_ht_entry entries[SIZE];

    char *strings[SIZE] = {
        "string 0",
        "aenima",
        "eon",
        "hash",
        "foo",
        "lat",
        "man",
        "tortoise"
    };

    struct cldm_ht ht = cldm_ht_init();

    cldm_for_each_zip(it0, it1, entries, strings) {
        *it0 = cldm_ht_mkentry_str(*it1);
    }

    cldm_for_each_zip(it0, it1, entries, strings) {
        ASSERT_TRUE(cldm_ht_insert(&ht, it0));
        ASSERT_EQ(cldm_ht_find(&ht, &cldm_ht_mkentry_str(*it1)), it0);
    }

    cldm_ht_free(&ht);
}

TEST(cldm_ht_insert_dynamic) {
    enum { SIZE = 128 };
    enum { MAX_STRLEN = 32 };

    struct cldm_ht_entry entries[SIZE];
    char strings[SIZE][MAX_STRLEN];

    struct cldm_ht ht = cldm_ht_init();

    for(unsigned i = 0; i < cldm_arrsize(entries); i++) {
        snprintf(strings[i], sizeof(strings[i]), "string %u", i);
        entries[i] = cldm_ht_mkentry_str((char *)strings[i]);
    }

    for(unsigned i = 0; i < cldm_arrsize(entries); i++) {
        ASSERT_TRUE(cldm_ht_insert(&ht, &entries[i]));
        for(unsigned j = 0; j <= i; j++) {
            ASSERT_EQ(cldm_ht_find(&ht, &cldm_ht_mkentry_str((char *)strings[j])), &entries[j]);
        }
    }

    cldm_ht_free(&ht);
}

TEST(cldm_ht_insert_find_unsigned) {
    enum { SIZE = 2048 };

    struct cldm_ht_entry entries[SIZE];
    unsigned us[SIZE];

    struct cldm_ht ht = cldm_ht_init();

    for(unsigned i = 0; i < cldm_arrsize(us); i++) {
        us[i] = i;
        entries[i] = cldm_ht_mkentry(us[i]);
    }

    for(unsigned i = 0; i < cldm_arrsize(us); i++) {
        ASSERT_TRUE(cldm_ht_insert(&ht, &entries[i]));
        for(unsigned j = 0; j <= i; j++) {
            ASSERT_EQ(cldm_ht_find(&ht, &cldm_ht_mkentry(us[j])), &entries[j]);
        }
    }

    cldm_ht_free(&ht);
}

TEST(cldm_ht_remove) {
    enum { SIZE = 2048 };
    enum { MAX_STRLEN = 32 };

    struct cldm_ht_entry entries[SIZE];
    char strings[SIZE][MAX_STRLEN];

    struct cldm_ht ht = cldm_ht_init();

    for(unsigned i = 0; i < cldm_arrsize(entries); i++) {
        snprintf(strings[i], sizeof(strings[i]), "string %u", i);
        entries[i] = cldm_ht_mkentry_str((char *)strings[i]);
    }

    for(unsigned i = 0; i < cldm_arrsize(entries); i++) {
        ASSERT_TRUE(cldm_ht_insert(&ht, &entries[i]));
    }

    for(unsigned i = 0; i < cldm_arrsize(entries); i++) {
        ASSERT_EQ(cldm_ht_remove(&ht, &cldm_ht_mkentry_str((char *)strings[i])), &entries[i]);
        ASSERT_FALSE(cldm_ht_find(&ht, &entries[i]));
    }

    cldm_ht_free(&ht);
}

TEST(cldm_ht_size) {
    enum { SIZE = 2048 };

    struct cldm_ht_entry entries[SIZE];
    unsigned us[SIZE];

    struct cldm_ht ht = cldm_ht_init();

    for(unsigned i = 0; i < cldm_arrsize(entries); i++) {
        us[i] = i;
        entries[i] = cldm_ht_mkentry(us[i]);
    }

    for(unsigned i = 0; i < cldm_arrsize(entries); i++) {
        ASSERT_TRUE(cldm_ht_insert(&ht, &entries[i]));
        ASSERT_EQ(cldm_ht_size(&ht), i + 1);
    }

    for(unsigned i = 0; i < cldm_arrsize(entries); i++) {
        ASSERT_EQ(cldm_ht_remove(&ht, &cldm_ht_mkentry(us[i])), &entries[i]);
        ASSERT_EQ(cldm_ht_size(&ht), cldm_arrsize(entries) - i - 1);
    }

    cldm_ht_free(&ht);
}

TEST(cldm_ht_capacity_is_prime) {
    enum { SIZE = 16384 };
    struct cldm_ht_entry *entries = malloc(SIZE * sizeof(*entries));
    unsigned *us = malloc(SIZE * sizeof(*us));

    struct cldm_ht ht = cldm_ht_init();

    ASSERT_TRUE(entries);
    ASSERT_TRUE(us);

    for(unsigned i = 0; i < SIZE; i++) {
        us[i] = i;
        entries[i] = cldm_ht_mkentry(us[i]);
        cldm_ht_insert(&ht, &entries[i]);
        ASSERT_TRUE(cldm_is_prime(cldm_ht_capacity(&ht)));
    }

    free(us);
    free(entries);

    cldm_ht_free(&ht);
}
