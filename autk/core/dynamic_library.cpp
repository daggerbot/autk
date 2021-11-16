/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <autk/core/dynamic_library.h>

using namespace autk;

DynamicLibraryError::DynamicLibraryError(const DynamicLibraryError& other) noexcept
    : std::runtime_error{other}
{
}

DynamicLibraryError::DynamicLibraryError(const char* what_arg)
    : std::runtime_error{what_arg}
{
}

DynamicLibraryError::DynamicLibraryError(const std::string& what_arg)
    : std::runtime_error{what_arg}
{
}

DynamicLibraryError::~DynamicLibraryError()
{
}

DynamicLibraryError& DynamicLibraryError::operator=(const DynamicLibraryError& other) noexcept
{
    if (&other != this) {
        std::runtime_error::operator=(other);
    }
    return *this;
}
