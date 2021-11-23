# Copyright (c) Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set(CORE_SOURCES
    "core.cpp"
    "exception.cpp"
    "object.cpp"
    "signals.cpp"
    "utf.cpp"
)
set(CORE_SOURCES_WIN32
    "win32/dynamic_library.cpp"
    "win32/encoding.cpp"
    "win32/platform.cpp"
)
set(CORE_SOURCES_UNIX
    "unix/dynamic_library.cpp"
    "unix/encoding.cpp"
)

#===============================================================================
# Build AutkCore library
#===============================================================================

if(AUTK_SHARED)
    add_library("AutkCore" SHARED ${CORE_SOURCES})
else()
    add_library("AutkCore" STATIC ${CORE_SOURCES})
    set_target_properties("AutkCore" PROPERTIES
        POSITION_INDEPENDENT_CODE "${AUTK_PIC}"
    )
endif()

set_target_properties("AutkCore" PROPERTIES
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    VERSION "${PROJECT_VERSION}"
)

target_compile_definitions("AutkCore" PRIVATE "AUTK_CORE=AUTK_EXPORT")
target_include_directories("AutkCore" PUBLIC "include" "${CONFIG_INCLUDE_DIR}")
target_link_libraries("AutkCore" PRIVATE "AutkCommon")

install(TARGETS "AutkCore"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(DIRECTORY "include/autk"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

#===============================================================================
# Configure target platform
#===============================================================================

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_target_properties("AutkCore" PROPERTIES
        RUNTIME_OUTPUT_NAME "AutkCore-${PROJECT_VERSION_MAJOR}"
    )
    target_sources("AutkCore" PRIVATE ${CORE_SOURCES_WIN32})
    target_link_libraries("AutkCore" PRIVATE "kernel32")
elseif(CMAKE_SYSTEM_NAME IN_LIST UNIX_SYSTEM_NAMES)
    target_sources("AutkCore" PRIVATE ${CORE_SOURCES_UNIX})
    target_link_libraries("AutkCore" PRIVATE "dl")

    if(AUTK_ICONV)
        find_package("Iconv" REQUIRED)
        target_link_libraries("AutkCore" PRIVATE "Iconv::Iconv")
    endif()
else()
    message(FATAL_ERROR "AutkCore does not support platform '${CMAKE_SYSTEM_NAME}'")
endif()
