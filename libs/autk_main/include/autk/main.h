/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#ifndef AUTK_MAIN_H_INCLUDED
#define AUTK_MAIN_H_INCLUDED

#include <autk/strings.h>

/// User-defined main function used in applications that link against `autk_main`.
int autk_main(int argc, const autk::oschar_t *const argv[]);

#endif // AUTK_MAIN_H_INCLUDED
