/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include <autk/core/dynamic_library.h>
#include <autk/core/encoding.h>
#include "platform.h"

using namespace autk;
using namespace std::literals::string_literals;

DynamicLibrary::DynamicLibrary(const tchar_t* path)
    : handle_{nullptr, &::FreeLibrary}
{
    handle_.reset(::LoadLibraryW(path));

    if (!handle_) {
        uint32_t error_code = ::GetLastError();
        throw DynamicLibraryError{"LoadLibraryW(): "s + encoding::to_sys_lossy(encoding::to_utf8_lossy(path))
                                  + win32::strerror(error_code)};
    }
}

DynamicLibrary::~DynamicLibrary()
{
}

void* DynamicLibrary::get_proc_address(const char* name)
{
    void* sym = try_get_proc_address(name);

    if (!sym) {
        uint32_t error_code = ::GetLastError();
        throw DynamicLibraryError{"dlsym(): "s + name + ": " + win32::strerror(error_code)};
    }

    return sym;
}

void* DynamicLibrary::try_get_proc_address(const char* name)
{
    return reinterpret_cast<void*>(::GetProcAddress(handle_.get(), name));
}
