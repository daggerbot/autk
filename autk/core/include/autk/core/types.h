/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_TYPES_H_INCLUDED
#define AUTK_CORE_TYPES_H_INCLUDED

#include <cstddef>
#include <cstdint>

#include <autk/config.h>

/// \def AUTK_T
/// Encodes a string literal as an `autk::tchar_t` array.
/// \ingroup core
#ifdef _WIN32
# define AUTK_T(x) L##x
#else
# define AUTK_T(x) x
#endif

namespace autk {

    /// \addtogroup core
    /// @{

    using nullptr_t = std::nullptr_t;
    using ptrdiff_t = std::ptrdiff_t;
    using size_t = std::size_t;

    using int8_t = std::int8_t;
    using int16_t = std::int16_t;
    using int32_t = std::int32_t;
    using int64_t = std::int64_t;
    using intptr_t = std::intptr_t;

    using uint8_t = std::uint8_t;
    using uint16_t = std::uint16_t;
    using uint32_t = std::uint32_t;
    using uint64_t = std::uint64_t;
    using uintptr_t = std::uintptr_t;

    /// \typedef tchar_t
    /// Preferred character type when using strings in the system encoding.
    /// This is `wchar_t` on Windows and `char` on all other platforms.
#ifdef _WIN32
    using tchar_t = wchar_t;
#else
    using tchar_t = char;
#endif

    /// Function pointer typedef with clearer (but more verbose) syntax than raw C function pointers.
    template<typename R, typename...Args>
    using FunctionPointer = R (*)(Args...);

    /// Member function pointer typedef with clearer (but more verbose) syntax than raw C++ member function pointers.
    /// Note that the return type is the first parameter and the target is second.
    template<typename R, typename T, typename...Args>
    using MemberFunctionPointer = R (T::*)(Args...);

    /// @}

} // namespace autk

#endif // AUTK_CORE_TYPES_H_INCLUDED
