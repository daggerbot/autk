/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_DISPLAY_OBJECT_DATA_H_INCLUDED
#define AUTK_DISPLAY_OBJECT_DATA_H_INCLUDED

#include <autk/display/context.h>
#include "../core/object_data.h"

namespace autk {

    //==============================================================================
    // DisplayContext
    //==============================================================================

    struct AUTK_DISPLAY DisplayContext::Data : Object::Data {
    };

    inline DisplayContext::DisplayContext(std::unique_ptr<Data>&& data)
        : Object{std::move(data)}
    {
    }

    inline DisplayContext::Data& DisplayContext::data()
    {
        return static_cast<Data&>(Object::data());
    }

    inline const DisplayContext::Data& DisplayContext::data() const
    {
        return static_cast<const Data&>(Object::data());
    }

} // namespace autk

#endif // AUTK_DISPLAY_OBJECT_DATA_H_INCLUDED
