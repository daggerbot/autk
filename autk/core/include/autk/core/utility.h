/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_UTILITY_H_INCLUDED
#define AUTK_CORE_UTILITY_H_INCLUDED

#include <concepts>
#include <type_traits>
#include <utility>

#include "types.h"

namespace autk {

    /// \addtogroup core
    /// @{

    /// Object which invokes a callback from its destructor.
    /// This is used to simulate the `finally` keyword found in other languages such as Java and C\#.
    template<std::invocable Callback>
    class Finally {
    public:
        Finally() = delete;
        Finally(Finally<Callback>&&) = delete;
        Finally(const Finally<Callback>&) = delete;

        /// Constructs a `Finally` by copying a callback.
        constexpr Finally(const Callback& callback)
        requires std::is_copy_constructible_v<Callback>
            : callback_{callback}
        {
        }

        /// Constructs a `Finally` by moving a callback.
        constexpr Finally(Callback&& callback)
        requires std::is_move_constructible_v<Callback>
            : callback_{std::move(callback)}
        {
        }

        /// Forwards `args` into the callback's constructor.
        template<typename...Args>
        constexpr explicit Finally(Args&&...args)
        requires std::is_constructible_v<Callback, Args...>
            : callback_{std::forward<Args>(args)...}
        {
        }

        /// Invokes the callback.
        /// Note that this will result in a call to `std::terminate()` if the callback throws.
        ~Finally()
        {
            callback_();
        }

        Finally<Callback>& operator=(Finally<Callback>&&) = delete;
        Finally<Callback>& operator=(const Finally<Callback>&) = delete;

    private:
        Callback callback_;
    };

    /// @}

} // namespace autk

#endif // AUTK_CORE_UTILITY_H_INCLUDED
