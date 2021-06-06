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

#include <climits>
#include <cstddef>
#include <array>
#include <limits>
#include <type_traits>

#include "autk/platform/string.h"

using namespace autk;
using namespace autk::impl;

//----------------------------------------------------------------------------------------------------------------------

Encoding_error::Encoding_error()
    : what_{"Encoding error"}
{
}

Encoding_error::Encoding_error(std::string&& what_arg)
    : what_{std::move(what_arg)}
{
}

Encoding_error::~Encoding_error()
{
}

//----------------------------------------------------------------------------------------------------------------------

Utf_error::Utf_error()
{
}

Utf_error::Utf_error(Utf_status status)
    : Encoding_error{strerror(status)}
    , status_{status}
{
}

Utf_error::Utf_error(Utf_status status, std::size_t index)
    : Encoding_error{strerror(status) + " at index " + std::to_string(index)}
    , status_{status}
    , index_{index}
{
}

Utf_error::~Utf_error()
{
}

//----------------------------------------------------------------------------------------------------------------------

std::string impl::strerror(Utf_status status)
{
    switch (status) {
    case Utf_status::ok:
        return "No error";
    case Utf_status::empty:
        return "Empty input string";
    case Utf_status::not_optimal:
        return "Non-optimal UTF-8 sequence";
    case Utf_status::unpaired_surrogate:
        return "Unpaired UTF-16 surrogate";
    case Utf_status::reserved:
        return "Code point reserved for UTF-16 surrogates";
    case Utf_status::incomplete:
        return "Incomplete sequence";
    case Utf_status::out_of_range:
        return "Code point out of range";
    case Utf_status::invalid:
        return "Invalid sequence";
    default:
        return "UTF encoding error";
    }
}
