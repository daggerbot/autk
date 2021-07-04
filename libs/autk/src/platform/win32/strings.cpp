/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#include <autk/strings.h>
#include "../../utf.h"

using namespace autk;
using namespace autk::impl;

//----------------------------------------------------------------------------------------------------------------------

Os_string autk::to_os_string(std::u8string_view str)
{
    return utf::convert<wchar_t>(str);
}

Os_string autk::to_os_string_lossy(std::u8string_view str)
{
    return utf::convert_lossy<wchar_t>(str);
}

std::u8string autk::to_utf8(Os_string_view str)
{
    return utf::convert<char8_t>(str);
}

std::u8string autk::to_utf8_lossy(Os_string_view str)
{
    return utf::convert_lossy<char8_t>(str);
}
