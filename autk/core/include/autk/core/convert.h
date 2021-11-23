/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_CONVERT_H_INCLUDED
#define AUTK_CORE_CONVERT_H_INCLUDED

#include <concepts>
#include <limits>
#include <type_traits>

#include "exception.h"

namespace autk {

    /// Conversion functions.
    namespace convert {

        /// Checked integer version.
        /// \throw `OverflowError` or `UnderflowError` on failure.
        template<std::integral Out, std::integral In>
        constexpr Out int_cast(In in)
        {
            if constexpr(std::is_signed_v<In>) {
                if constexpr(std::is_signed_v<Out>) {
                    if constexpr(std::numeric_limits<In>::digits > std::numeric_limits<Out>::digits) {
                        if (in < In(std::numeric_limits<Out>::min())) {
                            throw UnderflowError{"Integer underflow"};
                        }
                    }
                } else if (in < 0) {
                    throw UnderflowError{"Integer underflow"};
                }
            }

            if constexpr(std::numeric_limits<In>::digits > std::numeric_limits<Out>::digits) {
                if (in > In(std::numeric_limits<Out>::max())) {
                    throw OverflowError{"Integer overflow"};
                }
            }

            return Out(in);
        }

    } // namespace convert

} // namespace autk

#endif // AUTK_CORE_CONVERT_H_INCLUDED
