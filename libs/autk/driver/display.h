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

#ifndef AUTK_DRIVER_DISPLAY_H_INCLUDED
#define AUTK_DRIVER_DISPLAY_H_INCLUDED

#include "autk/config.h"
#include "autk/object.h"
#include "autk/signal.h"

namespace autk {

    namespace impl {

        class AUTK_HIDDEN Display_driver : public Object {
        public:
            /// Invoked from the Display_driver's destructor.
            Signal<Display_driver&> sig_destroyed;

            Display_driver();
            Display_driver(Display_driver&&) = delete;
            Display_driver(const Display_driver&) = delete;
            ~Display_driver();

            /// Returns the global display driver.
            static Display_driver& get_main();

            Display_driver& operator=(Display_driver&&) = delete;
            Display_driver& operator=(const Display_driver&) = delete;
        };

    } // namespace impl

} // namespace autk

#endif // AUTK_DRIVER_DISPLAY_H_INCLUDED
