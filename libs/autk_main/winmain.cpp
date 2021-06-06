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

#include "autk/main.h"
#include "autk/string.h"

#include <vector>

#include <windows.h>

using namespace autk;

extern "C" int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    int argc = __argc;
    wchar_t** wargv = __wargv;
    std::vector<Os_string> str_args;
    std::vector<const Os_char*> ptr_args;

    for (int i = 0; i < argc; ++i) {
        if (wargv && wargv[i]) {
            str_args.push_back(wargv[i]);
        } else {
            str_args.push_back({});
        }
    }

    for (const auto& str : str_args) {
        ptr_args.push_back(str.c_str());
    }

    return ::autk_main(argc, ptr_args.data());
}
