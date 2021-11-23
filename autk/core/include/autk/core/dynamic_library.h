/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_DYNAMIC_LIBRARY_H_INCLUDED
#define AUTK_CORE_DYNAMIC_LIBRARY_H_INCLUDED

#ifdef _WIN32
# include <wtypes.h>
#endif

#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "exception.h"

namespace autk {

    /// \addtogroup core
    /// @{

    /// Loads symbols from a dynamically loaded library.
    class AUTK_CORE DynamicLibrary {
    public:
        DynamicLibrary() = delete;
        DynamicLibrary(DynamicLibrary&&) = delete;
        DynamicLibrary(const DynamicLibrary&) = delete;

        /// Loads the library at the specified path.
        /// \throw DynamicLibraryError if the library could not be loaded.
        explicit DynamicLibrary(const tchar_t* path);

        ~DynamicLibrary();

        /// Returns a pointer to the requested symbol.
        /// \throw DynamicLibraryError if the symbol is not found.
        /// \see try_get_proc_address
        void* get_proc_address(const char* name);

        /// Returns a pointer to the requested symbol, or null if it is not found.
        /// \see get_proc_address
        void* try_get_proc_address(const char* name);

        DynamicLibrary& operator=(DynamicLibrary&&) = delete;
        DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    private:
#ifdef _WIN32
        std::unique_ptr<std::remove_pointer_t<HMODULE>, BOOL (__stdcall*)(HMODULE)> handle_;
#else
        std::unique_ptr<void, int (*)(void*)> handle_;
#endif
    };

    /// Exception raised when an error occurs in a `DynamicLibrary`.
    class AUTK_CORE DynamicLibraryError : public RuntimeError {
    public:
        DynamicLibraryError(const DynamicLibraryError& other) noexcept = default;

        explicit DynamicLibraryError(std::string&& what_arg)
            : RuntimeError{std::move(what_arg)}
        {
        }

        DynamicLibraryError& operator=(const DynamicLibraryError& other) noexcept = default;
    };

    /// @}

} // namespace

#endif // AUTK_CORE_DYNAMIC_LIBRARY_H_INCLUDED
