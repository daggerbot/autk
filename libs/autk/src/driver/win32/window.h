/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_DRIVER_WIN32_WINDOW_H_INCLUDED
#define AUTK_DRIVER_WIN32_WINDOW_H_INCLUDED

#include <memory>

#include <windows.h>

#include "../window.h"

namespace autk {

    namespace impl {

        class Win32_window_driver : public Window_driver {
        public:
            explicit Win32_window_driver(Window_driver_listener& listener, Window_kind kind);
            ~Win32_window_driver();

            void destroy() override;
            void dispose();
            bool is_alive() const override { return !!hwnd_; }
            bool is_visible() const override;
            void set_size(Vector size) override;
            void set_title(std::u8string_view title) override;
            void set_visible(bool visible) override;
            Vector size() const override;
            std::u8string title() const override;
            Window_kind window_kind() const override { return kind_; }

        private:
            std::unique_ptr<std::remove_pointer_t<HWND>, decltype(&::DestroyWindow)> hwnd_{nullptr, &::DestroyWindow};
            Window_kind kind_ = {};

            static Win32_window_driver* get_window(HWND hwnd);
            static void register_class();
            static ATOM register_class_inner();
            static LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

            LONG get_window_long(int index) const;
        };

    } // namespace impl

} // namespace autk

#endif // AUTK_DRIVER_WIN32_WINDOW_H_INCLUDED
