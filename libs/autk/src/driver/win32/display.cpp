/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#include <windows.h>

#include <filesystem>
#include <limits>
#include <memory>
#include <stdexcept>
#include <system_error>

#include <autk/strings.h>
#include "display.h"
#include "window.h"

using namespace autk;
using namespace autk::impl;

namespace {

    std::wstring get_exe_path_w()
    {
        std::unique_ptr<wchar_t[]> buffer;
        DWORD size = 256;
        DWORD result;

        ::SetLastError(0);

        for (;;) {
            buffer.reset(new wchar_t[size]);
            result = ::GetModuleFileNameW(nullptr, buffer.get(), size);

            if (!result) {
                if (auto error_code = ::GetLastError(); error_code && error_code != ERROR_INSUFFICIENT_BUFFER) {
                    throw std::system_error{int(error_code), std::system_category(), "GetModuleFileNameW failed"};
                }
            } else if (result < size - 1) {
                return std::wstring{buffer.get(), result};
            }

            if (size > std::numeric_limits<decltype(size)>::max() / 2) {
                throw std::overflow_error{"Buffer size overflow"};
            }
            size *= 2;
        }
    }

} // namespace

//----------------------------------------------------------------------------------------------------------------------

Win32_display_driver::Win32_display_driver()
{
}

Win32_display_driver::~Win32_display_driver()
{
}

std::unique_ptr<Window_driver> Win32_display_driver::create_window(Window_driver_listener& listener,
                                                                   Window_kind kind,
                                                                   Vector size)
{
    auto window = std::make_unique<Win32_window_driver>(listener, kind);
    window->set_size(size);
    return window;
}

std::u8string Win32_display_driver::default_app_title() const
{
    return to_utf8_lossy(std::filesystem::path{::get_exe_path_w()}.stem().wstring());
}

void Win32_display_driver::init_app()
{
    std::lock_guard<std::recursive_mutex> lock{mutex_};
    main_thread_id_ = ::GetCurrentThreadId();
}

void Win32_display_driver::quit()
{
    std::lock_guard<std::recursive_mutex> lock{mutex_};
    quit_requested_ = true;

    if (main_thread_id_.has_value()) {
        if (!::PostThreadMessageW(main_thread_id_.value(), WM_QUIT, 0, 0)) {
            throw std::system_error{int(::GetLastError()), std::system_category(), "PostThreadMessageW failed"};
        }
    }
}

void Win32_display_driver::run()
{
    MSG msg;

    // Check if quit() has already been called.
    {
        std::lock_guard<std::recursive_mutex> lock{mutex_};
        if (quit_requested_) {
            return;
        }
    }

    for (;;) {
        switch (::GetMessage(&msg, nullptr, 0, 0)) {
        case -1:
            throw std::system_error{int(::GetLastError()), std::system_category(), "GetMessage failed"};
        case 0:
            return;
        default:
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

HINSTANCE impl::get_exe_handle()
{
    auto handle = ::GetModuleHandleW(nullptr);
    if (!handle) {
        throw std::system_error{int(::GetLastError()), std::system_category(), "GetModuleHandleW failed"};
    }
    return handle;
}
