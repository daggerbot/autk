/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#include <windows.h>

#include <algorithm>
#include <stdexcept>
#include <system_error>

#include <autk/strings.h>
#include "display.h"
#include "window.h"

using namespace autk;
using namespace autk::impl;

namespace {

    const wchar_t class_name[] = L"AutkWindow";

} // namespace

//----------------------------------------------------------------------------------------------------------------------

Win32_window_driver::Win32_window_driver(Window_driver_listener& listener,
                                         Window_kind kind)
    : Window_driver{listener}
{
    DWORD style;
    DWORD ex_style;

    register_class();

    switch (kind) {
    case Window_kind::normal:
        style = WS_OVERLAPPEDWINDOW;
        ex_style = 0;
        break;
    default:
        throw std::invalid_argument{"Invalid or unimplemented window kind"};
    }

    hwnd_.reset(CreateWindowExW(ex_style, class_name, nullptr, style, CW_USEDEFAULT, CW_USEDEFAULT,
                                CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, impl::get_exe_handle(), nullptr));
    if (!hwnd_) {
        throw std::system_error{int(::GetLastError()), std::system_category(), "CreateWindowExW failed"};
    }

    ::SetLastError(0);
    ::SetWindowLongPtrW(hwnd_.get(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    if (auto error_code = ::GetLastError()) {
        throw std::system_error{int(error_code), std::system_category(), "SetWindowLongPtrW failed"};
    }
}

Win32_window_driver::~Win32_window_driver()
{
    destroy();
}

void Win32_window_driver::destroy()
{
    if (is_alive()) {
        // Invokes WM_DESTROY, which invokes dispose(), which sets hwnd_ to null.
        ::DestroyWindow(hwnd_.get());
    }
}

void Win32_window_driver::dispose()
{
    bool notify = is_alive();

    hwnd_.release();

    if (notify) {
        destroyed();
    }
}

bool Win32_window_driver::is_visible() const
{
    if (!is_alive()) {
        return false;
    }
    return get_window_long(GWL_STYLE) & WS_VISIBLE;
}

void Win32_window_driver::set_size(Vector size)
{
    if (!is_alive()) {
        return;
    }

    RECT rect = {0, 0, size.x, size.y};
    DWORD style = get_window_long(GWL_STYLE);
    DWORD ex_style = get_window_long(GWL_EXSTYLE);

    if (!::AdjustWindowRectEx(&rect, style, false, ex_style)) {
        throw std::system_error{int(::GetLastError()), std::system_category(), "AdjustWindowRectEx failed"};
    }

    auto result = ::SetWindowPos(hwnd_.get(), nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
                                 SWP_NOMOVE | SWP_NOZORDER);
    if (!result) {
        throw std::system_error{int(::GetLastError()), std::system_category(), "SetWindowPos failed"};
    }
}

void Win32_window_driver::set_title(std::u8string_view title)
{
    if (!is_alive()) {
        return;
    }

    if (!::SetWindowTextW(hwnd_.get(), to_os_string_lossy(title).c_str())) {
        throw std::system_error{int(::GetLastError()), std::system_category(), "SetWindowText failed"};
    }
}

void Win32_window_driver::set_visible(bool visible)
{
    if (visible) {
        throw_if_dead();
    } else if (!is_alive()) {
        return;
    }
    ::SetLastError(0);
    ::ShowWindow(hwnd_.get(), visible ? SW_SHOW : SW_HIDE);
    if (auto error_code = ::GetLastError()) {
        throw std::system_error{int(error_code), std::system_category(), "ShowWindow failed"};
    }
}

Vector Win32_window_driver::size() const
{
    if (!is_alive()) {
        return {};
    }

    RECT client_rect = {};

    if (!::GetClientRect(hwnd_.get(), &client_rect)) {
        throw std::system_error{int(::GetLastError()), std::system_category(), "GetClientRect failed"};
    }

    return {client_rect.right - client_rect.left, client_rect.bottom - client_rect.top};
}

std::u8string Win32_window_driver::title() const
{
    // Prevent a possible buffer overrun.
    static_assert(sizeof(std::size_t) >= sizeof(int));

    int length;
    std::wstring title;

    if (!is_alive()) {
        return {};
    }

    ::SetLastError(0);
    length = std::max<int>(0, ::GetWindowTextLengthW(hwnd_.get()));
    if (auto error_code = ::GetLastError()) {
        throw std::system_error{int(error_code), std::system_category(), "GetWindowTextLengthW failed"};
    }
    if (!length) {
        return {};
    }
    title.resize(std::size_t(length));
    length = std::max<int>(0, ::GetWindowTextW(hwnd_.get(), title.data(), length + 1));
    if (auto error_code = ::GetLastError()) {
        throw std::system_error{int(error_code), std::system_category(), "GetWindowTextW failed"};
    }
    title.resize(std::size_t(length));
    return to_utf8_lossy(title);
}

LONG Win32_window_driver::get_window_long(int index) const
{
    throw_if_dead();
    ::SetLastError(0);
    auto value = ::GetWindowLongW(hwnd_.get(), index);
    if (auto error_code = ::GetLastError()) {
        throw std::system_error{int(error_code), std::system_category(), "GetWindowLongW failed"};
    }
    return value;
}

Win32_window_driver* Win32_window_driver::get_window(HWND hwnd)
{
    if (!hwnd) {
        return nullptr;
    }
    ::SetLastError(0);
    auto data = ::GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (auto error_code = ::GetLastError()) {
        throw std::system_error{int(error_code), std::system_category(), "GetWindowLongPtrW failed"};
    }
    return reinterpret_cast<Win32_window_driver*>(data);
}

void Win32_window_driver::register_class()
{
    [[maybe_unused]] static ATOM atom = register_class_inner();
}

ATOM Win32_window_driver::register_class_inner()
{
    WNDCLASSEXW wc = {
        .cbSize = sizeof(WNDCLASSEXW),
        .style = 0,
        .lpfnWndProc = &wndproc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = impl::get_exe_handle(),
        .hIcon = nullptr,
        .hCursor = ::LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = nullptr,
        .lpszMenuName = nullptr,
        .lpszClassName = class_name,
        .hIconSm = nullptr,
    };

    auto atom = ::RegisterClassExW(&wc);
    if (!atom) {
        throw std::system_error{int(::GetLastError()), std::system_category(), "RegisterClassExW failed"};
    }
    return atom;
}

LRESULT CALLBACK Win32_window_driver::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    Win32_window_driver* window;

    switch (msg) {
    case WM_CLOSE:
        window = get_window(hwnd);
        if (window) {
            window->close_requested();
        }
        return 0;

    case WM_DESTROY:
        window = get_window(hwnd);
        if (window) {
            window->dispose();
        }
        return 0;

    default:
        return ::DefWindowProcW(hwnd, msg, wparam, lparam);
    }
}
