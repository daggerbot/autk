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

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_library("autk_main" OBJECT "winmain.cpp")
else()
    add_library("autk_main" OBJECT "main.cpp")
endif()

target_include_directories("autk_main" PUBLIC "include")
target_link_libraries("autk_main" PUBLIC "autk")

if(COMMON_CXX_FLAGS)
    target_compile_options("autk_main" PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${COMMON_CXX_FLAGS}>)
endif()

if(COMMON_DEFINITIONS)
    target_compile_definitions("autk_main" PRIVATE ${COMMON_DEFINITIONS})
endif()

if(MINGW)
    target_link_options("autk_main" INTERFACE "-municode" "-mwindows")
endif()

install(FILES $<TARGET_OBJECTS:autk_main>
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RENAME autk_main$<IF:$<BOOL:${CMAKE_CXX_OUTPUT_EXTENSION}>,${CMAKE_CXX_OUTPUT_EXTENSION},.o>)
install(DIRECTORY "include/autk"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
