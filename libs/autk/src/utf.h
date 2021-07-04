/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_UTF_H_INCLUDED
#define AUTK_UTF_H_INCLUDED

#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>

#include <autk/strings.h>
#include "unicode.h"

namespace autk {

    namespace impl {

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
        const char* strerror(Utf_status status) noexcept;

        // Raised when a UTF conversion fails.
        class Utf_error : public Encoding_error {
        public:
            Utf_error() = delete;
            Utf_error(Utf_error&&) = default;
            Utf_error(const Utf_error&) = default;
            explicit Utf_error(Utf_status status);
            explicit Utf_error(Utf_status status, std::size_t index);
            ~Utf_error();

            Utf_error& operator=(Utf_error&&) = default;
            Utf_error& operator=(const Utf_error&) = default;
        };

        // Return type for non-throwing UTF encode/decode operations.
        template<typename T>
        struct Utf_result {
            Utf_status status;
            T data;
            unsigned seq_len;
        };

        // Concept for types that can represent UTF-8 code units.
        template<typename T>
        concept Utf8_code_unit =
            (std::is_integral_v<T> && std::numeric_limits<T>::max() >= 0xFF)
            || (std::is_same_v<T, char> && CHAR_BIT >= 8);

        // Concept for types that can represent UTF-16 code units.
        template<typename T>
        concept Utf16_code_unit =
            std::is_integral_v<T> && std::numeric_limits<T>::max() >= 0xFFFF;

        // Concept for types that can represent UTF-32 code units.
        template<typename T>
        concept Utf32_code_unit =
            std::is_integral_v<T> && std::numeric_limits<T>::max() >= unicode::max_char;

        // UTF encoder/decoder.
        template<typename T, std::size_t Encoding = sizeof(T) * CHAR_BIT>
        struct Utf_codec;

//----------------------------------------------------------------------------------------------------------------------

        // UTF-8 encoder/decoder.
        template<Utf8_code_unit T>
        struct Utf_codec<T, 8> {
            static constexpr unsigned max_seq_len = 4;

            static Utf_result<char32_t> decode(std::basic_string_view<T> str)
            {
                if (str.empty()) {
                    return {Utf_status::empty, 0, 0};
                }

                auto get = [&str](unsigned i) -> std::uint8_t {
                    if constexpr(std::is_same_v<T, char>) {
                        // Simply converting char to unsigned char is not required by the spec to do what we would hope.
                        // However, converting char* to unsigned char* is well-defined and does not break strict
                        // aliasing rules.
                        return std::uint8_t(reinterpret_cast<const unsigned char*>(str.data())[i] & 0xFF);
                    } else {
                        return std::uint8_t(str[i] & 0xFF);
                    }
                };

                std::uint8_t unit = get(0);
                char32_t code_point;
                char32_t min_code_point;
                unsigned seq_len;

                // Decode the first code unit.
                if (!(unit & 0x80)) {
                    // 0xxxxxxx (ASCII)
                    return {Utf_status::ok, unit, 1};
                } else if ((unit & 0xE0) == 0xC0) {
                    // 110xxxxx 10xxxxxx
                    code_point = unit & 0x1F;
                    min_code_point = 0x80;
                    seq_len = 2;
                } else if ((unit & 0xF0) == 0xE0) {
                    // 1110xxxx 10xxxxxx 10xxxxxx
                    code_point = unit & 0x0F;
                    min_code_point = 0x800;
                    seq_len = 3;
                } else if ((unit & 0xF8) == 0xF0) {
                    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                    code_point = unit & 0x07;
                    min_code_point = 0x1'0000;
                    seq_len = 4;
                } else {
                    return {Utf_status::invalid, unicode::replacement_char, 1};
                }

                // Decode all remaining code units.
                for (unsigned index = 1; index < seq_len; ++index) {
                    if (str.size() == index) {
                        return {Utf_status::incomplete, unicode::replacement_char, index};
                    }
                    unit = get(index);
                    if ((unit & 0xC0) != 0x80) {
                        return {Utf_status::invalid, unicode::replacement_char, 1};
                    }
                    code_point = (code_point << 6) | (unit & 0x3F);
                }

                if (code_point > unicode::max_char) {
                    return {Utf_status::out_of_range, code_point, seq_len};
                } else if (code_point >= unicode::min_surrogate_char && code_point <= unicode::max_surrogate_char) {
                    return {Utf_status::reserved, code_point, seq_len};
                } else if (code_point < min_code_point) {
                    return {Utf_status::not_optimal, code_point, seq_len};
                }

                return {Utf_status::ok, code_point, seq_len};
            }

            static Utf_result<std::array<T, max_seq_len>> encode(char32_t code_point)
            {
                Utf_result<std::array<T, max_seq_len>> result = {Utf_status::ok, {}, 0};

                auto put = [&result](std::uint8_t unit) -> void {
                    if constexpr(std::is_same_v<T, char>) {
                        // Simply converting unsigned char to char is not required by the spec to do what we would hope.
                        // However, converting char* to unsigned char* is well-defined and does not break strict
                        // aliasing rules.
                        reinterpret_cast<unsigned char*>(result.data.data())[result.seq_len] = unit;
                    } else {
                        result.data[result.seq_len] = T(unit);
                    }
                    ++result.seq_len;
                };

                if (code_point <= 0x7F) {
                    // 0xxxxxxx (ASCII)
                    put(std::uint8_t(code_point));
                } else if (code_point <= 0x7FF) {
                    // 110xxxxx 10xxxxxx
                    put(std::uint8_t(0xC0 | (code_point >> 6)));
                    put(std::uint8_t(0x80 | (code_point & 0x3F)));
                } else if (code_point <= 0xFFFF) {
                    // 1110xxxx 10xxxxxx 10xxxxxx
                    if (code_point >= unicode::min_surrogate_char && code_point <= unicode::max_surrogate_char) {
                        result.status = Utf_status::reserved;
                    }
                    put(std::uint8_t(0xE0 | (code_point >> 12)));
                    put(std::uint8_t(0x80 | ((code_point >> 6) & 0x3F)));
                    put(std::uint8_t(0x80 | (code_point & 0x3F)));
                } else if (code_point <= 0x1F'FFFF) {
                    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                    if (code_point > unicode::max_char) {
                        result.status = Utf_status::out_of_range;
                    }
                    put(std::uint8_t(0xF0 | (code_point >> 18)));
                    put(std::uint8_t(0x80 | ((code_point >> 12) & 0x3F)));
                    put(std::uint8_t(0x80 | ((code_point >> 6) & 0x3F)));
                    put(std::uint8_t(0x80 | (code_point & 0x3F)));
                } else {
                    result = encode(unicode::replacement_char);
                    result.status = Utf_status::out_of_range;
                }

                return result;
            }
        };

//----------------------------------------------------------------------------------------------------------------------

        // UTF-16 encoder/decoder/
        template<Utf16_code_unit T>
        struct Utf_codec<T, 16> {
            static constexpr unsigned max_seq_len = 2;

            static Utf_result<char32_t> decode(std::basic_string_view<T> str)
            {
                if (str.empty()) {
                    return {Utf_status::empty, 0, 0};
                }

                std::uint16_t unit[2] = {std::uint16_t(str[0] & 0xFFFFu), 0};

                if ((unit[0] & 0xF800) != 0xD800) {
                    // Single-unit sequence.
                    return {Utf_status::ok, unit[0], 1};
                } else if (!(unit[0] & 0x0400)) {
                    // Possible surrogate pair sequence.
                    if (str.size() < 2) {
                        // Preserve unit[0] because it might just be an unpaired surrogate.
                        return {Utf_status::incomplete, unit[0], 1};
                    }
                    unit[1] = std::uint16_t(str[1] & 0xFFFFu);
                    if ((unit[1] & 0xFC00) != 0xDC00) {
                        return {Utf_status::unpaired_surrogate, unit[0], 1};
                    }
                    return {Utf_status::ok, ((char32_t(unit[0] & 0x03FF) << 10) + 0x1'0000) | unit[1], 2};
                } else {
                    return {Utf_status::unpaired_surrogate, unit[0], 1};
                }
            }

            static Utf_result<std::array<T, max_seq_len>> encode(char32_t code_point)
            {
                if (code_point <= 0xFFFFu) {
                    // Single-unit sequence.
                    if (code_point >= unicode::min_surrogate_char && code_point <= unicode::max_surrogate_char) {
                        return {Utf_status::reserved, {T(std::uint16_t(code_point))}, 1};
                    }
                    return {Utf_status::ok, {T(std::uint16_t(code_point))}, 1};
                } else if (code_point <= unicode::max_char) {
                    // Surrogate pair sequence.
                    return {Utf_status::ok, {T(std::uint16_t(0xD800 | ((code_point - 0x1'0000) >> 10))),
                                             T(std::uint16_t(0xDC00 | (code_point & 0x03FF)))}, 2};
                } else {
                    auto result = encode(unicode::replacement_char);
                    result.status = Utf_status::out_of_range;
                    return result;
                }
            }
        };

//----------------------------------------------------------------------------------------------------------------------

        template<Utf32_code_unit T>
        struct Utf_codec<T, 32> {
            static constexpr unsigned max_seq_len = 1;

            static Utf_result<char32_t> decode(std::basic_string_view<T> str)
            {
                if (str.empty()) {
                    return {Utf_status::empty, 0, 0};
                }

                auto unit = std::uint32_t(str[0] & 0xFFFF'FFFFul);

                if (unit > unicode::max_char) {
                    return {Utf_status::out_of_range, unit, 1};
                } else if (unit >= unicode::min_surrogate_char && unit <= unicode::max_surrogate_char) {
                    return {Utf_status::reserved, unit, 1};
                }

                return {Utf_status::ok, unit, 1};
            }

            static Utf_result<std::array<T, max_seq_len>> encode(char32_t code_point)
            {
                if (code_point > std::numeric_limits<T>::max()) {
                    return {Utf_status::out_of_range, {T(unicode::replacement_char)}, 1};
                } else if (code_point > unicode::max_char) {
                    return {Utf_status::out_of_range, {T(code_point)}, 1};
                } else if (code_point >= unicode::min_surrogate_char && code_point <= unicode::max_surrogate_char) {
                    return {Utf_status::reserved, {T(code_point)}, 1};
                }

                return {Utf_status::ok, {T(code_point)}, 1};
            }
        };

//----------------------------------------------------------------------------------------------------------------------

        namespace utf {

            // Converts from one UTF encoding to another.
            // Throws a Utf_error if an invalid input sequence is encountered.
            template<typename Out_char,
                    typename In_char,
                    typename Encoder = Utf_codec<Out_char>,
                    typename Decoder = Utf_codec<In_char>>
            std::basic_string<Out_char> convert(std::basic_string_view<In_char> str)
            {
                std::basic_string<Out_char> out_str;
                Utf_result<char32_t> decoded;
                Utf_result<std::array<Out_char, Encoder::max_seq_len>> encoded;
                std::size_t index = 0;

                while (index < str.size()) {
                    decoded = Decoder::decode(str.substr(index));
                    if (decoded.status != Utf_status::ok) {
                        throw Utf_error{decoded.status, index};
                    }
                    index += decoded.seq_len;
                    encoded = Encoder::encode(decoded.data);
                    out_str.append(encoded.data.data(), encoded.seq_len);
                }

                return out_str;
            }

            // Converts from one UTF encoding to another.
            // Substitutes unreadable sequences instead of throwing.
            template<typename Out_char,
                    typename In_char,
                    typename Encoder = Utf_codec<Out_char>,
                    typename Decoder = Utf_codec<In_char>>
            std::basic_string<Out_char> convert_lossy(std::basic_string_view<In_char> str)
            {
                std::basic_string<Out_char> out_str;
                Utf_result<char32_t> decoded;
                Utf_result<std::array<Out_char, Encoder::max_seq_len>> encoded;
                std::size_t index = 0;

                while (index < str.size()) {
                    decoded = Decoder::decode(str.substr(index));

                    switch (decoded.status) {
                    case Utf_status::ok:
                    case Utf_status::not_optimal:
                        break;
                    default:
                        decoded.data = unicode::replacement_char;
                        break;
                    }

                    index += decoded.seq_len;
                    encoded = Encoder::encode(decoded.data);
                    out_str.append(encoded.data.data(), encoded.seq_len);
                }

                return out_str;
            }

        } // namespace utf

    } // namespace impl

} // namespace autk

#endif // AUTK_UTF_H_INCLUDED
