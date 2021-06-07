// Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <mutex>
#include <stdexcept>

#include "autk/driver/display.h"
#include "autk/application.h"

using namespace autk;
using namespace autk::impl;

namespace {

    Application* instance = nullptr;
    bool instance_init = false;
    std::once_flag instance_init_flag{};

} // namespace

//----------------------------------------------------------------------------------------------------------------------

Application::Application()
    : display_{&Display_driver::get_main()}
{
    // Make sure only one instance is ever created.
    std::call_once(instance_init_flag, [this] {
        ::instance = this;
        ::instance_init = true;
    });

    if (::instance != this) {
        throw std::logic_error{"Multiple application instances created"};
    }
}

Application::~Application()
{
    ::instance = nullptr;
}

Application& Application::instance()
{
    if (!::instance_init) {
        throw std::logic_error{"No application instance created"};
    } else if (!::instance) {
        throw std::logic_error{"Application instance destroyed"};
    }

    return *::instance;
}
