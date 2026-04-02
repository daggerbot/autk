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

if(AUTK_SHARED)
    add_library(autk SHARED)
else()
    add_library(autk STATIC)
endif()

target_sources(autk PRIVATE
    core/client.c
    core/device.c
    core/diagnostics.c
    core/instance.c
    core/style.c
    core/window.c

    ext/style_ext_base.c
    ext/win9x_style.c

    os/compat.c

    utility/encoding.c
    utility/hash.c
    utility/math.c
    utility/job_queue.c
)

target_compile_definitions(autk
    PRIVATE
        AUTK_API=AUTK_EXPORT
        AUTK_MODULE_NAME=\"autk\"
        AUTK_MESSAGE_SOURCE_LOCATION=1
)

target_include_directories(autk
    PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        "${GENERATED_INCLUDE_DIR}"
    PRIVATE
        .
)

target_link_libraries(autk
    PRIVATE
        autk-compiler-options
)

if(WIN32)
    target_sources(autk PRIVATE
        os/windows/sync.c
        os/windows/system.c
    )
elseif(LINUX OR BSD)
    target_sources(autk PRIVATE
        os/posix/job_queue.c
        os/posix/sync.c
    )
endif()

#===============================================================================
#
# X11 client support
#
#===============================================================================

if(AUTK_CLIENT_X11)
    if(NOT PkgConfig_FOUND)
        find_package(PkgConfig REQUIRED)
    endif()

    if(NOT TARGET PkgConfig::xcb)
        pkg_check_modules(xcb REQUIRED IMPORTED_TARGET xcb)
    endif()

    target_sources(autk PRIVATE
        client/x11/client.c
        client/x11/window.c
    )

    target_link_libraries(autk PRIVATE PkgConfig::xcb)
endif()

#===============================================================================
#
# Windows support
#
#===============================================================================

if(WIN32)
    target_sources(autk PRIVATE
        client/windows/client.c
        client/windows/window.c
    )

    target_compile_definitions(autk PRIVATE
        _UNICODE
        UNICODE
    )

    target_link_libraries(autk PRIVATE
        user32
    )
endif()
