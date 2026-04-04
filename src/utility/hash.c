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

#include <assert.h>
#include <string.h>

#include <autk/instance.h>

#include "hash.h"

#define HASH_EMPTY 0
#define HASH_TOMBSTONE 1
#define HASH_MAX ((autk_hash_t) - 1ll)
#define HASH_MAX_BIT (HASH_MAX ^ (HASH_MAX >> 1))
#define HASH_OCCUPIED_BIT HASH_MAX_BIT
#define HASH_VALUE_MASK (HASH_MAX ^ HASH_OCCUPIED_BIT)
#define HASH_ALIGNMENT 8 // for bucket headers and for element data
#define BUCKET_OVERHEAD HASH_ALIGNMENT
#define MIN_BUFFER_BYTES 256

static size_t
align_up(size_t n)
{
    return ((n - 1) | (HASH_ALIGNMENT - 1)) + 1;
}

static size_t
next_pow_2(size_t n)
{
    static_assert(sizeof(n) == 4 || sizeof(n) == 8,
                  "Need to update next_pow_2() for this platform");

    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    if (sizeof(n) >= 8) {
        n |= n >> 32;
    }
    return n + 1;
}

AUTK_HIDDEN void
autk_hash_table_init(autk_instance_t *instance, autk_hash_table_t *ht, size_t element_size,
                     autk_hash_func_t hash_func, autk_hash_eq_func_t eq_func)
{
    *ht = (autk_hash_table_t){
        .instance = instance,
        .element_size = element_size,
        .hash_func = hash_func,
        .eq_func = eq_func,
        .bucket_size = BUCKET_OVERHEAD + align_up(element_size),
    };
}

AUTK_HIDDEN void
autk_hash_table_fini(autk_hash_table_t *ht)
{
    if (ht->data) {
        autk_instance_alloc(ht->instance, ht->data, ht->bucket_size * ht->bucket_count, 0,
                            AUTK_MEMORY_TAG_HASH);
        ht->bucket_count = 0;
        ht->used_count = 0;
        ht->worst_miss = 0;
        ht->data = NULL;
    }
}

AUTK_HIDDEN bool
autk_hash_table_find(const autk_hash_table_t *ht, const void *key, autk_hash_iter_t *out_iter)
{
    autk_hash_t hash;
    size_t index;
    autk_hash_t *bucket;
    const void *element;

    if (ht->used_count == 0) {
        return false;
    }

    // Determine where to begin the search.
    hash = ht->hash_func(key);
    index = hash % ht->bucket_count;
    hash |= HASH_OCCUPIED_BIT;

    // Perform the search.
    for (size_t i = 0; i <= ht->worst_miss; i++) {
        bucket = (autk_hash_t *)(ht->data + index * ht->bucket_size);
        if (*bucket == HASH_EMPTY) {
            // Stop the search early if we reach an empty bucket.
            return false;
        } else if (*bucket == hash) {
            // Cached hash matches. Check if the keys match.
            element = (char *)bucket + BUCKET_OVERHEAD;
            if (ht->eq_func(key, element)) {
                // We have a match.
                out_iter->index = index;
                return true;
            }
        }

        // Increment and possibly wrap the search.
        if (++index == ht->bucket_count) {
            index = 0;
        }
    }

    return false;
}

AUTK_HIDDEN void *
autk_hash_table_get(autk_hash_table_t *ht, autk_hash_iter_t iter)
{
    autk_hash_t *bucket;

    if (iter.index >= ht->bucket_count) {
        return NULL;
    }

    bucket = (autk_hash_t *)(ht->data + iter.index * ht->bucket_size);
    if (!(*bucket & HASH_OCCUPIED_BIT)) {
        return NULL;
    }

    return (char *)bucket + BUCKET_OVERHEAD;
}

static bool
do_insert(autk_hash_table_t *ht, const void *key, autk_hash_t hash, autk_hash_iter_t *out_iter)
{
    size_t index;
    autk_hash_t *bucket;
    autk_hash_t *first_tombstone = NULL;
    size_t first_tombstone_index = 0;
    void *element;
    size_t i;

    // Determine where to begin the search.
    hash |= HASH_OCCUPIED_BIT;
    index = (hash & HASH_VALUE_MASK) % ht->bucket_count;

    // Perform the search.
    for (i = 0; i <= ht->worst_miss; i++) {
        bucket = (autk_hash_t *)(ht->data + index * ht->bucket_size);
        if (*bucket == HASH_EMPTY) {
            // Empty bucket found. Insert here.
            *bucket = hash;
            element = (char *)bucket + BUCKET_OVERHEAD;
            memcpy(element, key, ht->element_size);
            if (out_iter) {
                out_iter->index = index;
            }
            ht->used_count++;
            return true;
        } else if (*bucket == HASH_TOMBSTONE) {
            // We can use this bucket if the key is not found, but we first have to keep searching
            // until `worst_miss` is reached.
            if (!first_tombstone) {
                first_tombstone = bucket;
                first_tombstone_index = index;
            }
        } else if (*bucket == hash) {
            // Cached hash matches. Check if the keys match.
            element = (char *)bucket + BUCKET_OVERHEAD;
            if (ht->eq_func(key, element)) {
                // We have a match.
                if (out_iter) {
                    out_iter->index = index;
                }
                return false;
            }
        }

        // Increment and possibly wrap the search.
        if (++index == ht->bucket_count) {
            index = 0;
        }
    }

    // If we reach here, we've reached `worst_miss`.
    // If we've encountered any tombstone, we can use it.
    if (first_tombstone) {
        *first_tombstone = hash;
        element = (char *)first_tombstone + BUCKET_OVERHEAD;
        memcpy(element, key, ht->element_size);
        if (out_iter) {
            out_iter->index = first_tombstone_index;
        }
        ht->used_count++;
        return true;
    }

    // If we reach here, we have to keep looking for any unoccupied bucket and update `worst_miss`.
    for (;; i++) {
        bucket = (autk_hash_t *)(ht->data + index * ht->bucket_size);
        if (!(*bucket & HASH_OCCUPIED_BIT)) {
            // We finally found an unoccupied bucket!
            *bucket = hash;
            element = (char *)bucket + BUCKET_OVERHEAD;
            memcpy(element, key, ht->element_size);
            if (out_iter) {
                out_iter->index = index;
            }
            ht->used_count++;
            ht->worst_miss = i;
            return true;
        }

        // Increment and possibly wrap the search.
        if (++index == ht->bucket_count) {
            index = 0;
        }
    }
}

AUTK_HIDDEN autk_status_t
autk_hash_table_reserve(autk_hash_table_t *ht, size_t min_count)
{
    size_t min_bucket_count;
    autk_hash_table_t old_ht;
    size_t min_buf_size;
    size_t new_buf_size;
    size_t new_bucket_count;
    char *new_buf;
    autk_hash_t *bucket;
    const void *element;

    // Skip if the buffer is already large enough.
    if (min_count > SIZE_MAX / 4) {
        return AUTK_ERR_ARITHMETIC_OVERFLOW;
    }
    min_bucket_count = min_count * 4 / 3;
    if (ht->bucket_count >= min_bucket_count) {
        return AUTK_OK;
    }

    // Determine the actual size of the new buffer.
    if (min_bucket_count > SIZE_MAX / ht->bucket_size) {
        return AUTK_ERR_ARITHMETIC_OVERFLOW;
    }
    min_buf_size = min_bucket_count * ht->bucket_size;
    new_buf_size = next_pow_2(min_buf_size);
    if (new_buf_size == 0) {
        return AUTK_ERR_ARITHMETIC_OVERFLOW;
    } else if (new_buf_size < MIN_BUFFER_BYTES) {
        new_buf_size = MIN_BUFFER_BYTES;
    }
    new_bucket_count = new_buf_size / ht->bucket_size;

    // Don't request any excess bytes, or else we'd have to track a separate `alloc_size` field for
    // the allocator.
    new_buf_size = new_bucket_count * ht->bucket_size;

    // Allocate the new buffer.
    new_buf = autk_instance_alloc(ht->instance, NULL, 0, new_buf_size, AUTK_MEMORY_TAG_HASH);
    if (!new_buf) {
        return AUTK_ERR_OUT_OF_MEMORY;
    }
    memset(new_buf, 0, new_buf_size);

    // Move the old table to a temporary location and update the main struct to the new one.
    old_ht = *ht;
    ht->bucket_count = new_bucket_count;
    ht->used_count = 0;
    ht->worst_miss = 0;
    ht->data = new_buf;

    // Reinsert all elements into the new table.
    bucket = (autk_hash_t *)ht->data;
    for (size_t i = 0; i < old_ht.bucket_count; i++) {
        // We can stop early if we know there are no valid elements left.
        if (ht->used_count == old_ht.used_count) {
            break;
        }

        // Reinsert the element if this bucket is occupied.
        if (*bucket & HASH_OCCUPIED_BIT) {
            element = (char *)bucket + BUCKET_OVERHEAD;
            if (!do_insert(ht, element, *bucket, NULL)) {
                // The table should never contain multiple copies of the same key.
                // If this happens, revert the table to its prior state.
                autk_instance_alloc(ht->instance, new_buf, new_buf_size, 0, AUTK_MEMORY_TAG_HASH);
                *ht = old_ht;
                return AUTK_ERR_DATA_CORRUPTION;
            }
        }

        // Advance to the next bucket.
        bucket = (autk_hash_t *)((char *)bucket + old_ht.bucket_size);
    }

    // Success!
    autk_instance_alloc(ht->instance, old_ht.data, old_ht.bucket_size * old_ht.bucket_count, 0,
                        AUTK_MEMORY_TAG_HASH);
    return AUTK_OK;
}

AUTK_HIDDEN autk_status_t
autk_hash_table_insert(autk_hash_table_t *ht, const void *key, autk_hash_iter_t *out_iter,
                       bool *out_inserted)
{
    autk_status_t status;
    bool inserted;

    // Make sure the buffer is large enough for a new element, even if we don't end up inserting a
    // new one. This is the most convenient place to put this check while only having it in one
    // place.
    status = autk_hash_table_reserve(ht, ht->used_count + 1);
    if (status != AUTK_OK) {
        return status;
    }

    // Insert the element.
    inserted = do_insert(ht, key, ht->hash_func(key), out_iter);
    if (out_inserted) {
        *out_inserted = inserted;
    }
    return AUTK_OK;
}

AUTK_HIDDEN void *
autk_hash_table_remove(autk_hash_table_t *ht, const void *key)
{
    autk_hash_iter_t iter;

    if (!autk_hash_table_find(ht, key, &iter)) {
        return NULL;
    }
    return autk_hash_table_remove_iter(ht, iter);
}

AUTK_HIDDEN void *
autk_hash_table_remove_iter(autk_hash_table_t *ht, autk_hash_iter_t iter)
{
    autk_hash_t *bucket;
    void *element;

    if (iter.index >= ht->bucket_count) {
        return NULL;
    }

    bucket = (autk_hash_t *)(ht->data + iter.index * ht->bucket_size);
    element = (char *)bucket + BUCKET_OVERHEAD;
    if (!(*bucket & HASH_OCCUPIED_BIT)) {
        return NULL;
    }

    *bucket = HASH_TOMBSTONE;
    ht->used_count--;
    return element;
}

AUTK_HIDDEN bool
autk_hash_table_begin(const autk_hash_table_t *ht, autk_hash_iter_t *iter)
{
    autk_hash_t *bucket;

    if (ht->used_count == 0) {
        return false;
    }

    for (size_t i = 0; i < ht->bucket_count; i++) {
        bucket = (autk_hash_t *)(ht->data + i * ht->bucket_size);
        if (*bucket & HASH_OCCUPIED_BIT) {
            iter->index = i;
            return true;
        }
    }

    // Should never be reached unless the table is corrupted.
    return false;
}

AUTK_HIDDEN bool
autk_hash_table_next(const autk_hash_table_t *ht, autk_hash_iter_t *iter)
{
    autk_hash_t *bucket;

    for (size_t i = iter->index + 1; i < ht->bucket_count; i++) {
        bucket = (autk_hash_t *)(ht->data + i * ht->bucket_size);
        if (*bucket & HASH_OCCUPIED_BIT) {
            iter->index = i;
            return true;
        }
    }

    return false;
}
