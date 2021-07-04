/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_DRIVER_WIN32_DISPLAY_H_INCLUDED
#define AUTK_DRIVER_WIN32_DISPLAY_H_INCLUDED

#include <mutex>
#include <optional>

#include <windows.h>

#include "../display.h"

namespace autk {

    namespace impl {

        class Win32_display_driver : public Display_driver {
        public:
            Win32_display_driver();
            ~Win32_display_driver();

            std::unique_ptr<Window_driver> create_window(Window_driver_listener& listener,
                                                         Window_kind kind,
                                                         Vector size) override;

            std::u8string default_app_title() const override;
            void init_app() override;
            void quit() override;
            void run() override;

        private:
            std::recursive_mutex mutex_;
            std::optional<DWORD> main_thread_id_;
            bool quit_requested_ = false;
        };

        HINSTANCE get_exe_handle();

    } // namespace impl

} // namespace autk

#endif // AUTK_DRIVER_WIN32_DISPLAY_H_INCLUDED
