# Copyright (c) 2026 Martin Mills
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

cmake_minimum_required(VERSION 3.31)
project(autk C)

include(GNUInstallDirs)

if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif()

if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif()

if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()

#===============================================================================
#
# Build options
#
#===============================================================================

option(AUTK_BUILD_EXAMPLES "Build example Autk programs" ON)
option(AUTK_SHARED "Build Autk as a shared library" ON)

set(AUTK_INSTALL_CONFIG_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}"
    CACHE PATH "Install path for <autk/config.h>")

# Wayland and X11 support.
if(UNIX AND NOT APPLE)
    find_package(PkgConfig)
    set(AUTK_CLIENT_WAYLAND_DEFAULT OFF)
    set(AUTK_CLIENT_X11_DEFAULT OFF)

    if(NOT DEFINED AUTK_CLIENT_WAYLAND AND PkgConfig_FOUND AND LINUX)
        pkg_check_modules(wayland-client IMPORTED_TARGET wayland-client)
        set(AUTK_CLIENT_WAYLAND_DEFAULT ${wayland-client_FOUND})
    endif()

    if(NOT DEFINED AUTK_CLIENT_X11 AND PkgConfig_FOUND)
        pkg_check_modules(xcb IMPORTED_TARGET xcb)
        set(AUTK_CLIENT_X11_DEFAULT ${xcb_FOUND})
    endif()

    option(AUTK_CLIENT_WAYLAND "Build Autk with Wayland support" ${AUTK_CLIENT_WAYLAND_DEFAULT})
    option(AUTK_CLIENT_X11 "Build Autk with X11 support" ${AUTK_CLIENT_X11_DEFAULT})
endif()

# Windows support.
if(WIN32)
    set(AUTK_CLIENT_WINDOWS ON)
else()
    set(AUTK_CLIENT_WINDOWS OFF)
endif()

# Generate <autk/config.h>
set(GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/include")
set(CONFIG_HEADER "${GENERATED_INCLUDE_DIR}/autk/config.h")
configure_file(config.h.in "${CONFIG_HEADER}")
install(FILES "${CONFIG_HEADER}" DESTINATION "${AUTK_INSTALL_CONFIG_INCLUDEDIR}/autk")

#===============================================================================
#
# Compiler options
#
#===============================================================================

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)

add_library(autk-compiler-options INTERFACE)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(autk-compiler-options INTERFACE
        "-fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/src/="
        -pedantic
        -Wall
        -Wconversion
        -Wextra
        -Wshadow
    )
endif()

#===============================================================================
#
# Build targets
#
#===============================================================================

add_subdirectory(src)

if(AUTK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

#===============================================================================
#
# Install headers
#
#===============================================================================

install(DIRECTORY include/autk DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
