/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_UNICODE_H_INCLUDED
#define AUTK_CORE_UNICODE_H_INCLUDED

#include "types.h"

namespace autk {

    /// \addtogroup core
    /// @{

    /// Unicode constants.
    namespace unicode {

        inline constexpr char32_t min_surrogate_char = 0xD800;
        inline constexpr char32_t max_surrogate_char = 0xDFFF;
        inline constexpr char32_t replacement_char = 0xFFFD;
        inline constexpr char32_t max_char = 0x10'FFFF;

    } // namespace unicode

    /// @}

} // namespace autk

#endif // AUTK_CORE_UNICODE_H_INCLUDED
