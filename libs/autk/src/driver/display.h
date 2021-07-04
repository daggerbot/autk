/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_DRIVER_DISPLAY_H_INCLUDED
#define AUTK_DRIVER_DISPLAY_H_INCLUDED

#include <memory>

#include <autk/window.h>

namespace autk {

    namespace impl {

        class Window_driver;
        class Window_driver_listener;

        class Display_driver {
        public:
            Display_driver() = default;
            Display_driver(Display_driver&&) = delete;
            Display_driver(const Display_driver&) = delete;
            virtual ~Display_driver() = 0;

            // Lazily initializes and returns the main display driver.
            static Display_driver& get_main();

            virtual std::unique_ptr<Window_driver> create_window(Window_driver_listener& listener,
                                                                 Window_kind kind,
                                                                 Vector size) = 0;

            // Gets a default title to give to Main_windows.
            virtual std::u8string default_app_title() const = 0;

            // Called when the Application instance is initialized.
            virtual void init_app() = 0;

            // Required to be thread-safe.
            virtual void quit() = 0;

            virtual void run() = 0;

            Display_driver& operator=(Display_driver&&) = delete;
            Display_driver& operator=(const Display_driver&) = delete;
        };

    } // namespace impl

} // namespace autk

#endif // AUTK_DRIVER_DISPLAY_H_INCLUDED
