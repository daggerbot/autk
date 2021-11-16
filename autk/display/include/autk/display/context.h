/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_DISPLAY_CONTEXT_H_INCLUDED
#define AUTK_DISPLAY_CONTEXT_H_INCLUDED

#include <autk/core/object.h>

namespace autk {

    /// \addtogroup display
    /// @{

    /// Base class for the entry point into the platform's display subsystem.
    class AUTK_DISPLAY DisplayContext : public Object {
        friend class X11DisplayContext;

    public:
        DisplayContext();
        virtual ~DisplayContext() = 0;

        /// Opens the default system display context.
        static std::unique_ptr<DisplayContext> open_default();

    private:
        struct Data;

        explicit DisplayContext(std::unique_ptr<Data>&& data);

        Data& data();
        const Data& data() const;
    };

    /// @}

} // namespace autk

#endif // AUTK_DISPLAY_CONTEXT_H_INCLUDED
