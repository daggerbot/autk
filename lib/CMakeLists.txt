# Copyright (c) 2026 Martin Mills
# SPDX-License-Identifier: ISC
# See LICENSE.txt for more information.

#===============================================================================
#
# autk-public
#
#===============================================================================

add_library(autk-public INTERFACE)
target_include_directories(autk-public INTERFACE "${PROJECT_SOURCE_DIR}/include")

#===============================================================================
#
# autk-private
#
#===============================================================================

add_library(autk-private INTERFACE)

target_sources(autk-private INTERFACE
    diagnostics.c
    init.c
    string_utils.c
)

target_compile_definitions(autk-private INTERFACE
    "AUTK_MODULE_NAME=\"autk\""
)

target_link_libraries(autk-private INTERFACE autk-compiler-options)

#===============================================================================
#
# autk-shared
#
#===============================================================================

if(AUTK_SHARED)
    add_library(autk-shared SHARED)
    set_target_properties(autk-shared PROPERTIES OUTPUT_NAME autk)
    target_link_libraries(autk-shared PUBLIC autk-public PRIVATE autk-private)

    install(TARGETS autk-shared
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

    add_library(autk-default INTERFACE)
    target_link_libraries(autk-default INTERFACE autk-shared)
endif()

#===============================================================================
#
# autk-static
#
#===============================================================================

if(AUTK_STATIC)
    add_library(autk-static STATIC)
    target_link_libraries(autk-static PUBLIC autk-public PRIVATE autk-private)

    install(TARGETS autk-static
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")

    if(NOT AUTK_SHARED)
        add_library(autk-default INTERFACE)
        target_link_libraries(autk-default INTERFACE autk-static)
    endif()
endif()

#===============================================================================
#
# Platform configuration
#
#===============================================================================

if(UNIX)
    target_sources(autk-private INTERFACE
        os/posix/diagnostics.c
        os/posix/dylib.c
    )
elseif(WIN32)
    target_link_options(autk-default INTERFACE -municode)
    target_sources(autk-private INTERFACE
        driver/windows/init.c

        os/windows/diagnostics.c
    )
endif()

#===============================================================================
#
# X11 driver support
#
#===============================================================================

if(AUTK_X11)
    target_compile_definitions(autk-private INTERFACE AUTK_X11=1)
    target_sources(autk-private INTERFACE
        driver/x11/init.c
        driver/x11/library.c
    )

    if(AUTK_X11_DLOPEN)
        target_compile_definitions(autk-private INTERFACE AUTK_X11_DLOPEN=1)
        target_include_directories(autk-private INTERFACE ${xcb_INCLUDE_DIRS})
    else()
        target_link_libraries(autk-private INTERFACE PkgConfig::xcb)
    endif()
endif()
