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

#ifndef AUTK_STYLE_H_
#define AUTK_STYLE_H_

#include "types.h"

AUTK_API extern const autk_style_class_t *const autk_style_class_default;

AUTK_BEGIN_DECLS

AUTK_API autk_status_t
autk_style_create(autk_instance_t *instance, const autk_style_create_params_t *params,
                  autk_style_t **out_style);

AUTK_API autk_style_t *
autk_style_retain(autk_style_t *style);

AUTK_API void
autk_style_release(autk_style_t *style);

AUTK_API void *
autk_style_get_class_data(autk_style_t *style);

AUTK_API autk_instance_t *
autk_style_get_instance(autk_style_t *style);

AUTK_API void *
autk_style_get_user_data(autk_style_t *style);

AUTK_API const autk_extension_header_t *
autk_style_find_extension(autk_style_t *style, const autk_extension_query_t *query);

AUTK_API bool
autk_style_class_provides_extension(const autk_style_class_t *klass,
                                    const autk_extension_query_t *query);

AUTK_END_DECLS

#endif // AUTK_STYLE_H_
