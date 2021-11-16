/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_CONCEPTS_H_INCLUDED
#define AUTK_CORE_CONCEPTS_H_INCLUDED

#include <concepts>
#include <type_traits>

#include "types.h"

namespace autk {

    /// \addtogroup core
    /// @{

    /// Concept for types that can be trivially forwarded without `std::forward`.
    template<typename T>
    concept TriviallyForwardable = std::is_trivially_copyable_v<T> || std::is_reference_v<T>;

    /// @}

} // namespace autk

#endif // AUTK_CORE_CONCEPTS_H_INCLUDED
