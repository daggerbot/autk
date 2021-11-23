/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_ITERATOR_H_INCLUDED
#define AUTK_CORE_ITERATOR_H_INCLUDED

#include <iterator>
#include <type_traits>
#include <utility>

#include "types.h"

namespace autk {

    /// \addtogroup core
    /// @{

    /// Forward iterator which keeps track of how many times it is incremented.
    template<std::forward_iterator Inner>
    class IndexedIterator {
    public:
        using difference_type = typename std::iterator_traits<Inner>::difference_type;
        using value_type = typename std::iterator_traits<Inner>::value_type;
        using pointer = typename std::iterator_traits<Inner>::pointer;
        using reference = typename std::iterator_traits<Inner>::reference;
        using iterator_category = std::forward_iterator_tag;

        constexpr IndexedIterator() requires std::is_constructible_v<Inner> = default;
        constexpr IndexedIterator(IndexedIterator&& other) requires std::is_move_constructible_v<Inner> = default;
        constexpr IndexedIterator(const IndexedIterator& other) requires std::is_copy_constructible_v<Inner> = default;

        constexpr IndexedIterator(Inner&& inner)
        requires std::is_move_constructible_v<Inner>
            : inner_{std::move(inner)}
        {
        }

        constexpr IndexedIterator(const Inner& inner)
        requires std::is_copy_constructible_v<Inner>
            : inner_{inner}
        {
        }

        template<typename...Args>
        constexpr explicit IndexedIterator(Args&&...args)
        requires std::is_constructible_v<Inner, decltype(std::forward<Args>(args))...>
            : inner_{std::forward<Args>(args)...}
        {
        }

        constexpr size_t index() const { return index_; }
        constexpr Inner& inner() { return inner_; }
        constexpr const Inner& inner() const { return inner_; }

        constexpr IndexedIterator<Inner>& operator=(IndexedIterator<Inner>&& inner)
        requires std::is_move_assignable_v<Inner> = default;

        constexpr IndexedIterator<Inner>& operator=(const IndexedIterator<Inner>& inner)
        requires std::is_copy_assignable_v<Inner> = default;

        constexpr bool operator==(const IndexedIterator<Inner>& rhs) const
        {
            return inner_ == rhs.inner_;
        }

        constexpr bool operator!=(const IndexedIterator<Inner>& rhs) const
        {
            return inner_ != rhs.inner_;
        }

        constexpr IndexedIterator<Inner>& operator++()
        {
            ++inner_;
            ++index_;
            return *this;
        }

        constexpr IndexedIterator<Inner> operator++(int)
        requires std::is_copy_constructible_v<Inner>
        {
            auto copy = *this;
            ++inner_;
            ++index_;
            return copy;
        }

        constexpr auto& operator*() const
        {
            return *inner_;
        }

        constexpr auto operator->() const
        {
            return inner_.operator->();
        }

    private:
        Inner inner_;
        size_t index_ = 0;
    };

    /// @}

} // namespace autk

#endif // AUTK_CORE_ITERATOR_H_INCLUDED
