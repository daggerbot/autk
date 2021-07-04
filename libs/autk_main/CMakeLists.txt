# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This software is subject to the terms of the Boost Software License version
# 1.0. See the included LICENSE file for more information. If you did not
# receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.

add_library("autk_main" OBJECT "src/main.cpp")
target_include_directories("autk_main" PUBLIC "include")
target_link_libraries("autk_main" PUBLIC "autk")

if(COMMON_CXX_FLAGS)
    target_compile_options("autk_main" PRIVATE ${COMMON_CXX_FLAGS})
endif()

if(COMMON_DEFS)
    target_compile_definitions("autk_main" PRIVATE ${COMMON_DEFS})
endif()

if(MINGW)
    target_link_options("autk_main" INTERFACE "-municode" "-mwindows")
endif()

install(FILES "$<TARGET_OBJECTS:autk_main>"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RENAME "autk_main${CMAKE_CXX_OUTPUT_EXTENSION}")

install(DIRECTORY "include/autk"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
