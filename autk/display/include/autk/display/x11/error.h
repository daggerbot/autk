/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_DISPLAY_X11_ERROR_H_INCLUDED
#define AUTK_DISPLAY_X11_ERROR_H_INCLUDED

#include <autk/core/exception.h>

namespace autk {

    /// \addtogroup display
    /// @{

    /// Exception raised when an X11 library or protocol error occurs.
    class AUTK_DISPLAY X11Error : public RuntimeError {
    public:
        X11Error(const X11Error& other) noexcept = default;

        explicit X11Error(std::string&& what_arg)
            : RuntimeError{std::move(what_arg)}
        {
        }

        X11Error& operator=(const X11Error& other) noexcept = default;
    };

    /// @}

} // namespace autk

#endif // AUTK_DISPLAY_X11_ERROR_H_INCLUDED
