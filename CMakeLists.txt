# Copyright (c) Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION "3.16")
project("Autk" VERSION "0.0.0" LANGUAGES CXX)

include("GNUInstallDirs")

set(CONFIG_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")
set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

#===============================================================================
# Configure build options
#===============================================================================

set(AUTK_DEMOS ON CACHE BOOL "Build the Autk demo apps")
set(AUTK_DISPLAY ON CACHE BOOL "Build AutkDisplay")
set(AUTK_DISPLAY_WAYLAND "AUTO" CACHE STRING "Enable Wayland support for AutkDisplay")
set(AUTK_DISPLAY_X11 "AUTO" CACHE STRING "Enable X11 support for AutkDisplay")
set(AUTK_DOXYGEN "doxygen" CACHE STRING "Doxygen command for building API documentation")
set(AUTK_ICONV ON CACHE BOOL "Build with libiconv support (Unix only)")
set(AUTK_INSTALL_CONFIG_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set(AUTK_PIC OFF CACHE BOOL "Build Autk with -fPIC (only if AUTK_SHARED)")
set(AUTK_SHARED OFF CACHE BOOL "Build Autk as a set of shared libraries")

set_property(CACHE AUTK_DISPLAY_WAYLAND PROPERTY STRINGS "AUTO" "ON" "OFF")
set_property(CACHE AUTK_DISPLAY_X11 PROPERTY STRINGS "AUTO" "ON" "OFF")

if(NOT AUTK_DISPLAY)
    set(AUTK_DISPLAY_WAYLAND OFF)
    set(AUTK_DISPLAY_X11 OFF)
endif()

#===============================================================================
# Configure compiler options
#===============================================================================

add_library("AutkCommon" INTERFACE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options("AutkCommon" INTERFACE $<$<COMPILE_LANGUAGE:CXX>:
        "-std=c++2a"
        "-pedantic"
        "-Wall"
        "-Wconversion"
        "-Wextra"
        "-Wsuggest-override"
    >)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options("AutkCommon" INTERFACE $<$<COMPILE_LANGUAGE:CXX>:
        "-std=c++2a"
        "-pedantic"
        "-Wall"
        "-Wconversion"
        "-Wextra"
        "-Wsuggest-override"
    >)
else()
    set(CMAKE_CXX_STANDARD "20")
endif()

#===============================================================================
# Configure target platform
#===============================================================================

set(UNIX_SYSTEM_NAMES
    "DragonFly"
    "FreeBSD"
    "Linux"
    "NetBSD"
    "OpenBSD"
)

if(CMAKE_SYSTEM_NAME IN_LIST UNIX_SYSTEM_NAMES)
    set(AUTK_UNIX 1)
else()
    set(AUTK_ICONV OFF)
    set(AUTK_UNIX 0)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions("AutkCommon" INTERFACE
        "_UNICODE"
        "_WIN32"
        "_WIN32_WINNT=0x0500"
        "_WINDOWS"
        "UNICODE"
        "WINDOWS"
        "WINVER=0x0500"
    )
endif()

#===============================================================================
# Build components
#===============================================================================

add_subdirectory("autk/core")

if(AUTK_DISPLAY)
    add_subdirectory("autk/display")
endif()

if(AUTK_DEMOS)
    if(NOT AUTK_DISPLAY)
        message(FATAL_ERROR "AUTK_DEMOS requires AUTK_DISPLAY")
    endif()
    add_subdirectory("demos/hello_world")
endif()

add_subdirectory("docs")

#===============================================================================
# Generate build configuration files
#===============================================================================

# Set config flags to 0 or 1 so they can be used by the C preprocessor.
set(CONFIG_FLAGS
    AUTK_DISPLAY_WAYLAND
    AUTK_DISPLAY_X11
    AUTK_ICONV
    AUTK_SHARED
)

foreach(FLAG ${CONFIG_FLAGS})
    if(${${FLAG}})
        set(${FLAG} 1)
    else()
        set(${FLAG} 0)
    endif()
endforeach()

configure_file("config.h.in" "${CONFIG_INCLUDE_DIR}/autk/config.h" @ONLY)
install(FILES "${CONFIG_INCLUDE_DIR}/autk/config.h" DESTINATION "${AUTK_INSTALL_CONFIG_INCLUDE_DIR}/autk")
