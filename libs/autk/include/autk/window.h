/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_WINDOW_H_INCLUDED
#define AUTK_WINDOW_H_INCLUDED

#include <memory>
#include <string>

#include "types.h"

namespace autk {

    /// \cond IMPL
    namespace impl {

        class Display_driver;
        class Window_driver;

        class Window_driver_listener {
        public:
            virtual ~Window_driver_listener() = 0;
            virtual void on_window_close_requested(Window_driver& window) = 0;
            virtual void on_window_destroyed(Window_driver& window) = 0;
        };

        inline Window_driver_listener::~Window_driver_listener()
        {
        }

    } // namespace impl
    /// \endcond

    /// Enumeration of window types.
    /// These values correspond to `_NET_WM_WINDOW_TYPE` values per
    /// [XDG Extended Window Manager Hints](https://specifications.freedesktop.org/wm-spec/1.5/).
    enum class Window_kind {
        none,
        normal,
        desktop,
        dock,
        toolbar,
        menu,
        utility,
        splash,
        dialog,
        dropdown_menu,
        popup_menu,
        tooltip,
        notification,
        combo,
        dnd,
    };

    /// Top-level window type.
    class AUTK Window : impl::Window_driver_listener {
        friend class Main_window;

    public:
        Window() = delete;
        Window(Window&&) = delete;
        Window(const Window&) = delete;
        explicit Window(Window_kind kind, Vector size);
        virtual ~Window();

        /// Destroys the window if it is alive.
        void destroy();

        /// Returns true if the window's underlying resources are still valid.
        bool is_alive() const;

        /// Returns true if the window's visibility flag is true.
        /// Note that this does not necessarily mean the window is on screen, as it may be obscured or otherwise
        /// hidden by the window manager.
        bool is_visible() const;

        /// Changes the window's title bar text.
        void set_title(std::u8string_view title);

        /// Shows or hides the window.
        void set_visible(bool visible);

        /// Returns the window's title bar text.
        std::u8string title() const;

        Window& operator=(Window&&) = delete;
        Window& operator=(const Window&) = delete;

    protected:
        /// Called when the window manager signals that the user wants to close the window.
        /// By default, this destroys the window.
        virtual void close_requested();
        /// Called when the window is destroyed.
        virtual void destroyed();

    private:
        impl::Display_driver* display_driver_ = nullptr;
        std::unique_ptr<impl::Window_driver> driver_;

        void on_window_close_requested(impl::Window_driver& driver) override;
        void on_window_destroyed(impl::Window_driver& driver) override;
        void throw_if_dead() const;
    };

} // namespace autk

#endif // AUTK_WINDOW_H_INCLUDED
