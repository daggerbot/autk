/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_CORE_OBJECT_DATA_H_INCLUDED
#define AUTK_CORE_OBJECT_DATA_H_INCLUDED

#include <autk/core/object.h>
#include <autk/core/signals.h>

namespace autk {

    //==============================================================================
    // Object
    //==============================================================================

    struct AUTK_CORE Object::Data {
        Signal<Object&> sig_disposed;

        signals::_impl::ConnectionBase* first_ctn = nullptr;
        signals::_impl::ConnectionBase* last_ctn = nullptr;
    };

    inline Object::Data& Object::data()
    {
        return *data_.get();
    }

    inline const Object::Data& Object::data() const
    {
        return *data_.get();
    }

} // namespace autk

#endif // AUTK_CORE_OBJECT_DATA_H_INCLUDED
