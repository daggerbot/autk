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

#include <stdexcept>

#include "autk/driver/win32/display.h"

using namespace autk;
using namespace autk::impl;

Win32_display_driver::Win32_display_driver(This_thread_t)
{
    // Make sure a driver is only ever constructed once per thread.
    static thread_local Win32_display_driver* thread_driver = this;
    if (thread_driver != this) {
        throw std::logic_error{"Display driver already created for the calling thread"};
    }
}

Win32_display_driver::~Win32_display_driver()
{
}
