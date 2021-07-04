/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_UNICODE_H_INCLUDED
#define AUTK_UNICODE_H_INCLUDED

#include <autk/types.h>

namespace autk {

    namespace impl {

        namespace unicode {

            inline constexpr char32_t min_surrogate_char = 0xD800;
            inline constexpr char32_t max_surrogate_char = 0xDFFF;
            inline constexpr char32_t replacement_char = 0xFFFD;
            inline constexpr char32_t max_char = 0x10'FFFF;

        } // namespace unicode

    } // namespace impl

} // namespace autk

#endif // AUTK_UNICODE_H_INCLUDED
