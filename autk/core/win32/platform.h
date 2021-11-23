/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_WIN32_PLATFORM_H_INCLUDED
#define AUTK_CORE_WIN32_PLATFORM_H_INCLUDED

#include <string>

#include <autk/core/types.h>

namespace autk {

    namespace win32 {

        AUTK_CORE std::string strerror(uint32_t error_code);

    } // namespace win32

} // namespace autk

#endif // AUTK_CORE_WIN32_PLATFORM_H_INCLUDED
