/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <autk/core/utf.h>

using namespace autk;

//==============================================================================
// UtfError
//==============================================================================

UtfError::UtfError(UtfStatus status)
    : EncodingError{std::string{strerror(status)}}
{
}

UtfError::UtfError(UtfStatus status, size_t index)
    : EncodingError{std::string{strerror(status)} + " at index " + std::to_string(index)}
{
}

//==============================================================================
// autk
//==============================================================================

const char* autk::strerror(UtfStatus status) noexcept
{
    switch (status) {
    case UtfStatus::ok:
        return "No error";
    case UtfStatus::empty:
        return "Empty input sequence";
    case UtfStatus::not_optimal:
        return "Non-optimal UTF-8 sequence";
    case UtfStatus::unpaired_surrogate:
        return "Unpaired UTF-16 surrogate";
    case UtfStatus::reserved:
        return "Code point reserved";
    case UtfStatus::incomplete:
        return "Incomplete UTF sequence";
    case UtfStatus::out_of_range:
        return "Code point out of range";
    case UtfStatus::invalid:
        return "UTF sequence not understood";
    default:
        return "UTF encoding error";
    }
}
