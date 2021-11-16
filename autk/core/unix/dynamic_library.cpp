/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <dlfcn.h>

#include <stdexcept>

#include <autk/core/dynamic_library.h>

using namespace autk;
using namespace std::literals::string_literals;

DynamicLibrary::DynamicLibrary(const tchar_t* path)
    : handle_{nullptr, &::dlclose}
{
    handle_.reset(::dlopen(path, RTLD_LAZY));
    if (!handle_) {
        throw DynamicLibraryError{"dlopen(): "s + path + ": " + ::dlerror()};
    }
}

DynamicLibrary::~DynamicLibrary()
{
}

void* DynamicLibrary::get_proc_address(const char* name)
{
    void* sym = try_get_proc_address(name);
    if (!sym) {
        const char* error_detail = ::dlerror();
        if (error_detail) {
            throw DynamicLibraryError{"dlsym(): "s + name + ": " + error_detail};
        }
    }
    return sym;
}

void* DynamicLibrary::try_get_proc_address(const char* name)
{
    return ::dlsym(handle_.get(), name);
}
