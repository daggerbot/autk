/*
 * Copyright (c) 2026 Martin Mills
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef AUTK_UTILITY_HASH_H_
#define AUTK_UTILITY_HASH_H_

#include <autk/types.h>

typedef uintptr_t autk_hash_t;
typedef autk_hash_t (*autk_hash_func_t)(const void *key);
typedef bool (*autk_hash_eq_func_t)(const void *key0, const void *key1);

typedef struct autk_hash_iter autk_hash_iter_t;
typedef struct autk_hash_table autk_hash_table_t;

struct autk_hash_table {
    autk_instance_t *instance; // for allocation
    size_t element_size;
    autk_hash_func_t hash_func;
    autk_hash_eq_func_t eq_func;
    size_t bucket_size;
    size_t bucket_count;
    size_t used_count;
    size_t worst_miss;
    char *data;
};

struct autk_hash_iter {
    size_t index;
};

AUTK_HIDDEN void
autk_hash_table_init(autk_instance_t *instance, autk_hash_table_t *ht, size_t element_size,
                     autk_hash_func_t hash_func, autk_hash_eq_func_t eq_func);

AUTK_HIDDEN void
autk_hash_table_fini(autk_hash_table_t *ht);

AUTK_HIDDEN bool
autk_hash_table_find(const autk_hash_table_t *ht, const void *key, autk_hash_iter_t *out_iter);

AUTK_HIDDEN void *
autk_hash_table_get(autk_hash_table_t *ht, autk_hash_iter_t iter);

AUTK_HIDDEN autk_status_t
autk_hash_table_reserve(autk_hash_table_t *ht, size_t min_count);

// Does not motify the existing element if the key was found (*inserted == false). If the caller
// needs to update the data, it should use `autk_hash_table_get()` to get a pointer to the existing
// data.
AUTK_HIDDEN autk_status_t
autk_hash_table_insert(autk_hash_table_t *ht, const void *key, autk_hash_iter_t *out_iter,
                       bool *inserted);

AUTK_HIDDEN void *
autk_hash_table_remove(autk_hash_table_t *ht, const void *key);

AUTK_HIDDEN void *
autk_hash_table_remove_iter(autk_hash_table_t *ht, autk_hash_iter_t iter);

#endif // AUTK_UTILITY_HASH_H_
