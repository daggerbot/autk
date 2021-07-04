/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_TYPES_H_INCLUDED
#define AUTK_TYPES_H_INCLUDED

#include <autk/config.h>

namespace autk {

    /// 2-dimensional integer vector type.
    struct Vector {
        int x, y;

        constexpr Vector() = default;
        constexpr Vector(Vector&&) = default;
        constexpr Vector(const Vector&) = default;

        constexpr Vector(int x, int y)
            : x{x}, y{y}
        {
        }

        constexpr explicit operator bool() const
        {
            return x || y;
        }

        constexpr bool operator!() const
        {
            return !x && !y;
        }

        constexpr Vector operator-() const
        {
            return {-x, -y};
        }

        constexpr Vector& operator=(Vector&&) = default;
        constexpr Vector& operator=(const Vector&) = default;

        constexpr Vector operator+(const Vector& rhs) const
        {
            return {x + rhs.x, y + rhs.y};
        }

        constexpr Vector& operator+=(const Vector& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        constexpr Vector operator-(const Vector& rhs) const
        {
            return {x - rhs.x, y - rhs.y};
        }

        constexpr Vector& operator-=(const Vector& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        constexpr Vector operator*(int rhs) const
        {
            return {x * rhs, y * rhs};
        }

        constexpr Vector operator*(const Vector& rhs) const
        {
            return {x * rhs.x, y * rhs.y};
        }

        constexpr Vector& operator*=(int rhs)
        {
            x *= rhs;
            y *= rhs;
            return *this;
        }

        constexpr Vector& operator*=(const Vector& rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            return *this;
        }

        constexpr Vector operator/(int rhs) const
        {
            return {x / rhs, y / rhs};
        }

        constexpr Vector operator/(const Vector& rhs) const
        {
            return {x / rhs.x, y / rhs.y};
        }

        constexpr Vector& operator/=(int rhs)
        {
            x /= rhs;
            y /= rhs;
            return *this;
        }

        constexpr Vector& operator/=(const Vector& rhs)
        {
            x /= rhs.x;
            y /= rhs.y;
            return *this;
        }

        constexpr bool operator==(const Vector& rhs) const
        {
            return x == rhs.x && y == rhs.y;
        }

        constexpr bool operator!=(const Vector& rhs) const
        {
            return x != rhs.x || y != rhs.y;
        }
    };

} // namespace autk

#endif // AUTK_TYPES_H_INCLUDED
