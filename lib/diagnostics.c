// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#include <stdio.h>
#include <stdlib.h>

#include "diagnostics_internal.h"
#include "string_utils.h"

struct message_state {
    AutkMessageFunc func;
    void *ctx;
    autk_message_severity_t min_severity;
};

static struct message_state s_message_state = {
    .func = autk_message_default,
};

//==============================================================================
//
// Message formatting internals
//
//==============================================================================

struct message_ctx {
    autk_message_severity_t severity;
    const char *module_name;
    const autk_source_location_t *location;
};

static void
do_message_formatted(void *ctx_ptr, const char *msg, AUTK_UNUSED size_t len)
{
    struct message_ctx *ctx = ctx_ptr;

    s_message_state.func(s_message_state.ctx, ctx->severity, ctx->module_name, ctx->location, msg);
}

static autk_status_t
do_message_vf(autk_message_severity_t severity, const char *module_name,
              const autk_source_location_t *location, const char *fmt, va_list args)
{
    struct message_ctx ctx = {
        .severity = severity,
        .module_name = module_name,
        .location = location,
    };

    return autk_call_with_formatted_string_vf(AUTK_FMT_ALLOCATE | AUTK_FMT_ALLOW_TRUNCATE,
                                              do_message_formatted, &ctx, fmt, args);
}

//==============================================================================
//
// Public API
//
//==============================================================================

AUTK_API AUTK_NORETURN void
autk_assert_failure(const char *module_name, const autk_source_location_t *location,
                    const char *expr)
{
    autk_message_f(AUTK_MESSAGE_SEVERITY_FATAL, module_name, location, "Assertion failed: %s",
                   expr ? expr : "<null>");
    autk_fatal_exit();
}

AUTK_API AUTK_NORETURN void
autk_expect_failure(const char *module_name, const autk_source_location_t *location,
                    const char *expr, autk_status_t status)
{
    char status_buf[80];

    autk_status_to_string(status, status_buf, sizeof(status_buf));
    autk_message_f(AUTK_MESSAGE_SEVERITY_FATAL, module_name, location,
                   "AUTK_EXPECT(%s) failed with: %s", expr ? expr : "<null>", status_buf);
    autk_fatal_exit();
}

AUTK_API void
autk_fatal_exit(void)
{
    exit(EXIT_FAILURE);
}

AUTK_API void
autk_fatal_message(const char *module_name, const autk_source_location_t *location,
                   const char *message)
{
    if (s_message_state.func) {
        autk_message(AUTK_MESSAGE_SEVERITY_FATAL, module_name, location, message);
    }
    autk_fatal_exit();
}

AUTK_API void
autk_fatal_message_f(const char *module_name, const autk_source_location_t *location,
                     const char *fmt, ...)
{
    va_list args;

    if (s_message_state.func) {
        va_start(args, fmt);
        do_message_vf(AUTK_MESSAGE_SEVERITY_FATAL, module_name, location, fmt, args);
        va_end(args);
    }
    autk_fatal_exit();
}

AUTK_API void
autk_fatal_message_vf(const char *module_name, const autk_source_location_t *location,
                      const char *fmt, va_list args)
{
    if (s_message_state.func) {
        do_message_vf(AUTK_MESSAGE_SEVERITY_FATAL, module_name, location, fmt, args);
    }
    autk_fatal_exit();
}

AUTK_API void
autk_message(autk_message_severity_t severity, const char *module_name,
             const autk_source_location_t *location, const char *message)
{
    if (s_message_state.func && severity >= s_message_state.min_severity) {
        s_message_state.func(s_message_state.ctx, severity, module_name, location,
                             message ? message : "<null>");
    }
    if (severity == AUTK_MESSAGE_SEVERITY_FATAL) {
        autk_fatal_exit();
    }
}

AUTK_API autk_status_t
autk_message_f(autk_message_severity_t severity, const char *module_name,
               const autk_source_location_t *location, const char *AUTK_FMTSTR fmt, ...)
{
    va_list args;
    autk_status_t status = AUTK_OK;

    if (s_message_state.func && severity >= s_message_state.min_severity) {
        va_start(args, fmt);
        status = do_message_vf(severity, module_name, location, fmt, args);
        va_end(args);
    }
    if (severity == AUTK_MESSAGE_SEVERITY_FATAL) {
        autk_fatal_exit();
    }

    return status;
}

AUTK_API autk_status_t
autk_message_vf(autk_message_severity_t severity, const char *module_name,
                const autk_source_location_t *location, const char *AUTK_FMTSTR fmt, va_list args)
{
    autk_status_t status = AUTK_OK;

    if (s_message_state.func && severity >= s_message_state.min_severity) {
        status = do_message_vf(severity, module_name, location, fmt, args);
    }
    if (severity == AUTK_MESSAGE_SEVERITY_FATAL) {
        autk_fatal_exit();
    }

    return status;
}

AUTK_API void
autk_set_message_handler(AutkMessageFunc func, void *ctx)
{
    s_message_state.func = func;
    s_message_state.ctx = ctx;
}

AUTK_API void
autk_set_min_message_severity(autk_message_severity_t severity)
{
    s_message_state.min_severity = severity;
}

AUTK_API char *
autk_status_to_string(autk_status_t status, char *buf, size_t buf_size)
{
    static const char *const strings[] = {
#define AUTK_DO_(IDENT, MSG) [IDENT] = MSG,
        AUTK_FOR_EACH_STATUS(AUTK_DO_)
#undef AUTK_DO_
    };

    if (status < AUTK_LENGTHOF(strings) && strings[status]) {
        return autk_strcpy_max(buf, strings[status], buf_size);
    } else if (snprintf(buf, buf_size, "Autk status code %" PRIu32, status) > 0) {
        return buf;
    } else {
        return autk_strcpy_max(buf, "Unknown Autk status code", buf_size);
    }
}

//==============================================================================
//
// Internals
//
//==============================================================================

AUTK_HIDDEN void
autk_message_stderr_impl(autk_message_flags_t flags, autk_message_severity_t severity,
                         const char *module_name, const autk_source_location_t *location,
                         const char *message)
{
    static const struct {
        autk_message_severity_t severity;
        const char *plain_str;
        const char *color_str;
    } severity_strings[] = {
        {AUTK_MESSAGE_SEVERITY_TRACE, "[trace] ", "\033[34m[trace]\033[0m "},
        {AUTK_MESSAGE_SEVERITY_DEBUG, "[debug] ", "\033[36m[debug]\033[0m "},
        {AUTK_MESSAGE_SEVERITY_INFO, "[info] ", "\033[32m[info]\033[0m "},
        {AUTK_MESSAGE_SEVERITY_WARNING, "[warning] ", "\033[1;33m[warning]\033[0m "},
        {AUTK_MESSAGE_SEVERITY_ERROR, "[error] ", "\033[1;31m[error]\033[0m "},
        {AUTK_MESSAGE_SEVERITY_FATAL, "[FATAL] ", "\033[1;31m[FATAL]\033[0m "},
    };

    bool ansi_escapes = (flags & AUTK_MESSAGE_ANSI_ESCAPES);
    const char *severity_str = NULL;
    const char *pos = message ? message : "<null>";
    const char *run_start;
    size_t write_result;

    for (size_t i = 0; i < AUTK_LENGTHOF(severity_strings); i++) {
        if (severity == severity_strings[i].severity) {
            if (ansi_escapes) {
                severity_str = severity_strings[i].color_str;
            } else {
                severity_str = severity_strings[i].plain_str;
            }
            break;
        }
    }

    if (severity_str) {
        fputs(severity_str, stderr);
    } else {
        fprintf(stderr, "[severity %d] ", severity);
    }

    if (module_name) {
        if (ansi_escapes) {
            fprintf(stderr, "\033[35m[%s]\033[0m ", module_name);
        } else {
            fprintf(stderr, "[%s] ", module_name);
        }
    }

    if (location && location->file_name) {
        if (ansi_escapes) {
            fprintf(stderr, "\033[2;36m(%s:%" PRIu32 ")\033[0m ", location->file_name,
                    location->line_num);
        } else {
            fprintf(stderr, "(%s:%" PRIu32 ") ", location->file_name, location->line_num);
        }
    }

    if (location && location->func_name) {
        if (ansi_escapes) {
            fprintf(stderr, "\033[2;35m(in %s)\033[0m ", location->func_name);
        } else {
            fprintf(stderr, "(in %s) ", location->func_name);
        }
    }

    if (ansi_escapes && severity == AUTK_MESSAGE_SEVERITY_TRACE) {
        fputs("\033[2m", stderr);
    }

    while (*pos) {
        if ((unsigned char)*pos <= 0x1F) {
            // Escape control chars to avoid clobbering the terminal output.
            switch (*pos) {
                case '\t':
                    fputc('\t', stderr);
                    break;
                case '\r':
                    // Treat DOS and Classic Mac line endings as '\n'.
                    if (pos[1] == '\n') {
                        pos++;
                    }
                    AUTK_FALLTHROUGH
                case '\n':
                    // Skip trailing line ending.
                    if (pos[1]) {
                        // Indent continuation lines.
                        fputs("\n\t", stderr);
                    }
                    break;
                default:
                    fprintf(stderr, "%%%02x", (unsigned char)*pos);
                    break;
            }
            pos++;
        } else {
            // Write consecutive non-control chars in one call for efficiency.
            run_start = pos++;
            while ((unsigned char)*pos >= 0x20) {
                pos++;
            }

            do {
                write_result = fwrite(run_start, 1, (size_t)(pos - run_start), stderr);
                if (!write_result) {
                    // Give up if writing failed.
                    return;
                }
                run_start += write_result;
            } while (run_start < pos);
        }
    }

    if (ansi_escapes) {
        fputs("\033[0m\n", stderr);
    } else {
        fputc('\n', stderr);
    }

#ifdef _WIN32
    fflush(stderr);
#endif
}
