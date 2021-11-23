/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_ENCODING_H_INCLUDED
#define AUTK_CORE_ENCODING_H_INCLUDED

#include <string>

#include "exception.h"

namespace autk {

    /// \addtogroup core
    /// @{

    /// Exception raised when improperly encoded text is encountered.
    class AUTK_CORE EncodingError : public InvalidData {
    public:
        EncodingError(const EncodingError& other) noexcept = default;

        explicit EncodingError(std::string&& what_arg)
            : InvalidData{std::move(what_arg)}
        {
        }

        EncodingError& operator=(const EncodingError& other) noexcept = default;
    };

    /// Functions for converting text.
    namespace encoding {

        /// Converts a UTF-8 string to the narrow system encoding.
        /// \throw `EncodingError` if the input string is invalid.
        AUTK_CORE std::string to_sys(std::u8string_view str);

        /// Converts a UTF-8 string to the narrow system encoding.
        /// Attempts to preserve invalid data from the input string instead of throwing.
        AUTK_CORE std::string to_sys_lossy(std::u8string_view str);

        /// Converts a string from the narrow system encoding to UTF-8.
        /// \throw `EncodingError` if the input string is invalid.
        AUTK_CORE std::u8string to_utf8(std::string_view str);

        /// Converts a string from the wide system encoding to UTF-8.
        /// \throw `EncodingError` if the input string is invalid.
        AUTK_CORE std::u8string to_utf8(std::wstring_view str);

        /// Converts a string from the narrow system encoding to UTF-8.
        /// Attempts to preserve invalid data from the input string instead of throwing.
        AUTK_CORE std::u8string to_utf8_lossy(std::string_view str);

        /// Converts a string from the wide system encoding to UTF-8.
        /// Attempts to preserve invalid data from the input string instead of throwing.
        AUTK_CORE std::u8string to_utf8_lossy(std::wstring_view str);

        /// Converts a UTF-8 string to the wide system encoding.
        /// \throw `EncodingError` if the input string is invalid.
        AUTK_CORE std::wstring to_wsys(std::u8string_view str);

        /// Converts a UTF-8 string to the wide system encoding.
        /// Attempts to preserve invalid data from the input string instead of throwing.
        AUTK_CORE std::wstring to_wsys_lossy(std::u8string_view str);

        /// Converts a UTF-8 string to the preferred system encoding.
        /// \throw `EncodingError` if the input string is invalid.
        inline std::basic_string<tchar_t> to_tsys(std::u8string_view str)
        {
#ifdef _WIN32
            return encoding::to_wsys(str);
#else
            return encoding::to_sys(str);
#endif
        }

        /// Converts a UTF-8 string to the preferred system encoding.
        /// Attempts to preserve invalid data from the input string instead of throwing.
        inline std::basic_string<tchar_t> to_tsys_lossy(std::u8string_view str)
        {
#ifdef _WIN32
            return encoding::to_wsys_lossy(str);
#else
            return encoding::to_sys_lossy(str);
#endif
        }

    } // namespace encoding

    /// @}

} // namespace autk

#endif // AUTK_CORE_ENCODING_H_INCLUDED
