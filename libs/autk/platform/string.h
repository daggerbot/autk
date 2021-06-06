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

#ifndef AUTK_PLATFORM_STRING_H_INCLUDED
#define AUTK_PLATFORM_STRING_H_INCLUDED

#include "autk/string.h"
#include "autk/types.h"

#include <climits>
#include <array>
#include <limits>
#include <optional>
#include <type_traits>

namespace autk {

    namespace impl {

        inline constexpr char32_t min_surrogate_char = 0xD800;
        inline constexpr char32_t max_surrogate_char = 0xDFFF;
        inline constexpr char32_t replacement_char = 0xFFFD;
        inline constexpr char32_t unicode_max = 0x10'FFFF;

        // Error code for UTF encode/decode operations.
        enum class Utf_status {
            ok,
            empty,
            not_optimal,
            unpaired_surrogate,
            reserved,
            incomplete,
            out_of_range,
            invalid,
        };

        // Returns a string describing a Utf_status.
        AUTK_HIDDEN std::string strerror(Utf_status status);

        // UTF exception typ.
        class AUTK_HIDDEN Utf_error : public Encoding_error {
        public:
            Utf_error();
            Utf_error(Utf_error&&) = default;
            Utf_error(const Utf_error&) = default;
            explicit Utf_error(Utf_status status);
            explicit Utf_error(Utf_status status, std::size_t index);
            ~Utf_error();

            std::optional<std::size_t> index() const { return index_; }
            Utf_status status() const { return status_; }

            Utf_error& operator=(Utf_error&&) = default;
            Utf_error& operator=(const Utf_error&) = default;

        private:
            Utf_status status_ = {};
            std::optional<std::size_t> index_;
        };

        // Return value for UTF encode/decode operations.
        template<typename T>
        struct Utf_result {
            Utf_status status;
            T data;
            unsigned seq_len;
        };

        // Concept for types that can represent UTF-8 code units.
        template<typename T>
        concept Utf8_code_unit =
            std::is_integral_v<T>
            && ((std::numeric_limits<T>::min() <= 0 && std::numeric_limits<T>::max() >= 0xFF)
                || (std::is_same_v<T, char> && CHAR_BIT >= 8));

        // Concept for types that can represent UTF-16 code units.
        template<typename T>
        concept Utf16_code_unit =
            std::is_integral_v<T>
            && std::numeric_limits<T>::min() <= 0
            && std::numeric_limits<T>::max() >= 0xFFFF;

        // Concept for types that can represent UTF-32 code units.
        template<typename T>
        concept Utf32_code_unit =
            std::is_integral_v<T>
            && std::numeric_limits<T>::min() <= 0
            && std::numeric_limits<T>::max() >= unicode_max;

        // UTF encoder/decoder.
        template<typename T, std::size_t Encoding = sizeof(T) * CHAR_BIT>
        struct Utf_codec;

        // UTF-8 encoder/decoder.
        template<Utf8_code_unit T>
        struct Utf_codec<T, 8> : No_inst {
            static constexpr unsigned max_seq_len = 4;

            // Decodes a UTF-8 sequence at the front of the input string.
            static Utf_result<char32_t> decode(std::basic_string_view<T> str)
            {
                if (str.empty()) {
                    return {Utf_status::empty, 0, 0};
                }

                auto get = [&str](unsigned i) -> std::uint8_t {
                    if constexpr(std::is_same_v<T, char>) {
                        // Simply casting char to unsigned char is technically undefined behavior, even if
                        // mainstream compilers do what we would expect. However, casting char* to unsigned
                        // char* is well-defined and doees not break strict aliasing rules.
                        return reinterpret_cast<const unsigned char*>(str.data())[i] & 0xFF;
                    } else {
                        return std::uint8_t(str[i] & 0xFF);
                    }
                };

                std::uint8_t unit;
                std::uint32_t ch;
                std::uint32_t min_ch;
                unsigned seq_len;

                unit = get(0);

                if (!(unit & 0x80)) {
                    // 0xxxxxxx (ASCII)
                    return {Utf_status::ok, unit, 1};
                } else if ((unit & 0xE0) == 0xC0) {
                    // 110xxxxx 10xxxxxx
                    ch = unit & 0x1F;
                    min_ch = 0x80;
                    seq_len = 2;
                } else if ((unit & 0xF0) == 0xE0) {
                    // 1110xxxx 10xxxxxx 10xxxxxx
                    ch = unit & 0x0F;
                    min_ch = 0x800;
                    seq_len = 3;
                } else if ((unit & 0xF8) == 0xF0) {
                    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                    ch = unit & 0x07;
                    min_ch = 0x1'0000;
                    seq_len = 4;
                } else {
                    return {Utf_status::invalid, replacement_char, 1};
                }

                for (unsigned i = 1; i < seq_len; ++i) {
                    if (i == str.size()) {
                        return {Utf_status::incomplete, replacement_char, 1};
                    }
                    unit = get(i);
                    if ((unit & 0xC0) != 0x80) {
                        return {Utf_status::invalid, replacement_char, 1};
                    }
                    ch = (ch << 6) | (unit & 0x3F);
                }

                if (ch > unicode_max) {
                    return {Utf_status::out_of_range, replacement_char, seq_len};
                } else if (ch >= min_surrogate_char && ch <= max_surrogate_char) {
                    return {Utf_status::reserved, ch, seq_len};
                } else if (ch < min_ch) {
                    return {Utf_status::not_optimal, ch, seq_len};
                }

                return {Utf_status::ok, ch, seq_len};
            }

            // Encodes a UTF-8 sequence.
            static Utf_result<std::array<T, max_seq_len>> encode(char32_t ch)
            {
                Utf_result<std::array<T, max_seq_len>> result = {Utf_status::ok, {}, 0};

                auto put = [&result](std::uint8_t unit) {
                    if constexpr(std::is_same_v<T, char>) {
                        // Simply casting unsigned char to char is technically undefined behavior, even if
                        // mainstream compilers do what we would expect. However, casting char* to unsigned
                        // char* is well-defined and doees not break strict aliasing rules.
                        reinterpret_cast<unsigned char*>(result.data.data())[result.seq_len] = unit;
                    } else {
                        result.data[result.seq_len] = T(unit);
                    }
                    ++result.seq_len;
                };

                if (ch <= 0x7F) {
                    // 0xxxxxxx (ASCII)
                    put(std::uint8_t(ch));
                } else if (ch <= 0x7FF) {
                    // 110xxxxx 10xxxxxx
                    put(std::uint8_t(0xC0 | (ch >> 6)));
                    put(std::uint8_t(0x80 | (ch & 0x3F)));
                } else if (ch <= 0xFFFF) {
                    // 1110xxxx 10xxxxxx 10xxxxxx
                    if (ch >= min_surrogate_char && ch <= max_surrogate_char) {
                        result.status = Utf_status::reserved;
                    }
                    put(std::uint8_t(0xE0 | (ch >> 12)));
                    put(std::uint8_t(0x80 | ((ch >> 6) & 0x3F)));
                    put(std::uint8_t(0x80 | (ch & 0x3F)));
                } else if (ch <= unicode_max) {
                    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                    put(std::uint8_t(0xF0 | (ch >> 18)));
                    put(std::uint8_t(0x80 | ((ch >> 12) & 0x3F)));
                    put(std::uint8_t(0x80 | ((ch >> 6) & 0x3F)));
                    put(std::uint8_t(0x80 | (ch & 0x3F)));
                } else {
                    result = encode(replacement_char);
                    result.status = Utf_status::out_of_range;
                }

                return result;
            }
        };

        // UTF-16 encoder/decoder.
        template<Utf16_code_unit T>
        struct Utf_codec<T, 16> : No_inst {
            static constexpr unsigned max_seq_len = 2;

            // Decodes a UTF-16 sequence at the front of the input string.
            static Utf_result<char32_t> decode(std::basic_string_view<T> str)
            {
                if (str.empty()) {
                    return {Utf_status::empty, 0, 0};
                }

                auto unit = std::uint16_t(str[0] & 0xFFFF);

                if ((unit & 0xF800) != 0xD800) {
                    // Single-unit sequence.
                    return {Utf_status::ok, unit, 1};
                } else if (!(unit & 0x0400)) {
                    // Surrogate pair sequence.
                    if (str.size() < 2) {
                        return {Utf_status::incomplete, unit, 1};
                    }
                    auto ch = (std::uint32_t(unit & 0x03FF) << 10) + 0x1'0000;
                    unit = std::uint16_t(str[1] & 0xFFFF);
                    if ((unit & 0xFC00) != 0xDC00) {
                        return {Utf_status::unpaired_surrogate, std::uint16_t(str[0] & 0xFFFF), 1};
                    }
                    ch |= unit & 0x03FF;
                    return {Utf_status::ok, ch, 2};
                } else {
                    return {Utf_status::unpaired_surrogate, unit, 1};
                }
            }

            // Encodes a UTF-16 sequence.
            static Utf_result<std::array<T, max_seq_len>> encode(char32_t ch)
            {
                if (ch <= 0xFFFF) {
                    // Single-unit sequence.
                    if (ch >= min_surrogate_char && ch <= max_surrogate_char) {
                        return {Utf_status::reserved, {T(std::uint16_t(ch))}, 1};
                    }
                    return {Utf_status::ok, {T(std::uint16_t(ch))}, 1};
                } else if (ch <= unicode_max) {
                    // Surrogate pair sequence.
                    return {Utf_status::ok, {T(std::uint16_t(0xD800 | ((ch - 0x1'0000) >> 10))),
                                             T(std::uint16_t(0xDC00 | (ch & 0x03FF)))}, 2};
                } else {
                    auto result = encode(replacement_char);
                    result.status = Utf_status::out_of_range;
                    return result;
                }
            }
        };

        // UTF-32 encoder/decoder.
        template<Utf32_code_unit T>
        struct Utf_codec<T, 32> {
            static constexpr unsigned max_seq_len = 1;

            // Decodes a UTF-32 sequence at the front of the input string.
            static Utf_result<char32_t> decode(std::basic_string_view<T> str)
            {
                if (str.empty()) {
                    return {Utf_status::empty, 0, 0};
                }

                auto ch = char32_t(str[0] & 0xFFFFFFFFul);

                if (ch > unicode_max) {
                    return {Utf_status::out_of_range, replacement_char, 1};
                } else if (ch >= min_surrogate_char && ch <= max_surrogate_char) {
                    return {Utf_status::reserved, ch, 1};
                }

                return {Utf_status::ok, ch, 1};
            }

            // Encodes a UTF-32 sequence.
            static Utf_result<std::array<T, max_seq_len>> encode(char32_t ch)
            {
                if (ch > unicode_max) {
                    return {Utf_status::out_of_range, {T(replacement_char)}, 1};
                } else if (ch >= min_surrogate_char && ch <= max_surrogate_char) {
                    return {Utf_status::reserved, {T(ch)}, 1};
                }

                return {Utf_status::ok, {T(ch)}, 1};
            }
        };

        // Converts a string from one UTF encoding to another.
        // Throws an exception if any invalid input is encountered.
        template<typename Out_char,
                 typename In_char,
                 typename Encoder = Utf_codec<Out_char>,
                 typename Decoder = Utf_codec<In_char>>
        std::basic_string<Out_char> utf_convert(std::basic_string_view<In_char> str)
        {
            std::basic_string<Out_char> out_str;
            std::size_t index = 0;

            while (!str.empty()) {
                auto decoded = Decoder::decode(str);
                if (decoded.status != Utf_status::ok) {
                    throw Utf_error{decoded.status, index};
                }
                str = str.substr(decoded.seq_len);
                index += decoded.seq_len;
                auto encoded = Encoder::encode(decoded.data);
                out_str.append(encoded.data.data(), encoded.seq_len);
            }

            return out_str;
        }

        // Converts a string from one UTF encoding to another.
        // Attempts to substitute invalid sequences instead of throwing.
        template<typename Out_char,
                 typename In_char,
                 typename Encoder = Utf_codec<Out_char>,
                 typename Decoder = Utf_codec<In_char>>
        std::basic_string<Out_char> utf_convert_lossy(std::basic_string_view<In_char> str)
        {
            std::basic_string<Out_char> out_str;
            std::size_t index = 0;

            while (!str.empty()) {
                auto decoded = Decoder::decode(str);

                switch (decoded.status) {
                case Utf_status::unpaired_surrogate:
                case Utf_status::reserved:
                    decoded.data = replacement_char;
                    break;
                default:
                    break;
                }

                str = str.substr(decoded.seq_len);
                index += decoded.seq_len;
                auto encoded = Encoder::encode(decoded.data);
                out_str.append(encoded.data.data(), encoded.seq_len);
            }

            return out_str;
        }

    } // namespace impl

} // namespace autk

#endif // AUTK_PLATFORM_STRING_H_INCLUDED
