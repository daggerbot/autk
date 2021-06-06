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

#ifndef AUTK_EXTERNAL_ICONV_H_INCLUDED
#define AUTK_EXTERNAL_ICONV_H_INCLUDED

#include "autk/string.h"

#include <cerrno>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>

#include <iconv.h>

namespace autk {

    namespace impl {

        // libiconv error type.
        class AUTK_HIDDEN Iconv_error : public Encoding_error {
        public:
            Iconv_error() = delete;
            Iconv_error(Iconv_error&&) = default;
            Iconv_error(const Iconv_error&) = default;
            explicit Iconv_error(std::string&& what_arg, int error_code);
            ~Iconv_error();

            Iconv_error& operator=(Iconv_error&&) = default;
            Iconv_error& operator=(const Iconv_error&) = default;
        };

        // Converts a string using libiconv.
        template<typename Out_char, typename In_char>
        std::basic_string<Out_char> iconv_convert(std::basic_string_view<In_char> str,
                                                  const char* in_encoding,
                                                  const char* out_encoding)
        {
            std::unique_ptr<std::remove_pointer_t<iconv_t>, decltype(&::iconv_close)> iconv{
                ::iconv_open(out_encoding, in_encoding), &::iconv_close};

            if (!iconv) {
                throw Iconv_error{"iconv_open failed", errno};
            }

            auto in_ptr = const_cast<char*>(reinterpret_cast<const char*>(str.data()));
            size_t in_size = str.size() * sizeof(In_char);
            char out_buf[512];
            char* out_ptr = out_buf;
            size_t out_size = sizeof(out_buf);
            std::basic_string<Out_char> out_str;

            while (in_size) {
                if (::iconv(iconv.get(), &in_ptr, &in_size, &out_ptr, &out_size) == size_t(-1)) {
                    throw Iconv_error{"iconv failed", errno};
                }

                size_t out_bytes = size_t(out_ptr - out_buf);
                size_t out_chars = out_bytes / sizeof(Out_char);

                if (out_chars) {
                    out_str.append(reinterpret_cast<Out_char*>(&out_buf[0]), out_chars);
                }
                if (out_bytes) {
                    std::memmove(out_buf,
                                 out_buf + out_chars * sizeof(Out_char),
                                 out_bytes - out_chars * sizeof(Out_char));
                }
            }

            return out_str;
        }

    } // namespace impl

} // namespace autk

#endif // AUTK_EXTERNAL_ICONV_H_INCLUDED
