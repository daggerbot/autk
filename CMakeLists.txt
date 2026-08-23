# Copyright (c) 2026 Martin Mills
# SPDX-License-Identifier: ISC
# See LICENSE.txt for more information.

cmake_minimum_required(VERSION 3.31)
project(autk C)

include(GNUInstallDirs)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()

#===============================================================================
#
# Build options
#
#===============================================================================

option(AUTK_DEMOS "Build the Autk demo apps" ON)
option(AUTK_SHARED "Build Autk as a shared library" ON)
option(AUTK_STATIC "Build Autk as a static library" OFF)

if(NOT AUTK_SHARED AND NOT AUTK_STATIC)
    message(FATAL_ERROR "Must enable at least one of AUTK_SHARED and AUTK_STATIC")
endif()

#===============================================================================
#
# Driver configuration
#
#===============================================================================

if(LINUX OR BSD)
    find_package(PkgConfig)
    set(AUTK_X11_DEFAULT OFF)
    set(AUTK_WAYLAND_DEFAULT OFF)

    if(PKG_CONFIG_FOUND)
        pkg_check_modules(xcb IMPORTED_TARGET xcb)
        pkg_check_modules(wayland-client IMPORTED_TARGET wayland-client)
        set(AUTK_X11_DEFAULT ${xcb_FOUND})
        if(LINUX)
            set(AUTK_WAYLAND_DEFAULT ${wayland-client_FOUND})
        endif()
    endif()

    option(AUTK_X11 "Enable X11 (xcb) driver support for Autk" ${AUTK_X11_DEFAULT})
    option(AUTK_X11_DLOPEN "Load X11 libraries dynamically" OFF)
    option(AUTK_WAYLAND "Enable Wayland driver support for Autk" ${AUTK_WAYLAND_DEFAULT})
    option(AUTK_WAYLAND_DLOPEN "Load Wayland libraries dynamically" OFF)

    if(AUTK_X11 AND NOT xcb_FOUND)
        message(FATAL_ERROR "Missing required modules for AUTK_X11")
    endif()

    if(AUTK_WAYLAND AND NOT wayland-client_FOUND)
        message(FATAL_ERROR "Missing required modules for AUTK_WAYLAND")
    endif()
else()
    set(AUTK_X11 OFF)
    set(AUTK_WAYLAND OFF)
endif()

#===============================================================================
#
# Compiler options
#
#===============================================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)

add_library(autk-compiler-options INTERFACE)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(autk-compiler-options INTERFACE
        -Wall
        -Wconversion
        -Wextra
        -Wpedantic
        "-fmacro-prefix-map=${PROJECT_SOURCE_DIR}/="
    )
endif()

if(WIN32)
    target_compile_definitions(autk-compiler-options INTERFACE
        _UNICODE
        _WIN32_WINNT=0x0500
        NOMINMAX
        UNICODE
    )
endif()

#===============================================================================
#
# Build targets
#
#===============================================================================

add_subdirectory(include)
add_subdirectory(lib)

if(AUTK_DEMOS)
    add_subdirectory(demos/hello)
endif()
