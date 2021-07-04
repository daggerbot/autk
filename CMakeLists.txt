# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This software is subject to the terms of the Boost Software License version
# 1.0. See the included LICENSE file for more information. If you did not
# receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.

cmake_minimum_required(VERSION "3.16")
project("autk" VERSION "0.0.0" LANGUAGES CXX)

include("GNUInstallDirs")

set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")

#
# Configure build options.
#

unset(DEFAULT_DRIVER)
set(ALL_DRIVERS
    "WIN32"
    "COCOA"
    "X11"
    "WAYLAND"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(DEFAULT_DRIVER "WIN32")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(DEFAULT_DRIVER "COCOA")
elseif(UNIX)
    set(DEFAULT_DRIVER "X11")
endif()

set(AUTK_DEMOS ON CACHE BOOL "Build the Autk demo apps")
set(AUTK_DRIVER "${DEFAULT_DRIVER}" CACHE STRING "Display driver for Autk")
set(AUTK_INSTALL_CONFIG_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE PATH "Install path for <autk/config.h>")
set(AUTK_SHARED OFF CACHE BOOL "Build Autk as a shared library")
set(DOXYGEN "doxygen" CACHE STRING "Doxygen command for building documentation")

if(NOT AUTK_DRIVER)
    message(FATAL_ERROR "Missing AUTK_DRIVER")
elseif(NOT AUTK_DRIVER IN_LIST ALL_DRIVERS)
    message(FATAL_ERROR "Invalid AUTK_DRIVER: ${AUTK_DRIVER}")
endif()

set(CONFIG_FLAGS
    AUTK_SHARED
)

foreach(FLAG ${CONFIG_FLAGS})
    if(${${FLAG}})
        set(${FLAG} "1")
    else()
        set(${FLAG} "0")
    endif()
endforeach()

set(CONFIG_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")
configure_file("config.h.in" "include/autk/config.h" @ONLY)
install(DIRECTORY "${CONFIG_INCLUDE_DIR}/autk" DESTINATION "${AUTK_INSTALL_CONFIG_INCLUDE_DIR}")

#
# Choose common compiler flags.
#

unset(COMMON_CXX_FLAGS)
unset(COMMON_DEFS)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "10")
        message(WARNING "Autk does not support GCC version < 10")
    endif()

    set(COMMON_CXX_FLAGS
        "-std=c++2a"
        "-pedantic"
        "-Wall"
        "-Wconversion"
        "-Wextra"
        "-Wsuggest-override"
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "10")
        message(WARNING "Autk does not support Clang version < 10")
    endif()

    set(COMMON_CXX_FLAGS
        "-std=c++2a"
        "-pedantic"
        "-Wall"
        "-Wconversion"
        "-Wextra"
        "-Wsuggest-override"
    )
else()
    set(CMAKE_CXX_STANDARD "20")
endif()

#
# Configure target platform.
#

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND COMMON_DEFS
        "_UNICODE"
        "_WIN32_WINNT=0x0501"
        "NOMINMAX"
        "UNICODE"
    )
endif()

#
# Build components.
#

add_subdirectory("libs/autk")
add_subdirectory("libs/autk_main")

if(AUTK_DEMOS)
    add_subdirectory("demos/widget_demo")
endif()

#
# Build documentation.
#

configure_file("Doxyfile.in" "Doxyfile" @ONLY)
add_custom_target("docs" COMMAND ${DOXYGEN} "Doxyfile")
