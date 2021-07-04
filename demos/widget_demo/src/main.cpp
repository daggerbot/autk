/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#include <autk/autk.h>

namespace {

    class Widget_demo : public autk::Application {
    public:
        autk::Main_window main_window;

        Widget_demo()
        {
        }

        void run()
        {
            main_window.set_visible(true);
            autk::Application::run();
        }
    };

} // namespace

int autk_main(int, const autk::oschar_t *const[])
{
    Widget_demo{}.run();
    return 0;
}
