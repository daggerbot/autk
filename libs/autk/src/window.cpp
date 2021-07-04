/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#include <stdexcept>

#include <autk/application.h>
#include <autk/window.h>
#include "driver/display.h"
#include "driver/window.h"

using namespace autk;
using namespace autk::impl;

Window::Window(Window_kind kind, Vector size)
{
    auto& app = Application::get_instance();
    display_driver_ = app.display_driver_;
    driver_ = app.display_driver_->create_window(*this, kind, size);
    app.all_windows_.insert(this);
}

Window::~Window()
{
}

void Window::destroy()
{
    driver_.reset();
}

bool Window::is_alive() const
{
    return driver_ && driver_->is_alive();
}

bool Window::is_visible() const
{
    return driver_ && driver_->is_visible();
}

void Window::set_title(std::u8string_view title)
{
    if (!is_alive()) {
        return;
    }
    driver_->set_title(title);
}

void Window::set_visible(bool visible)
{
    if (visible) {
        throw_if_dead();
    }
    if (is_alive()) {
        driver_->set_visible(visible);
    }
}

std::u8string Window::title() const
{
    if (!is_alive()) {
        return {};
    }
    return driver_->title();
}

void Window::close_requested()
{
    destroy();
}

void Window::destroyed()
{
}

void Window::on_window_close_requested(Window_driver&)
{
    close_requested();
}

void Window::on_window_destroyed(Window_driver&)
{
    auto& app = Application::get_instance();
    app.all_windows_.erase(this);

    destroyed();

    if (app.all_windows_.empty()) {
        app.last_window_destroyed();
    }
}

void Window::throw_if_dead() const
{
    if (!is_alive()) {
        throw std::logic_error{"Window is disposed"};
    }
}
