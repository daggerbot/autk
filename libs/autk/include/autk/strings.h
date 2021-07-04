/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_STRINGS_H_INCLUDED
#define AUTK_STRINGS_H_INCLUDED

#include <exception>
#include <string>

#include "types.h"

/// \def AUTK_T
/// Encodes the following string literal as an autk::oschar_t array.
#ifdef _WIN32
# define AUTK_T L""
#else
# define AUTK_T
#endif

namespace autk {

    /// \typedef oschar_t
    /// Preferred character type when using the underlying platform APIs.
    /// This is `wchar_t` on Windows or `char` on all other platforms.
    /// \see AUTK_T to encode a string literal as an oschar_t array.
#ifdef _WIN32
    using oschar_t = wchar_t;
#else
    using oschar_t = char;
#endif

    using Os_string = std::basic_string<oschar_t>;
    using Os_string_view = std::basic_string_view<oschar_t>;

    /// Raised when a string conversion fails.
    class AUTK Encoding_error : public std::exception {
    public:
        Encoding_error() = delete;
        Encoding_error(Encoding_error&&) = default;
        Encoding_error(const Encoding_error&) = default;
        explicit Encoding_error(std::string&& what_arg);
        ~Encoding_error();

        const char* what() const noexcept override { return what_.c_str(); }

        Encoding_error& operator=(Encoding_error&&) = default;
        Encoding_error& operator=(const Encoding_error&) = default;

    private:
        std::string what_;
    };

    /// Converts a UTF-8 string to an Os_string.
    /// \throw Encoding_error if the conversion fails.
    AUTK Os_string to_os_string(std::u8string_view str);

    /// Converts a UTF-8 string to an Os_string.
    /// Attempts to substitute or omit sequences that cannot be converted instead of throwing.
    AUTK Os_string to_os_string_lossy(std::u8string_view str);

    /// Converts an Os_string_view to UTF-8.
    /// \throw Encoding_error if the conversion fails.
    AUTK std::u8string to_utf8(Os_string_view str);

    /// Converts an Os_string_view to UTF-8.
    /// Attempts to substitute or omit sequences that cannot be converted instead of throwing.
    AUTK std::u8string to_utf8_lossy(Os_string_view str);

} // namespace autk

#endif // AUTK_STRINGS_H_INCLUDED
