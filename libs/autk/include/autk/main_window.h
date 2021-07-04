/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_MAIN_WINDOW_H_INCLUDED
#define AUTK_MAIN_WINDOW_H_INCLUDED

#include "window.h"

namespace autk {

    /// Main application window.
    /// Can typically have a menubar and a statusbar.
    class AUTK Main_window : public Window {
    public:
        Main_window();
        ~Main_window();
    };

} // namespace autk

#endif // AUTK_MAIN_WINDOW_H_INCLUDED
