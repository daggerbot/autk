/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <autk/core/exception.h>

using namespace autk;

//==============================================================================
// Exception
//==============================================================================

Exception::Exception(std::string&& what_arg)
    : what_{std::make_shared<std::string>(std::move(what_arg))}
{
}

Exception::~Exception()
{
}

const char* Exception::what() const noexcept
{
    return what_->c_str();
}
