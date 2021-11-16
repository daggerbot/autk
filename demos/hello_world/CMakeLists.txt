# Copyright (c) Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_executable("hello_world" WIN32
    "main.cpp"
)
target_link_libraries("hello_world" PRIVATE "AutkCommon" "AutkDisplay")
