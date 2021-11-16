/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <autk/core/signals.h>
#include "object_data.h"

using namespace autk;

Object::Object()
    : Object{std::make_unique<Data>()}
{
}

Object::Object(std::unique_ptr<Data>&& data)
    : data_{std::move(data)}
{
    assert(data_);
}

Object::~Object()
{
    data().sig_disposed(*this);

    // Delete all signal connections.
    while (auto ctn = data().first_ctn) {
        unlink_connection(ctn);
        ctn->unlink_from_signal();
        delete ctn;
    }
}

Signal<Object&>& Object::sig_disposed() noexcept
{
    return data().sig_disposed;
}

void Object::link_connection(signals::_impl::ConnectionBase* ctn) noexcept
{
    if (data().last_ctn) {
        data().last_ctn->next_in_owner = ctn;
    } else {
        data().first_ctn = ctn;
    }
    ctn->prev_in_owner = data().last_ctn;
    data().last_ctn = ctn;
}

void Object::unlink_connection(signals::_impl::ConnectionBase* ctn) noexcept
{
    if (ctn->prev_in_owner) {
        ctn->prev_in_owner->next_in_owner = ctn->next_in_owner;
    } else {
        data().first_ctn = ctn->next_in_owner;
    }

    if (ctn->next_in_owner) {
        ctn->next_in_owner->prev_in_owner = ctn->prev_in_owner;
    } else {
        data().last_ctn = ctn->prev_in_owner;
    }

    ctn->prev_in_owner = nullptr;
    ctn->next_in_owner = nullptr;
}
