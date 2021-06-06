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

set(WIDGET_DEMO_SOURCES
    "main.cpp"
)

add_executable("widget_demo" WIN32 ${WIDGET_DEMO_SOURCES})
target_link_libraries("widget_demo" PRIVATE "autk_main")

if(COMMON_CXX_FLAGS)
    target_compile_options("widget_demo" PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${COMMON_CXX_FLAGS}>)
endif()

if(COMMON_DEFINITIONS)
    target_compile_definitions("widget_demo" PRIVATE ${COMMON_DEFINITIONS})
endif()
