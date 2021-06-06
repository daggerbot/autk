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

#ifndef AUTK_TYPES_H_INCLUDED
#define AUTK_TYPES_H_INCLUDED

#include "autk/config.h"

/// \def AUTK_T
/// Encodes the following string literal as an autk::Os_char array.
#ifdef _WIN32
# define AUTK_T L""
#else
# define AUTK_T
#endif

namespace autk {

    /// \typedef Os_char
    /// Preferred character type when using the underlying OS APIs.
    /// This is `wchar_t` on Windows and `char` on other platforms.
#ifdef _WIN32
    using Os_char = wchar_t;
#else
    using Os_char = char;
#endif

    /// Base for types that cannot be instantiated.
    struct No_inst {
        No_inst() = delete;
        No_inst(No_inst&&) = delete;
        No_inst(const No_inst&) = delete;
        No_inst& operator=(No_inst&&) = delete;
        No_inst& operator=(const No_inst&) = delete;
    };

} // namespace autk

#endif // AUTK_TYPES_H_INCLUDED
