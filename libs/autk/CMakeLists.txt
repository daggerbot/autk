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

set(AUTK_SOURCES
    "driver/display.cpp"
    "application.cpp"
    "object.cpp"
    "string.cpp"
)
set(AUTK_DRIVER_SOURCES_WIN32
    "driver/win32/display.cpp"
)
set(AUTK_PLATFORM_SOURCES_WIN32
    "platform/win32/os.cpp"
    "platform/win32/string.cpp"
)
set(AUTK_PLATFORM_SOURCES_UNIX
    "platform/unix/os.cpp"
    "platform/unix/string.cpp"
)

set(AUTK_PRIVATE_DEFS ${COMMON_DEFINITIONS})
set(AUTK_PRIVATE_LIBS)

if(AUTK_ICONV)
    find_package("Iconv" REQUIRED)
    list(APPEND AUTK_PRIVATE_LIBS "Iconv::Iconv")
    list(APPEND AUTK_SOURCES "external/iconv.cpp")
endif()

#
# Configure target platform.
#

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND AUTK_SOURCES ${AUTK_PLATFORM_SOURCES_WIN32})
    list(APPEND AUTK_PRIVATE_DEFS "AUTK_IMPORT=__declspec(dllexport)")
elseif(UNIX)
    list(APPEND AUTK_SOURCES ${AUTK_PLATFORM_SOURCES_UNIX})
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

#
# Configure GUI driver.
#

if(AUTK_DRIVER STREQUAL "WIN32")
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
        message(FATAL_ERROR "AUTK_DRIVER=WIN32 is only supported on Windows")
    endif()
    list(APPEND AUTK_SOURCES ${AUTK_DRIVER_SOURCES_WIN32})
else()
    message(FATAL_ERROR "Unimplemented AUTK_DRIVER: ${AUTK_DRIVER}")
endif()

#
# Build the autk library.
#

if(AUTK_SHARED)
    add_library("autk" SHARED ${AUTK_SOURCES})
else()
    add_library("autk" STATIC ${AUTK_SOURCES})
    set_target_properties("autk" PROPERTIES POSITION_INDEPENDENT_CODE ${AUTK_PIC})
endif()

target_include_directories("autk" PUBLIC "include" PRIVATE "${TOPSRCDIR}/libs")

if(AUTK_PRIVATE_LIBS)
    target_link_libraries("autk" PRIVATE ${AUTK_PRIVATE_LIBS})
endif()

if(COMMON_CXX_FLAGS)
    target_compile_options("autk" PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${COMMON_CXX_FLAGS}>)
endif()

if(AUTK_PRIVATE_DEFS)
    target_compile_definitions("autk" PRIVATE ${AUTK_PRIVATE_DEFS})
endif()

install(TARGETS "autk"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(DIRECTORY "include/autk"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

#
# Generate build configuration header.
#

set(HEADER_FLAGS
    AUTK_ICONV
    AUTK_MAIN
    AUTK_PIC
    AUTK_SHARED
)

foreach(FLAG ${HEADER_FLAGS})
    if(${${FLAG}})
        set(${FLAG} "1")
    else()
        set(${FLAG} "0")
    endif()
endforeach()

configure_file("config.h.in" "include/autk/config.h" @ONLY)
target_include_directories("autk" PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include")
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/autk"
        DESTINATION "${AUTK_INSTALL_CONFIG_INCLUDEDIR}")
