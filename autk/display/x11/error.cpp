/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <autk/display/x11/error.h>

using namespace autk;

X11Error::X11Error(const X11Error& other) noexcept
    : std::runtime_error{other}
{
}

X11Error::X11Error(const char* what_arg)
    : std::runtime_error{what_arg}
{
}

X11Error::X11Error(const std::string& what_arg)
    : std::runtime_error{what_arg}
{
}

X11Error::~X11Error()
{
}

X11Error& X11Error::operator=(const X11Error& other) noexcept
{
    if (&other != this) {
        std::runtime_error::operator=(other);
    }
    return *this;
}
