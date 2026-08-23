// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#ifndef AUTK_DIAGNOSTICS_INTERNAL_H_
#define AUTK_DIAGNOSTICS_INTERNAL_H_

#include <autk/diagnostics.h>

typedef uint32_t autk_message_flags_t; ///< \see \ref autk_message_flags

enum autk_message_flags {
    AUTK_MESSAGE_ANSI_ESCAPES = 1 << 0,
};

AUTK_HIDDEN void
autk_message_stderr_impl(autk_message_flags_t flags, autk_message_severity_t severity,
                         const char *module_name, const autk_source_location_t *location,
                         const char *message);

#endif // AUTK_DIAGNOSTICS_INTERNAL_H_
