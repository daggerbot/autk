/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_UTF_H_INCLUDED
#define AUTK_CORE_UTF_H_INCLUDED

#include <climits>
#include <concepts>
#include <iterator>
#include <limits>
#include <optional>
#include <ranges>
#include <tuple>
#include <type_traits>

#include "encoding.h"
#include "iterator.h"
#include "unicode.h"

namespace autk {

    /// \addtogroup core
    /// @{

    /// Error code for UTF conversions.
    enum class UtfStatus {
        /// The operationw as successful.
        ok,
        /// The input range is empty.
        empty,
        /// The UTF-8 sequence should have been encoded in fewer bytes.
        not_optimal,
        /// A UTF-16 surrogate code unit is not part of a matching pair.
        unpaired_surrogate,
        /// The code point is reserved for UTF-16 surrogates.
        reserved,
        /// The input range ends before finishing the sequence.
        incomplete,
        /// The code point exceeds the maximum Unicode scalar.
        out_of_range,
        /// The sequence was not understood.
        invalid,
    };

    /// Returns a string describing a `UtfStatus`.
    AUTK_CORE const char* strerror(UtfStatus status) noexcept;

    /// Exception raised when a UTF conversion fails.
    class AUTK_CORE UtfError : public EncodingError {
    public:
        UtfError(const UtfError& other) noexcept = default;
        explicit UtfError(UtfStatus status);
        explicit UtfError(UtfStatus status, size_t index);

        UtfError& operator=(const UtfError& other) noexcept = default;
    };

    /// Concept for types that can represent UTF-8 code units.
    template<typename T>
    concept Utf8CodeUnit = (std::is_integral_v<T> && std::numeric_limits<T>::max() >= 0xFF)
                           || (std::is_same_v<T, char> && CHAR_BIT >= 8);

    /// Concept for types that can represent UTF-16 code units.
    template<typename T>
    concept Utf16CodeUnit = std::is_integral_v<T> && std::numeric_limits<T>::max() >= 0xFFFF;

    /// Concept for types that can represent UTF-32 code units.
    template<typename T>
    concept Utf32CodeUnit = std::is_integral_v<T> && std::numeric_limits<T>::max() >= unicode::max_char;

    /// UTF encoder/decoder.
    template<typename T, size_t Encoding = sizeof(T) * CHAR_BIT>
    struct UtfCodec;

    /// Concept for types that can decode UTF sequences from a given iterator type.
    template<typename T, typename Iter>
    concept UtfDecoder =
        std::forward_iterator<Iter>
        && std::is_same_v<typename T::char_type, std::iter_value_t<Iter>>
        && requires {
            { T::decode(std::declval<Iter&>(), std::declval<const Iter&>()) }
                -> std::same_as<std::tuple<UtfStatus, char32_t>>;
        };

    /// Concept for types that can encode UTF sequences using a given callback type.
    template<typename T, typename Callback>
    concept UtfEncoder =
        std::invocable<Callback, typename T::char_type>
        && requires {
            { T::encode(std::declval<char32_t>(), std::declval<Callback&>()) } -> std::same_as<UtfStatus>;
        };

    /// UTF conversion functions.
    namespace utf {

        /// Converts a range of text from one UTF encoding to another.
        /// \throw UtfError if the conversion fails.
        template<typename InChar,
                 typename OutChar,
                 std::forward_iterator Iter,
                 typename Callback,
                 UtfDecoder<Iter> Decoder = UtfCodec<InChar>,
                 UtfEncoder<Callback> Encoder = UtfCodec<OutChar>>
        void convert(Iter iter, const Iter& end, Callback&& callback)
        requires std::is_same_v<InChar, std::iter_value_t<Iter>>
                 && std::is_invocable_v<std::remove_cvref_t<Callback>, OutChar>
        {
            IndexedIterator<Iter> idx_iter = iter;
            IndexedIterator<Iter> idx_end = end;

            while (idx_iter != idx_end) {
                size_t index = idx_iter.index();
                auto [status, code_point] = Decoder::decode(idx_iter, idx_end);
                if (status != UtfStatus::ok) {
                    throw UtfError{status, index};
                }
                status = Encoder::encode(code_point, callback);
                if (status != UtfStatus::ok) {
                    throw UtfError{status};
                }
            }
        }

        /// \copydoc convert
        template<typename InChar,
                 typename OutChar,
                 std::ranges::forward_range Range,
                 typename Callback,
                 UtfDecoder<std::ranges::iterator_t<Range>> Decoder = UtfCodec<InChar>,
                 UtfEncoder<Callback> Encoder = UtfCodec<OutChar>>
        void convert_range(const Range& range, Callback&& callback)
        requires std::is_same_v<InChar, std::ranges::range_value_t<Range>>
                 && std::is_invocable_v<std::remove_cvref_t<Callback>, OutChar>
        {
            utf::convert<InChar, OutChar, std::ranges::iterator_t<const Range>, std::remove_cvref_t<Callback>&, Decoder,
                         Encoder>
                (range.begin(), range.end(), callback);
        }

        /// Converts a range of text from one UTF encoding to another.
        /// Attempts to preserve as much information as possible instead of throwing when an invalid sequence is
        /// encountered.
        template<typename InChar,
                 typename OutChar,
                 std::forward_iterator Iter,
                 typename Callback,
                 UtfDecoder<Iter> Decoder = UtfCodec<InChar>,
                 UtfEncoder<Callback> Encoder = UtfCodec<OutChar>>
        void convert_lossy(Iter iter, const Iter& end, Callback&& callback)
        requires std::is_same_v<InChar, std::iter_value_t<Iter>>
                 && std::is_invocable_v<std::remove_cvref_t<Callback>, OutChar>
        {
            while (iter != end) {
                auto [status, code_point] = Decoder::decode(iter, end);
                Encoder::encode(code_point, callback);
            }
        }

        /// \copydoc convert_lossy
        template<typename InChar,
                 typename OutChar,
                 std::ranges::forward_range Range,
                 typename Callback,
                 UtfDecoder<std::ranges::iterator_t<Range>> Decoder = UtfCodec<InChar>,
                 UtfEncoder<Callback> Encoder = UtfCodec<OutChar>>
        void convert_range_lossy(const Range& range, Callback&& callback)
        requires std::is_same_v<InChar, std::ranges::range_value_t<Range>>
                 && std::is_invocable_v<std::remove_cvref_t<Callback>, OutChar>
        {
            utf::convert_lossy<InChar, OutChar, std::ranges::iterator_t<const Range>, std::remove_cvref_t<Callback>&,
                               Decoder, Encoder>
                (range.begin(), range.end(), callback);
        }

    } // namespace utf

    //==============================================================================
    // UtfCodec<T, 8>
    //==============================================================================

    /// UTF-8 encoder/decoder.
    template<Utf8CodeUnit T>
    struct UtfCodec<T, 8> : NoInstance {
        using char_type = T;

        static constexpr unsigned encoding = 8;
        static constexpr unsigned max_seq_len = 4;

        /// Decodes a UTF-8 sequence from the front of the input range and advances `iter` to the end of the sequence.
        /// \return an error code and the decoded code point.
        template<std::forward_iterator Iter>
        static std::tuple<UtfStatus, char32_t> decode(Iter& iter, const Iter& end)
        requires std::is_same_v<T, std::iter_value_t<Iter>>
        {
            if (iter == end) {
                return {UtfStatus::empty, 0};
            }

            auto get = [&iter]() -> uint8_t {
                if constexpr(std::is_same_v<T, char>) {
                    auto ch = *iter;
                    return *reinterpret_cast<const unsigned char*>(&ch);
                } else {
                    return uint8_t(*iter);
                }
            };

            uint8_t unit = get();
            uint32_t code_point;
            uint32_t min_code_point;
            unsigned seq_len;

            ++iter;

            if (unit <= 0x7F) {
                // 0xxxxxxx (ASCII)
                return {UtfStatus::ok, unit};
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
                return {UtfStatus::invalid, unicode::replacement_char};
            }

            for (unsigned i = 1; i < seq_len; ++i) {
                if (iter == end) {
                    return {UtfStatus::incomplete, unicode::replacement_char};
                }
                unit = get();
                if ((unit & 0xC0) != 0x80) {
                    return {UtfStatus::invalid, unicode::replacement_char};
                }
                ++iter;
                code_point = (code_point << 6) | (unit & 0x3F);
            }

            if (code_point > unicode::max_char) {
                return {UtfStatus::out_of_range, unicode::replacement_char};
            } else if (code_point >= unicode::min_surrogate_char && code_point <= unicode::max_surrogate_char) {
                return {UtfStatus::reserved, code_point};
            } else if (code_point < min_code_point) {
                return {UtfStatus::not_optimal, code_point};
            }

            return {UtfStatus::ok, code_point};
        }

        /// Encodes a UTF-8 sequence by emitting code units through a callback.
        template<typename Callback>
        static UtfStatus encode(char32_t code_point, Callback&& callback)
        requires std::is_invocable_v<std::remove_cvref_t<Callback>, T>
        {
            auto put = [&callback](uint8_t unit) {
                if constexpr(std::is_same_v<T, char>) {
                    unsigned char ch = unit;
                    callback(T(*reinterpret_cast<const char*>(&ch)));
                } else {
                    callback(T(unit));
                }
            };

            if (code_point <= 0x7F) {
                // 0xxxxxxx (ASCII)
                put(uint8_t(code_point));
            } else if (code_point <= 0x7FF) {
                // 110xxxxx 10xxxxxx
                put(uint8_t(0xC0 | (code_point >> 6)));
                put(uint8_t(0x80 | (code_point & 0x3F)));
            } else if (code_point <= 0xFFFF) {
                // 1110xxxx 10xxxxxx 10xxxxxx
                put(uint8_t(0xE0 | (code_point >> 12)));
                put(uint8_t(0x80 | ((code_point >> 6) & 0x3F)));
                put(uint8_t(0x80 | (code_point & 0x3F)));
                if (code_point >= unicode::min_surrogate_char && code_point <= unicode::max_surrogate_char) {
                    return UtfStatus::reserved;
                }
            } else if (code_point <= unicode::max_char) {
                // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                put(uint8_t(0xF0 | (code_point >> 18)));
                put(uint8_t(0x80 | ((code_point >> 12) & 0x3F)));
                put(uint8_t(0x80 | ((code_point >> 6) & 0x3F)));
                put(uint8_t(0x80 | (code_point & 0x3F)));
            } else {
                encode(unicode::replacement_char, callback);
                return UtfStatus::out_of_range;
            }

            return UtfStatus::ok;
        }
    };

    //==============================================================================
    // UtfCodec<T, 16>
    //==============================================================================

    /// UTF-16 encoder/decoder.
    template<Utf16CodeUnit T>
    struct UtfCodec<T, 16> : NoInstance {
        using char_type = T;

        static constexpr unsigned encoding = 16;
        static constexpr unsigned max_seq_len = 2;

        /// Decodes a UTF-16 sequence from the front of the input range and advances `iter` to the end of the sequence.
        /// \return an error code and the decoded code point.
        template<std::forward_iterator Iter>
        static std::tuple<UtfStatus, char32_t> decode(Iter& iter, const Iter& end)
        requires std::is_same_v<T, std::iter_value_t<Iter>>
        {
            if (iter == end) {
                return {UtfStatus::empty, 0};
            }

            uint16_t units[2] = {uint16_t(*iter), 0};
            ++iter;

            if ((units[0] & 0xF800) != 0xD800) {
                // Single-unit sequence.
                return {UtfStatus::ok, units[0]};
            } else if (!(units[0] & 0x0400)) {
                // Possible surrogate pair sequence.
                if (iter == end) {
                    return {UtfStatus::incomplete, units[0]};
                }
                units[1] = *iter;
                if ((units[1] & 0xFC00) != 0xDC00) {
                    return {UtfStatus::unpaired_surrogate, units[0]};
                }
                ++iter;
                return {UtfStatus::ok, ((uint32_t(units[0] & 0x03FF) << 10) + 0x1'0000) | (units[1] & 0x03FF)};
            } else {
                return {UtfStatus::unpaired_surrogate, units[0]};
            }
        }

        /// Encodes a UTF-16 sequence by emitting code units through a callback.
        template<typename Callback>
        static UtfStatus encode(char32_t code_point, Callback&& callback)
        requires std::is_invocable_v<std::remove_cvref_t<Callback>, T>
        {
            if (code_point <= 0xFFFF) {
                // Single-unit sequence.
                callback(T(uint16_t(code_point)));
                if (code_point >= unicode::min_surrogate_char && code_point <= unicode::max_surrogate_char) {
                    return UtfStatus::reserved;
                }
            } else if (code_point <= unicode::max_char) {
                // Surrogate pair sequence.
                callback(T(uint16_t(0xD800 | ((code_point - 0x1'0000) >> 10))));
                callback(T(uint16_t(0xDC00 | (code_point & 0x03FF))));
            } else {
                encode(unicode::replacement_char, callback);
                return UtfStatus::out_of_range;
            }

            return UtfStatus::ok;
        }
    };

    //==============================================================================
    // UtfCodec<T, 32>
    //==============================================================================

    /// UTF-32 encoder/decoder.
    template<Utf32CodeUnit T>
    struct UtfCodec<T, 32> : NoInstance {
        using char_type = T;

        static constexpr unsigned encoding = 32;
        static constexpr unsigned max_seq_len = 1;

        /// Decodes a UTF-32 sequence from the front of the input range and advances `iter` to the end of the sequence.
        /// \return an error code and the decoded code point.
        template<std::forward_iterator Iter>
        static std::tuple<UtfStatus, char32_t> decode(Iter& iter, const Iter& end)
        requires std::is_same_v<T, std::iter_value_t<Iter>>
        {
            if (iter == end) {
                return {UtfStatus::empty, 0};
            }

            uint32_t code_point = uint32_t(*iter);
            ++iter;

            if (code_point > unicode::max_char) {
                return {UtfStatus::out_of_range, unicode::replacement_char};
            } else if (code_point >= unicode::min_surrogate_char && code_point <= unicode::max_surrogate_char) {
                return {UtfStatus::reserved, code_point};
            }

            return {UtfStatus::ok, code_point};
        }

        /// Encodes a UTF-32 sequence by emitting code units through a callback.
        template<typename Callback>
        static UtfStatus encode(char32_t code_point, Callback&& callback)
        requires std::is_invocable_v<std::remove_cvref_t<Callback>, T>
        {
            if (code_point > unicode::max_char) {
                callback(T(unicode::replacement_char));
                return UtfStatus::out_of_range;
            } else if (code_point >= unicode::min_surrogate_char && code_point <= unicode::max_surrogate_char) {
                callback(T(code_point));
                return UtfStatus::reserved;
            }

            callback(T(code_point));
            return UtfStatus::ok;
        }
    };

    /// @}

} // namespace autk

#endif // AUTK_CORE_UTF_H_INCLUDED
