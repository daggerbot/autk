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

#ifndef AUTK_OBJECT_H_INCLUDED
#define AUTK_OBJECT_H_INCLUDED

#include "autk/config.h"

namespace autk {

    /// Base class for signal-handling objects.
    class AUTK_IMPORT Object {
    public:
        Object();
        Object(Object&&) = delete;
        Object(const Object&) = delete;
        virtual ~Object() = 0;

        Object& operator=(Object&&) = delete;
        Object& operator=(const Object&) = delete;
    };

} // namespace autk

#endif // AUTK_OBJECT_H_INCLUDED
