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

#include "encoding.h"

#define STACK_BUF_SIZE 256

#define ASCII_MAX 0x7F
#define LATIN1_MAX 0xFF
#define UCS_MIN_SURROGATE 0xD800
#define UCS_MAX_SURROGATE 0xDFFF
#define UCS_REPLACEMENT 0xFFFD
#define UCS_MAX 0x10FFFF

typedef struct {
    const char *in_pos;
    size_t in_rem;
    char *out_pos; // NULL if we just want to count the output length
    size_t out_rem;
    size_t out_total;
} char_to_char_state_t;

typedef struct {
    const char *in_pos;
    size_t in_rem;
    uint16_t *out_pos; // NULL if we just want to count the output length
    size_t out_rem;
    size_t out_total;
} char_to_uint16_state_t;

static int
decode_utf8_sequence(const char *seq, size_t len, uint32_t *out_codepoint)
{
    uint8_t byte;
    int seq_len;
    uint32_t codepoint;
    uint32_t min_codepoint;

    if (!len) {
        *out_codepoint = 0;
        return 0;
    }

    byte = (uint8_t)seq[0];
    if (!(byte & 0x80)) {
        *out_codepoint = byte;
        return 1;
    } else if ((byte & 0xE0) == 0xC0) {
        codepoint = byte & 0x1F;
        seq_len = 2;
        min_codepoint = 0x80;
    } else if ((byte & 0xF0) == 0xE0) {
        codepoint = byte & 0x0F;
        seq_len = 3;
        min_codepoint = 0x800;
    } else if ((byte & 0xF8) == 0xF0) {
        codepoint = byte & 0x07;
        seq_len = 4;
        min_codepoint = 0x10000;
    } else {
        *out_codepoint = UCS_REPLACEMENT;
        return -1; // Invalid first byte
    }

    for (int i = 1; i < seq_len; i++) {
        if ((unsigned int)i == len) {
            *out_codepoint = UCS_REPLACEMENT;
            return -i; // Incomplete sequence
        }
        byte = (uint8_t)seq[i];
        if ((byte & 0xC0) != 0x80) {
            *out_codepoint = UCS_REPLACEMENT;
            return -i; // Invalid continuation byte
        }
        codepoint = (codepoint << 6) | (byte & 0x3F);
    }

    if ((codepoint >= UCS_MIN_SURROGATE && codepoint <= UCS_MAX_SURROGATE) || codepoint > UCS_MAX) {
        *out_codepoint = UCS_REPLACEMENT;
        return -seq_len; // Invalid code point
    } else if (codepoint < min_codepoint) {
        *out_codepoint = codepoint; // Overlong encoding, but we'll decode it anyway
        return -seq_len; // Still technically invalid
    }

    *out_codepoint = codepoint;
    return seq_len;
}

static unsigned int
encode_utf16_sequence(uint32_t codepoint, uint16_t *out_seq, size_t out_rem)
{
    if ((codepoint >= UCS_MIN_SURROGATE && codepoint <= UCS_MAX_SURROGATE) || codepoint > UCS_MAX) {
        codepoint = UCS_REPLACEMENT;
    }

    if (codepoint <= 0xFFFF) {
        if (out_seq) {
            if (out_rem < 1) {
                return 0; // Not enough space
            }
            *out_seq = (uint16_t)codepoint;
        }
        return 1;
    } else {
        if (out_seq) {
            if (out_rem < 2) {
                return 0; // Not enough space
            }
            codepoint -= 0x10000;
            out_seq[0] = (uint16_t)(0xD800 | (codepoint >> 10));
            out_seq[1] = (uint16_t)(0xDC00 | (codepoint & 0x3FF));
        }
        return 2;
    }
}

static autk_status_t
convert_utf8_to_latin1(char_to_char_state_t *state, autk_encoding_flags_t flags,
                       char replacement_char)
{
    int decode_result;
    uint32_t codepoint;

    while (state->in_rem) {
        // Stop if the output buffer is full.
        if (state->out_pos && state->out_rem == 0) {
            return AUTK_ERR_INSUFFICIENT_BUFFER;
        }

        // Decode the next UTF-8 sequence.
        decode_result = decode_utf8_sequence(state->in_pos, state->in_rem, &codepoint);

        // Fail or fallback if the code point can't be represented in Latin-1.
        if (decode_result < 0 || codepoint > LATIN1_MAX) {
            if (flags & AUTK_ENCODING_FLAGS_LOSSY) {
                codepoint = (unsigned char)replacement_char;
            } else {
                return AUTK_ERR_INVALID_STRING_ENCODING;
            }
        }

        // Advance the input position.
        if (decode_result >= 0) {
            state->in_pos += (unsigned int)decode_result;
            state->in_rem -= (unsigned int)decode_result;
        } else {
            state->in_pos += (unsigned int)-decode_result;
            state->in_rem -= (unsigned int)-decode_result;
        }

        // Advance the output position.
        if (state->out_pos) {
            *state->out_pos++ = (char)(unsigned char)codepoint;
            state->out_rem--;
        }
        state->out_total++;
    }

    return AUTK_OK;
}

AUTK_HIDDEN autk_status_t
autk_with_utf8_to_latin1(autk_instance_t *instance, const char *in_str, int in_len, void *ctx,
                         void (*callback)(void *ctx, const char *out_str, size_t out_len),
                         autk_encoding_flags_t flags, char replacement_char)
{
    char stack_buf[STACK_BUF_SIZE];
    const char *out_str = stack_buf;
    autk_status_t status;
    char_to_char_state_t state = {
        .in_pos = in_str,
        .in_rem = in_len >= 0 ? (size_t)in_len : (in_str ? strlen(in_str) : 0),
        .out_pos = stack_buf,
        .out_rem = sizeof(stack_buf) - 1,
    };
    size_t heap_buf_size = 0;
    char *heap_buf = NULL;

    // Convert as much as possible into the stack buffer.
    status = convert_utf8_to_latin1(&state, flags, replacement_char);
    if (status != AUTK_OK && status != AUTK_ERR_INSUFFICIENT_BUFFER) {
        return status;
    }

    // Check if we need to allocate a heap buffer for the remaining output.
    if (status == AUTK_ERR_INSUFFICIENT_BUFFER) {
        char_to_char_state_t tmp_state = {
            .in_pos = state.in_pos,
            .in_rem = state.in_rem,
            .out_total = state.out_total,
        };

        // Calculate the total output length needed.
        status = convert_utf8_to_latin1(&tmp_state, flags, replacement_char);
        if (status != AUTK_OK) {
            return status;
        }

        // Allocate a heap buffer for the remaining output.
        heap_buf_size = tmp_state.out_total + 1;
        heap_buf = autk_instance_alloc(instance, NULL, 0, heap_buf_size, AUTK_MEMORY_TAG_STRING);
        if (heap_buf) {
            // Copy the already converted part from the stack buffer to the heap buffer.
            memcpy(heap_buf, stack_buf, state.out_total);

            // Finish conversion into the heap buffer.
            state.out_pos = heap_buf + state.out_total;
            state.out_rem = heap_buf_size - state.out_total - 1;
            status = convert_utf8_to_latin1(&state, flags, replacement_char);
            if (status != AUTK_OK) {
                autk_instance_alloc(instance, heap_buf, heap_buf_size, 0, AUTK_MEMORY_TAG_STRING);
                return status;
            }

            // Commit to using the heap buffer.
            out_str = heap_buf;
        } else if (!(flags & AUTK_ENCODING_FLAGS_TRUNCATE_ON_ALLOC_FAILURE)) {
            return AUTK_ERR_OUT_OF_MEMORY;
        }
    }

    // Null-terminate the output string.
    *state.out_pos = 0;

    // Call the callback with the converted string.
    callback(ctx, out_str, state.out_total);

    // Clean up.
    if (heap_buf) {
        autk_instance_alloc(instance, heap_buf, heap_buf_size, 0, AUTK_MEMORY_TAG_STRING);
    }

    return AUTK_OK;
}

static autk_status_t
convert_utf8_to_utf16(char_to_uint16_state_t *state, autk_encoding_flags_t flags)
{
    int decode_result;
    uint32_t codepoint;
    unsigned int encode_result;

    while (state->in_rem) {
        // Decode the next UTF-8 sequence.
        decode_result = decode_utf8_sequence(state->in_pos, state->in_rem, &codepoint);
        if (decode_result < 0) {
            if (flags & AUTK_ENCODING_FLAGS_LOSSY) {
                decode_result = -decode_result;
            } else {
                return AUTK_ERR_INVALID_STRING_ENCODING;
            }
        }

        // Encode the code point as UTF-16.
        encode_result = encode_utf16_sequence(codepoint, state->out_pos, state->out_rem);
        if (encode_result == 0) {
            return AUTK_ERR_INSUFFICIENT_BUFFER;
        }

        // Advance the input position.
        state->in_pos += (unsigned int)decode_result;
        state->in_rem -= (unsigned int)decode_result;

        // Advance the output position.
        if (state->out_pos) {
            state->out_pos += encode_result;
            state->out_rem -= encode_result;
        }
        state->out_total += encode_result;
    }

    return AUTK_OK;
}

AUTK_HIDDEN autk_status_t
autk_with_utf8_to_utf16(autk_instance_t *instance, const char *in_str, int in_len, void *ctx,
                        void (*callback)(void *ctx, const uint16_t *out_str, size_t out_len),
                        autk_encoding_flags_t flags)
{
    uint16_t stack_buf[STACK_BUF_SIZE];
    const uint16_t *out_str = stack_buf;
    autk_status_t status;
    char_to_uint16_state_t state = {
        .in_pos = in_str,
        .in_rem = in_len >= 0 ? (size_t)in_len : (in_str ? strlen(in_str) : 0),
        .out_pos = stack_buf,
        .out_rem = sizeof(stack_buf) / sizeof(uint16_t),
    };
    size_t heap_buf_size = 0;
    uint16_t *heap_buf = NULL;

    // Convert as much as possible into the stack buffer.
    status = convert_utf8_to_utf16(&state, flags);
    if (status != AUTK_OK && status != AUTK_ERR_INSUFFICIENT_BUFFER) {
        return status;
    }

    // Check if we need to allocate a heap buffer for the remaining output.
    if (status == AUTK_ERR_INSUFFICIENT_BUFFER) {
        char_to_uint16_state_t tmp_state = {
            .in_pos = state.in_pos,
            .in_rem = state.in_rem,
            .out_total = state.out_total,
        };

        // Calculate the total output length needed.
        status = convert_utf8_to_utf16(&tmp_state, flags);
        if (status != AUTK_OK) {
            return status;
        }

        // Allocate a heap buffer for the remaining output.
        heap_buf_size = (tmp_state.out_total + 1) * sizeof(uint16_t);
        heap_buf = autk_instance_alloc(instance, NULL, 0, heap_buf_size, AUTK_MEMORY_TAG_STRING);
        if (heap_buf) {
            // Copy the already converted part from the stack buffer to the heap buffer.
            memcpy(heap_buf, stack_buf, state.out_total * sizeof(uint16_t));

            // Finish conversion into the heap buffer.
            state.out_pos = heap_buf + state.out_total;
            state.out_rem = (heap_buf_size / sizeof(uint16_t)) - state.out_total - 1;
            status = convert_utf8_to_utf16(&state, flags);
            if (status != AUTK_OK) {
                autk_instance_alloc(instance, heap_buf, heap_buf_size, 0, AUTK_MEMORY_TAG_STRING);
                return status;
            }

            // Commit to using the heap buffer.
            out_str = heap_buf;
        } else if (!(flags & AUTK_ENCODING_FLAGS_TRUNCATE_ON_ALLOC_FAILURE)) {
            return AUTK_ERR_OUT_OF_MEMORY;
        }
    }

    // Null-terminate the output string.
    *state.out_pos = 0;

    // Call the callback with the converted string.
    callback(ctx, out_str, state.out_total);

    // Clean up.
    if (heap_buf) {
        autk_instance_alloc(instance, heap_buf, heap_buf_size, 0, AUTK_MEMORY_TAG_STRING);
    }

    return AUTK_OK;
}
