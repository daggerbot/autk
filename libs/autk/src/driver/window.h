/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_DRIVER_WINDOW_H_INCLUDED
#define AUTK_DRIVER_WINDOW_H_INCLUDED

#include <string>

#include <autk/window.h>

namespace autk {

    namespace impl {

        class Window_driver {
        public:
            Window_driver() = delete;
            Window_driver(Window_driver&&) = delete;
            Window_driver(const Window_driver&) = delete;

            explicit Window_driver(Window_driver_listener& listener)
                : listener_{&listener}
            {
            }

            virtual ~Window_driver() = 0;

            void close_requested();
            virtual void destroy() = 0;
            void destroyed();
            virtual bool is_alive() const = 0;
            virtual bool is_visible() const = 0;
            virtual void set_size(Vector size) = 0;
            virtual void set_title(std::u8string_view title) = 0;
            virtual void set_visible(bool visible) = 0;
            virtual Vector size() const = 0;
            void throw_if_dead() const;
            virtual std::u8string title() const = 0;
            virtual Window_kind window_kind() const = 0;

            Window_driver& operator=(Window_driver&&) = delete;
            Window_driver& operator=(const Window_driver&) = delete;

        private:
            Window_driver_listener* listener_ = nullptr;
        };

    } // namespace impl

} // namespace autk

#endif // AUTK_DRIVER_WINDOW_H_INCLUDED
