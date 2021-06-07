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

#ifndef AUTK_APPLICATION_H_INCLUDED
#define AUTK_APPLICATION_H_INCLUDED

#include "autk/object.h"

namespace autk {

    /// \cond IMPL
    namespace impl {

        class Display_driver;

    } // namespace impl
    /// \endcond

    /// Encapsulates most of an `autk` application's global state.
    class AUTK_IMPORT Application : public Object {
    public:
        /// \throw std::logic_error if more than one Application is created in a process.
        Application();

        Application(Application&&) = delete;
        Application(const Application&) = delete;
        ~Application();

        /// Returns the global application instance.
        /// \throw std::logic_error if the application has not yet been created or if it has been destroyed.
        static Application& instance();

        Application& operator=(Application&&) = delete;
        Application& operator=(const Application&) = delete;

    private:
        impl::Display_driver* display_ = nullptr;
    };

} // namespace autk

#endif // AUTK_APPLICATION_H_INCLUDED
