/*
 * Copyright (c) 2026 Martin Mills
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <autk/diagnostics.h>
#include <os/compat.h>

AUTK_API AUTK_NORETURN void
autk_default_expect_failed(const char *expr, autk_status_t status, const char *module_name,
                           const autk_source_location_t *location)
{
    char full_msg[512];
    const char *msg_ptr;

    if (snprintf(full_msg, sizeof(full_msg), "AUTK_EXPECT failed with '%s': %s",
                 autk_status_to_string(status), expr)
        >= 0)
    {
        msg_ptr = full_msg;
    } else {
        msg_ptr = autk_status_to_string(status);
    }

    autk_stderr_message(NULL, AUTK_MESSAGE_SEVERITY_FATAL, module_name, location, msg_ptr);
    exit(EXIT_FAILURE);
}

AUTK_API const char *
autk_memory_tag_to_string(autk_memory_tag_t tag)
{
    switch (tag) {
#define AUTK_DO(e, s)                                                                              \
    case e:                                                                                        \
        return s;
        AUTK_FOREACH_MEMORY_TAG(AUTK_DO)
#undef AUTK_DO
        default:
            return "unknown";
    }
}

AUTK_API const char *
autk_status_to_string(autk_status_t status)
{
    switch (status) {
#define AUTK_DO(e, s)                                                                              \
    case e:                                                                                        \
        return s;
        AUTK_FOREACH_STATUS(AUTK_DO)
#undef AUTK_DO
        default:
            return "Unknown error";
    }
}

AUTK_API void
autk_stderr_message(void *unused, autk_message_severity_t severity, const char *module_name,
                    const autk_source_location_t *location, const char *message)
{
    const char *severity_str;

    (void)unused;

    switch (severity) {
        case AUTK_MESSAGE_SEVERITY_DEBUG:
            severity_str = "debug: ";
            break;
        case AUTK_MESSAGE_SEVERITY_WARN:
            severity_str = "warning: ";
            break;
        case AUTK_MESSAGE_SEVERITY_ERROR:
            severity_str = "error: ";
            break;
        case AUTK_MESSAGE_SEVERITY_FATAL:
            severity_str = "fatal error: ";
            break;
        default:
            severity_str = "unknown severity: ";
            break;
    }

    fputs(severity_str, stderr);

    if (module_name) {
        fprintf(stderr, "[%s]: ", module_name);
    }

    if (location && location->func_name) {
        fprintf(stderr, "in %s: ", location->func_name);
    }

    if (location && location->file_name) {
        fprintf(stderr, "(%s:%" PRIu32 "): ", location->file_name, location->line_num);
    }

    fputs(message ? message : "<null>", stderr);
    fputc('\n', stderr);

#ifdef _WIN32
    fflush(stderr);
#endif
}
