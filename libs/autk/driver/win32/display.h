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

#ifndef AUTK_DRIVER_WIN32_DISPLAY_H_INCLUDED
#define AUTK_DRIVER_WIN32_DISPLAY_H_INCLUDED

#include "autk/driver/display.h"
#include "autk/types.h"

namespace autk {

    namespace impl {

        class AUTK_HIDDEN Win32_display_driver : public Display_driver {
        public:
            Win32_display_driver() = delete;
            Win32_display_driver(Win32_display_driver&&) = delete;
            Win32_display_driver(const Win32_display_driver&) = delete;
            explicit Win32_display_driver(This_thread_t);
            ~Win32_display_driver();

            Win32_display_driver& operator=(Win32_display_driver&&) = delete;
            Win32_display_driver& operator=(const Win32_display_driver&) = delete;
        };

    } // namespace impl

} // namespace autk

#endif // AUTK_DRIVER_WIN32_DISPLAY_H_INCLUDED
