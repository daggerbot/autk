/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#include <autk/config.h>
#include "display.h"

#if AUTK_DRIVER == AUTK_DRIVER_WIN32
# include "win32/display.h"
#endif

using namespace autk;
using namespace autk::impl;

Display_driver::~Display_driver()
{
}

Display_driver& Display_driver::get_main()
{
    // Unfortunately, we have to leak the driver instance here.
    // We can't expect the library user to terminate all worker threads before the Application is destroyed.
#if AUTK_DRIVER == AUTK_DRIVER_WIN32
    static auto instance = new Win32_display_driver;
#else
# error "Not implemented"
#endif

    return *instance;
}
