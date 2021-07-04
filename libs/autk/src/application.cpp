/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#include <atomic>
#include <cstdlib>
#include <stdexcept>

#include <autk/application.h>
#include "driver/display.h"

using namespace autk;
using namespace autk::impl;

namespace {

    enum class App_state {
        none,
        alive,
        destroyed,
    };

    constinit std::atomic<App_state> app_state;
    constinit Application* instance = nullptr;

} // namespace

//----------------------------------------------------------------------------------------------------------------------

Application::Application()
{
    // Make sure only one instance is ever created.
    auto expected_state = App_state::none;
    if (!app_state.compare_exchange_strong(expected_state, App_state::alive)) {
        throw std::logic_error{"Multiple application instances created"};
    }
    instance = this;

    display_driver_ = &Display_driver::get_main();
    display_driver_->init_app();
}

Application::~Application()
{
    for (auto window : all_windows_) {
        window->destroy();
    }
    app_state.store(App_state::destroyed, std::memory_order::relaxed);
}

Application& Application::get_instance()
{
    switch (app_state.load(std::memory_order::relaxed)) {
    case App_state::none:
        throw std::logic_error{"Application instance does not exist"};
    case App_state::alive:
        return *instance;
    case App_state::destroyed:
        throw std::logic_error{"Application instance has been destroyed"};
    default:
        // Unreachable
        std::abort();
    }
}

void Application::quit()
{
    Display_driver::get_main().quit();
}

void Application::run()
{
    if (running_) {
        throw std::logic_error{"Recursive main loop"};
    }
    running_ = true;

    try {
        display_driver_->run();
    } catch (...) {
        running_ = false;
        throw;
    }

    running_ = false;
}

void Application::last_window_destroyed()
{
    quit();
}
