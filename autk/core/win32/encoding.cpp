/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include <autk/core/convert.h>
#include <autk/core/encoding.h>
#include <autk/core/utf.h>
#include "platform.h"

using namespace autk;
using namespace std::literals::string_literals;

namespace {

    std::wstring acp_to_wide_char(std::string_view str)
    {
        if (str.empty()) {
            return {};
        }

        ::SetLastError(0);
        int result = ::MultiByteToWideChar(CP_ACP, 0, str.data(), convert::int_cast<int>(str.size()), nullptr, 0);
        if (result <= 0) {
            throw EncodingError{"MultiByteToWideChar: "s + win32::strerror(::GetLastError())};
        }

        std::unique_ptr<wchar_t[]> buf{new wchar_t[result]};
        result = ::MultiByteToWideChar(CP_ACP, 0, str.data(), int(str.size()), buf.get(), result);
        if (result <= 0) {
            throw EncodingError{"MultiByteToWideChar: "s + win32::strerror(::GetLastError())};
        }

        return std::wstring{buf.get(), size_t(result)};
    }

    std::string wide_char_to_acp(std::wstring_view str)
    {
        if (str.empty()) {
            return {};
        }

        ::SetLastError(0);
        int result = ::WideCharToMultiByte(CP_ACP, 0, str.data(), convert::int_cast<int>(str.size()), nullptr, 0,
                                           nullptr, nullptr);
        if (result <= 0) {
            throw EncodingError{"WideCharToMultiByte: "s + win32::strerror(::GetLastError())};
        }

        std::unique_ptr<char[]> buf{new char[result]};
        result = ::WideCharToMultiByte(CP_ACP, 0, str.data(), int(str.size()), buf.get(), result, nullptr, nullptr);
        if (result <= 0) {
            throw EncodingError{"WideCharToMultiByte: "s + win32::strerror(::GetLastError())};
        }

        return std::string{buf.get(), size_t(result)};
    }

} // namespace

std::string encoding::to_sys(std::u8string_view str)
{
    return ::wide_char_to_acp(encoding::to_wsys(str));
}

std::string encoding::to_sys_lossy(std::u8string_view str)
{
    return ::wide_char_to_acp(encoding::to_wsys_lossy(str));
}

std::u8string encoding::to_utf8(std::string_view str)
{
    return encoding::to_utf8(::acp_to_wide_char(str));
}

std::u8string encoding::to_utf8(std::wstring_view str)
{
    std::u8string out_str;
    utf::convert_range<wchar_t, char8_t>(str, [&out_str](char8_t ch) { out_str.push_back(ch); });
    return out_str;
}

std::u8string encoding::to_utf8_lossy(std::string_view str)
{
    return encoding::to_utf8_lossy(::acp_to_wide_char(str));
}

std::u8string encoding::to_utf8_lossy(std::wstring_view str)
{
    std::u8string out_str;
    utf::convert_range_lossy<wchar_t, char8_t>(str, [&out_str](char8_t ch) { out_str.push_back(ch); });
    return out_str;
}

std::wstring encoding::to_wsys(std::u8string_view str)
{
    std::wstring out_str;
    utf::convert_range<char8_t, wchar_t>(str, [&out_str](wchar_t ch) { out_str.push_back(ch); });
    return out_str;
}

std::wstring encoding::to_wsys_lossy(std::u8string_view str)
{
    std::wstring out_str;
    utf::convert_range_lossy<char8_t, wchar_t>(str, [&out_str](wchar_t ch) { out_str.push_back(ch); });
    return out_str;
}
