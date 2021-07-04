/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#include <stdexcept>

#include "window.h"

using namespace autk;
using namespace autk::impl;

Window_driver::~Window_driver()
{
}

void Window_driver::close_requested()
{
    listener_->on_window_close_requested(*this);
}

void Window_driver::destroyed()
{
    listener_->on_window_destroyed(*this);
}

void Window_driver::throw_if_dead() const
{
    if (!is_alive()) {
        throw std::runtime_error{"Window is disposed"};
    }
}
