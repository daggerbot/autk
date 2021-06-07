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

#include <cstring>

#include <iconv.h>

#include "autk/external/iconv.h"

using namespace autk;
using namespace autk::impl;

Iconv_error::Iconv_error(std::string&& what_arg, int error_code)
    : Encoding_error{what_arg + ": " + std::strerror(error_code)}
{
}

Iconv_error::~Iconv_error()
{
}
