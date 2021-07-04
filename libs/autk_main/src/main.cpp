/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifdef _WIN32
# include <windows.h>
#endif

#include <autk/main.h>

#ifdef _WIN32

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    return ::autk_main(__argc, __wargv);
}

#else // !defined(_WIN32)

int main(int argc, char* argv[])
{
    return ::autk_main(argc, argv);
}

#endif // !defined(_WIN32)
