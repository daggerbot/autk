# Copyright (c) 2026 Martin Mills
# SPDX-License-Identifier: ISC
# See LICENSE.txt for more information.

add_executable(hello WIN32 hello.c)
target_link_libraries(hello PRIVATE autk-compiler-options autk-default)
