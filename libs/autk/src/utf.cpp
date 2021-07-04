/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This software is subject to the terms of the Boost Software License version
 * 1.0. See the included LICENSE file for more information. If you did not
 * receive a copy of the license, see <https://www.boost.org/LICENSE_1_0.txt>.
 */

#include "utf.h"

using namespace autk;
using namespace autk::impl;

Utf_error::Utf_error(Utf_status status)
    : Encoding_error{strerror(status)}
{
}

Utf_error::Utf_error(Utf_status status, std::size_t index)
    : Encoding_error{std::string{strerror(status)} + " at index " + std::to_string(index)}
{
}

Utf_error::~Utf_error()
{
}

//----------------------------------------------------------------------------------------------------------------------

const char* impl::strerror(Utf_status status) noexcept
{
    switch (status) {
    case Utf_status::ok:
        return "The operation was successful";
    case Utf_status::empty:
        return "Empty input string";
    case Utf_status::not_optimal:
        return "Non-optimal UTF-8 sequence";
    case Utf_status::unpaired_surrogate:
        return "Unpaired UTF-16 surrogate";
    case Utf_status::reserved:
        return "Code point reserved for UTF-16 surrogates";
    case Utf_status::incomplete:
        return "Incomplete input sequence";
    case Utf_status::out_of_range:
        return "Code point out of range";
    case Utf_status::invalid:
        return "Invalid sequence";
    default:
        return "UTF error";
    }
}
