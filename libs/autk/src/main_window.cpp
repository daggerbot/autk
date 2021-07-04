/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#include <autk/main_window.h>
#include "driver/display.h"

using namespace autk;
using namespace autk::impl;

namespace {

    // The value here does not matter much.
    // The window will usually be packed after widgets are added to it.
    inline constexpr Vector default_size = {320, 200};

} // namespace

//----------------------------------------------------------------------------------------------------------------------

Main_window::Main_window()
    : Window{Window_kind::normal, default_size}
{
    set_title(display_driver_->default_app_title());
}

Main_window::~Main_window()
{
}
