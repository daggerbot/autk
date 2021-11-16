/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_DYNAMIC_LIBRARY_H_INCLUDED
#define AUTK_CORE_DYNAMIC_LIBRARY_H_INCLUDED

#include <memory>
#include <stdexcept>
#include <string>

#include "types.h"

namespace autk {

    /// \addtogroup core
    /// @{

    /// Loads symbols from a dynamically loaded library.
    class AUTK_CORE DynamicLibrary {
    public:
        DynamicLibrary() = delete;
        DynamicLibrary(DynamicLibrary&&) = delete;
        DynamicLibrary(const DynamicLibrary&) = delete;
        ~DynamicLibrary();

        /// Loads the library at the specified path.
        /// \throw DynamicLibraryError if the library could not be loaded.
        explicit DynamicLibrary(const tchar_t* path);

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
        std::unique_ptr<void, int (*)(void*)> handle_;
    };

    /// Exception raised when an error occurs in a `DynamicLibrary`.
    class AUTK_CORE DynamicLibraryError : public std::runtime_error {
    public:
        DynamicLibraryError(const DynamicLibraryError& other) noexcept;
        explicit DynamicLibraryError(const char* what_arg);
        explicit DynamicLibraryError(const std::string& what_arg);
        ~DynamicLibraryError();

        DynamicLibraryError& operator=(const DynamicLibraryError& other) noexcept;
    };

    /// @}

} // namespace

#endif // AUTK_CORE_DYNAMIC_LIBRARY_H_INCLUDED
