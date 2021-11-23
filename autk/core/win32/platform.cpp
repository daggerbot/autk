/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include <memory>

#include "platform.h"

using namespace autk;
using namespace std::literals::string_literals;

std::string win32::strerror(uint32_t error_code)
{
    char* msg_ptr = nullptr;

    ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                     nullptr, error_code, 0, reinterpret_cast<LPSTR>(&msg_ptr), 0, nullptr);

    if (msg_ptr) {
        std::unique_ptr<char[], decltype(&::LocalFree)> msg_buf{msg_ptr, &::LocalFree};
        return std::string{msg_buf.get()};
    } else {
        return "Win32 error code "s + std::to_string(error_code);
    }
}
