/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <autk/config.h>

#include <cerrno>
#include <cstring>
#include <memory>
#include <type_traits>

#if AUTK_ICONV
# include <iconv.h>
#endif

#include <autk/core/encoding.h>
#include <autk/core/utf.h>

using namespace autk;
using namespace std::literals::string_literals;

namespace {

#if AUTK_ICONV
    template<typename OutChar, typename InChar>
    std::basic_string<OutChar> iconv_convert(std::basic_string_view<InChar> in_str, const char* in_encoding,
                                             const char* out_encoding)
    {
        std::unique_ptr<std::remove_pointer_t<iconv_t>, decltype(&::iconv_close)> iconv
            {::iconv_open(out_encoding, in_encoding), &::iconv_close};

        if (!iconv) {
            throw RuntimeError{"iconv_open(): "s + std::strerror(errno)};
        }

        const char* in_ptr = reinterpret_cast<const char*>(in_str.data());
        size_t in_size = in_str.size() * sizeof(InChar);
        OutChar out_buf[512 / sizeof(OutChar)];
        char* out_ptr = reinterpret_cast<char*>(&out_buf[0]);
        size_t out_size = sizeof(out_buf);
        std::basic_string<OutChar> out_str;

        while (in_size) {
            if (::iconv(iconv.get(), const_cast<char**>(&in_ptr), &in_size, &out_ptr, &out_size) == size_t(-1)) {
                switch (errno) {
                case EILSEQ:
                case EINVAL:
                    throw EncodingError{"iconv(): "s + std::strerror(errno)};
                default:
                    throw RuntimeError{"iconv(): "s + std::strerror(errno)};
                }
            }

            size_t avail_bytes = out_size - sizeof(out_buf);
            size_t avail_chars = avail_bytes / sizeof(OutChar);

            if (avail_chars) {
                out_str.append(out_buf, avail_chars);
                avail_bytes -= avail_chars * sizeof(OutChar);
                out_ptr = reinterpret_cast<char*>(&out_buf[avail_bytes]);
                out_size = sizeof(out_buf) - avail_bytes;
                std::memmove(out_buf, out_buf + avail_chars, avail_bytes);
            }
        }

        return out_str;
    }
#endif // AUTK_ICONV

} // namespace

std::string encoding::to_sys(std::u8string_view str)
{
#if AUTK_ICONV
    return ::iconv_convert<char>(str, "UTF-8", "char");
#else
    std::string out_str;
    utf::convert_range<char8_t, char>(str, [&out_str](char ch) { out_str.push_back(ch); });
    return out_str;
#endif
}

std::string encoding::to_sys_lossy(std::u8string_view str)
{
#if AUTK_ICONV
    return ::iconv_convert<char>(str, "UTF-8", "char//TRANSLIT//IGNORE");
#else
    return std::string{reinterpret_cast<const char*>(str.data()), str.size()};
#endif
}

std::u8string encoding::to_utf8(std::string_view str)
{
#if AUTK_ICONV
    return ::iconv_convert<char8_t>(str, "char", "UTF-8");
#else
    std::u8string out_str;
    utf::convert_range<char, char8_t>(str, [&out_str](char8_t ch) { out_str.push_back(ch); });
    return out_str;
#endif
}

std::u8string encoding::to_utf8(std::wstring_view str)
{
#if AUTK_ICONV
    return ::iconv_convert<char8_t>(str, "wchar_t", "UTF-8");
#else
    std::u8string out_str;
    utf::convert_range<wchar_t, char8_t>(str, [&out_str](char8_t ch) { out_str.push_back(ch); });
    return out_str;
#endif
}

std::u8string encoding::to_utf8_lossy(std::string_view str)
{
#if AUTK_ICONV
    return ::iconv_convert<char8_t>(str, "char", "UTF-8//TRANSLIT//IGNORE");
#else
    std::u8string out_str;
    utf::convert_range_lossy<char, char8_t>(str, [&out_str](char8_t ch) { out_str.push_back(ch); });
    return out_str;
#endif
}

std::u8string encoding::to_utf8_lossy(std::wstring_view str)
{
#if AUTK_ICONV
    return ::iconv_convert<char8_t>(str, "wchar_t", "UTF-8//TRANSLIT//IGNORE");
#else
    std::u8string out_str;
    utf::convert_range_lossy<wchar_t, char8_t>(str, [&out_str](char8_t ch) { out_str.push_back(ch); });
    return out_str;
#endif
}

std::wstring encoding::to_wsys(std::u8string_view str)
{
#if AUTK_ICONV
    return ::iconv_convert<wchar_t>(str, "UTF-8", "wchar_t");
#else
    std::wstring out_str;
    utf::convert_range<char8_t, wchar_t>(str, [&out_str](wchar_t ch) { out_str.push_back(ch); });
    return out_str;
#endif
}

std::wstring encoding::to_wsys_lossy(std::u8string_view str)
{
#if AUTK_ICONV
    return ::iconv_convert<wchar_t>(str, "UTF-8", "wchar_t//TRANSLIT//IGNORE");
#else
    std::wstring out_str;
    utf::convert_range_lossy<char8_t, wchar_t>(str, [&out_str](wchar_t ch) { out_str.push_back(ch); });
    return out_str;
#endif
}
