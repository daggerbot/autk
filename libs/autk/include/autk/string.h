// Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef AUTK_STRING_H_INCLUDED
#define AUTK_STRING_H_INCLUDED

#include <exception>
#include <string>

#include "autk/config.h"
#include "autk/types.h"

namespace autk {

    using Os_string = std::basic_string<Os_char>;
    using Os_string_view = std::basic_string_view<Os_char>;

    /// Converts a UTF-8 string to the preferred encoding of the underlying OS APIs.
    /// \throw Encoding_error if the conversion fails.
    AUTK_IMPORT Os_string to_os_string(std::u8string_view str);

    /// Converts a UTF-8 string to the preferred encoding of the underlying OS APIs.
    /// Attempts to substitute invalid sequences instead of throwing.
    AUTK_IMPORT Os_string to_os_string_lossy(std::u8string_view str);

    /// Converts a system narrow string to UTF-8.
    /// \throw Encoding_error if the conversion fails.
    AUTK_IMPORT std::u8string to_utf8(std::string_view str);

    /// Converts a system wide string to UTF-8.
    /// \throw Encoding_error if the conversion fails.
    AUTK_IMPORT std::u8string to_utf8(std::wstring_view str);

    /// Converts a system narrow string to UTF-8.
    /// Attempts to substitute invalid sequences instead of throwing.
    AUTK_IMPORT std::u8string to_utf8_lossy(std::string_view str);

    /// Converts a system wide string to UTF-8.
    /// Attempts to substitute invalid sequences instead of throwing.
    AUTK_IMPORT std::u8string to_utf8_lossy(std::wstring_view str);

    /// Raised when a text conversion fails.
    class AUTK_IMPORT Encoding_error : public std::exception {
    public:
        Encoding_error();
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

} // namespace autk

#endif // AUTK_STRING_H_INCLUDED
