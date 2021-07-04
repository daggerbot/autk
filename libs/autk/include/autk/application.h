/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_APPLICATION_H_INCLUDED
#define AUTK_APPLICATION_H_INCLUDED

#include <unordered_set>

#include "types.h"

namespace autk {

    class Window;

    /// \cond IMPL
    namespace impl {

        class Display_driver;

    } // namespace impl
    /// \endcond

    /// Encapsulates most of an application's global state.
    /// It should be noted that none of this class's functionality is thread-safe unless otherwise specified.
    class AUTK Application {
        friend class Window;

    public:
        Application();
        Application(Application&&) = delete;
        Application(const Application&) = delete;
        virtual ~Application();

        /// Returns a reference to the application instance.
        /// This should only ever be called from the main thread.
        /// \throw std::logic_error if the application has not been initialized or if it has been destroyed.
        static Application& get_instance();

        /// Quits the application, causing the main loop to return.
        /// This function is thread safe.
        static void quit();

        /// Runs the application's main loop.
        void run();

        Application& operator=(Application&&) = delete;
        Application& operator=(const Application&) = delete;

    protected:
        /// Called when the last window is destroyed.
        /// By default, this quits the application.
        virtual void last_window_destroyed();

    private:
        impl::Display_driver* display_driver_ = nullptr;
        std::unordered_set<Window*> all_windows_;
        bool running_ = false;
    };

} // namespace autk

#endif // AUTK_APPLICATION_H_INCLUDED
