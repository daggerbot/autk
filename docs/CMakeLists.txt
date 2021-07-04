# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This software is subject to the terms of the Boost Software License version
# 1.0. See the included LICENSE file for more information. If you did not
# receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.

configure_file("Doxyfile.in" "Doxyfile" @ONLY)
add_custom_target("docs" COMMAND ${DOXYGEN} "Doxyfile")
