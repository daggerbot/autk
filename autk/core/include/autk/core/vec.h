/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_VEC_H_INCLUDED
#define AUTK_CORE_VEC_H_INCLUDED

#include <utility>
#include <type_traits>

#include "types.h"

namespace autk {

    /// \addtogroup core
    /// @{

    //==============================================================================
    // Vec2<T>
    //==============================================================================

    /// 2-dimensional vector structure.
    template<typename T>
    struct Vec2 {
        using scalar_type = T;

        static constexpr size_t extent = 2;

        T x, y;

        constexpr Vec2() requires std::is_default_constructible_v<T> = default;
        constexpr Vec2(Vec2<T>&& other) requires std::is_move_constructible_v<T> = default;
        constexpr Vec2(const Vec2<T>& other) requires std::is_copy_constructible_v<T> = default;

        constexpr Vec2(const T& x, const T& y)
        requires std::is_copy_constructible_v<T>
            : x{x}, y{y}
        {
        }

        template<typename X, typename Y>
        constexpr Vec2(X&& x, Y&& y)
        requires std::is_constructible_v<T, decltype(std::forward<X>(x))>
                 && std::is_constructible_v<T, decltype(std::forward<Y>(y))>
            : x{std::forward<X>(x)}, y{std::forward<Y>(y)}
        {
        }

        constexpr Vec2<T>& operator=(Vec2<T>&& other) requires std::is_move_assignable_v<T> = default;
        constexpr Vec2<T>& operator=(const Vec2<T>& other) requires std::is_copy_assignable_v<T> = default;
    };

    using Vec2i = Vec2<int>;
    using Vec2f = Vec2<float>;
    using Vec2d = Vec2<double>;

    //==============================================================================
    // Vec3<T>
    //==============================================================================

    /// 3-dimensional vector structure.
    template<typename T>
    struct Vec3 {
        using scalar_type = T;

        static constexpr size_t extent = 3;

        T x, y, z;

        constexpr Vec3() requires std::is_default_constructible_v<T> = default;
        constexpr Vec3(Vec3<T>&& other) requires std::is_move_constructible_v<T> = default;
        constexpr Vec3(const Vec3<T>& other) requires std::is_copy_constructible_v<T> = default;

        constexpr Vec3(const T& x, const T& y, const T& z)
        requires std::is_copy_constructible_v<T>
            : x{x}, y{y}, z{z}
        {
        }

        template<typename X, typename Y, typename Z>
        constexpr Vec3(X&& x, Y&& y, Z&& z)
        requires std::is_constructible_v<T, decltype(std::forward<X>(x))>
                 && std::is_constructible_v<T, decltype(std::forward<Y>(y))>
                 && std::is_constructible_v<T, decltype(std::forward<Z>(z))>
            : x{std::forward<X>(x)}, y{std::forward<Y>(y)}, z{std::forward<Z>(z)}
        {
        }

        constexpr Vec3<T>& operator=(Vec3<T>&& other) requires std::is_move_assignable_v<T> = default;
        constexpr Vec3<T>& operator=(const Vec3<T>& other) requires std::is_copy_assignable_v<T> = default;
    };

    using Vec3i = Vec3<int>;
    using Vec3f = Vec3<float>;
    using Vec3d = Vec3<double>;

    //==============================================================================
    // Vec4<T>
    //==============================================================================

    /// 4-dimensional vector structure.
    template<typename T>
    struct Vec4 {
        using scalar_type = T;

        static constexpr size_t extent = 4;

        T x, y, z, w;

        constexpr Vec4() requires std::is_default_constructible_v<T> = default;
        constexpr Vec4(Vec4<T>&& other) requires std::is_move_constructible_v<T> = default;
        constexpr Vec4(const Vec4<T>& other) requires std::is_copy_constructible_v<T> = default;

        constexpr Vec4(const T& x, const T& y, const T& z, const T& w)
        requires std::is_copy_constructible_v<T>
            : x{x}, y{y}, z{z}, w{w}
        {
        }

        template<typename X, typename Y, typename Z, typename W>
        constexpr Vec4(X&& x, Y&& y, Z&& z, W&& w)
        requires std::is_constructible_v<T, decltype(std::forward<X>(x))>
                 && std::is_constructible_v<T, decltype(std::forward<Y>(y))>
                 && std::is_constructible_v<T, decltype(std::forward<Z>(z))>
                 && std::is_constructible_v<T, decltype(std::forward<W>(w))>
            : x{std::forward<X>(x)}, y{std::forward<Y>(y)}, z{std::forward<Z>(z)}, w{std::forward<W>(w)}
        {
        }

        constexpr Vec4<T>& operator=(Vec4<T>&& other) requires std::is_move_assignable_v<T> = default;
        constexpr Vec4<T>& operator=(const Vec4<T>& other) requires std::is_copy_assignable_v<T> = default;
    };

    using Vec4i = Vec4<int>;
    using Vec4f = Vec4<float>;
    using Vec4d = Vec4<double>;

    /// @}

} // namespace autk

#endif // AUTK_CORE_VEC_H_INCLUDED
