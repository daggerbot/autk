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

#ifndef AUTK_CONCEPTS_H_INCLUDED
#define AUTK_CONCEPTS_H_INCLUDED

#include <type_traits>

namespace autk {

    class Object;

    /// Concept for types derived from Object.
    template<typename T>
    concept Any_object = std::is_base_of_v<Object, T>;

    /// Concept for types that are suitable as signal arguments.
    template<typename T>
    concept Signal_arg = std::is_trivially_copyable_v<T> || std::is_reference_v<T>;

} // namespace autk

#endif // AUTK_CONCEPTS_H_INCLUDED
