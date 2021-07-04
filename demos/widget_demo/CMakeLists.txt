# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This software is subject to the terms of the Boost Software License version
# 1.0. See the included LICENSE file for more information. If you did not
# receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.

add_executable("widget_demo" WIN32
    "src/main.cpp"
)
set_target_properties("widget_demo" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
target_link_libraries("widget_demo" "autk_main")

if(COMMON_CXX_FLAGS)
    target_compile_options("widget_demo" PRIVATE ${COMMON_CXX_FLAGS})
endif()

if(COMMON_DEFS)
    target_compile_definitions("widget_demo" PRIVATE ${COMMON_DEFS})
endif()
