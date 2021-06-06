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

#include "autk/platform/string.h"
#include "autk/config.h"

#if AUTK_ICONV
# include "autk/external/iconv.h"
#endif

using namespace autk;
using namespace autk::impl;

Os_string autk::to_os_string(std::u8string_view str)
{
#if AUTK_ICONV
    return impl::iconv_convert<char>(str, "UTF-8", "char");
#else
    return impl::utf_convert<char, char8_t, Utf_codec<char, 8>, Utf_codec<char8_t, 8>>(str);
#endif
}

Os_string autk::to_os_string_lossy(std::u8string_view str)
{
#if AUTK_ICONV
    return impl::iconv_convert<char>(str, "UTF-8", "char//TRANSLIT//IGNORE");
#else
    return impl::utf_convert_lossy<char, char8_t, Utf_codec<char, 8>, Utf_codec<char8_t, 8>>(str);
#endif
}

std::u8string autk::to_utf8(std::string_view str)
{
#if AUTK_ICONV
    return impl::iconv_convert<char8_t>(str, "char", "UTF-8");
#else
    return impl::utf_convert<char8_t, char, Utf_codec<char8_t, 8>, Utf_codec<char, 8>>(str);
#endif
}

std::u8string autk::to_utf8(std::wstring_view str)
{
#if AUTK_ICONV
    return impl::iconv_convert<char8_t>(str, "wchar_t", "UTF-8");
#else
    return impl::utf_convert<char8_t, wchar_t, Utf_codec<char8_t, 8>>(str);
#endif
}

std::u8string autk::to_utf8_lossy(std::string_view str)
{
#if AUTK_ICONV
    return impl::iconv_convert<char8_t>(str, "char", "UTF-8//TRANSLIT//IGNORE");
#else
    return impl::utf_convert_lossy<char8_t, char, Utf_codec<char8_t, 8>, Utf_codec<char, 8>>(str);
#endif
}

std::u8string autk::to_utf8_lossy(std::wstring_view str)
{
#if AUTK_ICONV
    return impl::iconv_convert<char8_t>(str, "wchar_t", "UTF-8//TRANSLIT//IGNORE");
#else
    return impl::utf_convert_lossy<char8_t, wchar_t, Utf_codec<char8_t, 8>>(str);
#endif
}
