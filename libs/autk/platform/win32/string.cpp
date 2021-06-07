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

#include <windows.h>

#include <system_error>

#include "autk/platform/string.h"

using namespace autk;
using namespace autk::impl;

namespace {

    std::wstring acp_to_wstring(std::string_view str)
    {
        if (str.empty()) {
            return {};
        }

        ::SetLastError(0);
        auto result = ::MultiByteToWideChar(CP_ACP, 0, str.data(), int(str.size()), nullptr, 0);
        if (result <= 0) {
            throw std::system_error{int(::GetLastError()), std::system_category(), "MultiByteToWideChar failed"};
        }

        std::wstring out_str(std::size_t(result), 0);
        result = ::MultiByteToWideChar(CP_ACP, 0, str.data(), int(str.size()), out_str.data(), result);
        if (result <= 0) {
            throw std::system_error{int(::GetLastError()), std::system_category(), "MultiByteToWideChar failed"};
        }

        out_str.resize(std::size_t(result));
        return out_str;
    }

} // namespace

//----------------------------------------------------------------------------------------------------------------------

Os_string autk::to_os_string(std::u8string_view str)
{
    return impl::utf_convert<wchar_t, char8_t, Utf_codec<wchar_t>, Utf_codec<char8_t, 8>>(str);
}

Os_string autk::to_os_string_lossy(std::u8string_view str)
{
    return impl::utf_convert_lossy<wchar_t, char8_t, Utf_codec<wchar_t>, Utf_codec<char8_t, 8>>(str);
}

std::u8string autk::to_utf8(std::string_view str)
{
    return autk::to_utf8(::acp_to_wstring(str));
}

std::u8string autk::to_utf8(std::wstring_view str)
{
    return impl::utf_convert<char8_t, wchar_t, Utf_codec<char8_t, 8>, Utf_codec<wchar_t>>(str);
}

std::u8string autk::to_utf8_lossy(std::string_view str)
{
    return autk::to_utf8_lossy(::acp_to_wstring(str));
}

std::u8string autk::to_utf8_lossy(std::wstring_view str)
{
    return impl::utf_convert_lossy<char8_t, wchar_t, Utf_codec<char8_t, 8>, Utf_codec<wchar_t>>(str);
}
