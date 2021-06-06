# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION "3.16")
project("autk" VERSION "0.0.0" LANGUAGES CXX)

include("GNUInstallDirs")

set(TOPSRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")

#
# Configure build options.
#

set(AUTK_INSTALL_CONFIG_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE STRING "Install location for <autk/config.h>")
set(AUTK_DEMOS ON CACHE BOOL "Build the autk demo apps")
set(AUTK_DOXYGEN "doxygen" CACHE STRING "Command for building Doxygen documentation")
set(AUTK_ICONV ON CACHE BOOL "Build autk with libiconv support")
set(AUTK_MAIN ON CACHE BOOL "Build autk_main")
set(AUTK_PIC ON CACHE BOOL "Generate position-independent code for autk")
set(AUTK_SHARED OFF CACHE BOOL "Build autk as a shared library")

#
# Set common compiler flags.
#

unset(COMMON_CXX_FLAGS)
unset(COMMON_DEFINITIONS)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "10")
        message(WARNING "GCC versions <10 are not supported")
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
        message(WARNING "Clang versions <10 are not supported")
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
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
    set(AUTK_ICONV OFF)
    list(APPEND COMMON_DEFINITIONS
        "_UNICODE"
        "_WIN32_WINNT=0x0400"
        "NOMINMAX"
        "UNICODE"
    )
endif()

#
# Build targets.
#

add_subdirectory("libs/autk")

if(AUTK_MAIN)
    add_subdirectory("libs/autk_main")
endif()

if(AUTK_DEMOS)
    if(NOT AUTK_MAIN)
        message(FATAL_ERROR "AUTK_DEMOS requires AUTK_MAIN")
    endif()

    add_subdirectory("demos/widget_demo")
endif()

#
# Install documentation.
#

install(
    FILES
        "LICENSE.txt"
        "README.md"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}")

#
# Build doxygen docs.
#

configure_file("Doxyfile.in" "Doxyfile" @ONLY)
add_custom_target("docs" COMMAND ${AUTK_DOXYGEN} "Doxyfile")
