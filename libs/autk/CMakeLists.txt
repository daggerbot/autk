# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This software is subject to the terms of the Boost Software License version
# 1.0. See the included LICENSE file for more information. If you did not
# receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.

set(AUTK_SOURCES
    "src/driver/display.cpp"
    "src/driver/window.cpp"
    "src/application.cpp"
    "src/main_window.cpp"
    "src/strings.cpp"
    "src/utf.cpp"
    "src/window.cpp"
)

set(AUTK_PLATFORM_SOURCES_WIN32
    "src/platform/win32/strings.cpp"
)

set(AUTK_DRIVER_SOURCES_WIN32
    "src/driver/win32/display.cpp"
    "src/driver/win32/window.cpp"
)

set(AUTK_PRIVATE_DEFS ${COMMON_DEFS})

#
# Configure target platform.
#

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND AUTK_SOURCES ${AUTK_PLATFORM_SOURCES_WIN32})
    if(AUTK_SHARED)
        list(APPEND AUTK_PRIVATE_DEFS "AUTK=__declspec(dllexport)")
    endif()
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

#
# Configure target driver.
#

if(AUTK_DRIVER STREQUAL "WIN32")
    list(APPEND AUTK_SOURCES ${AUTK_DRIVER_SOURCES_WIN32})
else()
    message(FATAL_ERROR "AUTK_DRIVER not implemented: ${AUTK_DRIVER}")
endif()

#
# Build Autk library.
#

if(AUTK_SHARED)
    add_library("autk" SHARED ${AUTK_SOURCES})
    set_target_properties("autk" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    )
else()
    add_library("autk" STATIC ${AUTK_SOURCES})
endif()

target_include_directories("autk" PUBLIC "include" "${CONFIG_INCLUDE_DIR}")

if(COMMON_CXX_FLAGS)
    target_compile_options("autk" PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${COMMON_CXX_FLAGS}>)
endif()

if(AUTK_PRIVATE_DEFS)
    target_compile_definitions("autk" PRIVATE ${COMMON_DEFS})
endif()
