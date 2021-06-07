// Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "autk/driver/display.h"
#include "autk/config.h"

#if AUTK_DRIVER == AUTK_DRIVER_WIN32
# include "autk/driver/win32/display.h"
#endif

using namespace autk;
using namespace autk::impl;

Display_driver::Display_driver()
{
}

Display_driver::~Display_driver()
{
    sig_destroyed(*this);
}

Display_driver& Display_driver::get_main()
{
    // We don't know if static objects or objects in other threads will try to use the main driver,
    // so we unfortunately have to leak it.
#if AUTK_DRIVER == AUTK_DRIVER_WIN32
    static Display_driver* instance = new Win32_display_driver{this_thread};
#else
# error "Not implemented"
#endif

    return *instance;
}
