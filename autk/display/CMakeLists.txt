# Copyright (c) Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set(DISPLAY_SOURCES
    "context.cpp"
    "display.cpp"
)
set(DISPLAY_SOURCES_X11
    "x11/context.cpp"
    "x11/library.cpp"
)

#===============================================================================
# Build AutkDisplay library
#===============================================================================

if(AUTK_SHARED)
    add_library("AutkDisplay" SHARED ${DISPLAY_SOURCES})
else()
    add_library("AutkDisplay" STATIC ${DISPLAY_SOURCES})
    set_target_properties("AutkDisplay" PROPERTIES POSITION_INDEPENDENT_CODE "${AUTK_PIC}")
endif()

set_target_properties("AutkDisplay" PROPERTIES
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    VERSION "${PROJECT_VERSION}"
)

target_compile_definitions("AutkDisplay" PRIVATE "AUTK_DISPLAY=AUTK_EXPORT")
target_include_directories("AutkDisplay" PUBLIC "include")
target_link_libraries("AutkDisplay" PUBLIC "AutkCore" PRIVATE "AutkCommon")

install(TARGETS "AutkDisplay"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(DIRECTORY "include/autk"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    PATTERN "x11" EXCLUDE)

#===============================================================================
# Configure target platform
#===============================================================================

if(CMAKE_SYSTEM_NAME IN_LIST UNIX_SYSTEM_NAMES)
    find_package("PkgConfig" REQUIRED)
    set(AUTK_DISPLAY_WAYLAND OFF PARENT_SCOPE) # Planned but not yet implemented.

    if(AUTK_DISPLAY_X11 STREQUAL "AUTO")
        pkg_check_modules("X11" IMPORTED_TARGET "x11" "x11-xcb" "xcb")
        if(TARGET "PkgConfig::X11")
            set(AUTK_DISPLAY_X11 ON PARENT_SCOPE)
        endif()
    elseif(AUTK_DISPLAY_X11)
        pkg_check_modules("X11" REQUIRED IMPORTED_TARGET "x11" "x11-xcb" "xcb")
    endif()

    if(AUTK_DISPLAY_X11)
        target_sources("AutkDisplay" PRIVATE ${DISPLAY_SOURCES_X11})
        target_link_libraries("AutkDisplay" PUBLIC "PkgConfig::X11")
        install(DIRECTORY "include/autk/display/x11"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/autk/display")
    endif()

    if(NOT AUTK_DISPLAY_WAYLAND AND NOT AUTK_DISPLAY_X11)
        message(FATAL_ERROR "No AutkDisplay drivers enabled")
    endif()
else()
    message(FATAL_ERROR "AutkDisplay does not support platform '${CMAKE_SYSTEM_NAME}'")
endif()
