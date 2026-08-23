// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../diagnostics_internal.h"

struct posix_message_state {
    pthread_mutex_t mutex;
    bool ansi_escapes;
};

static struct posix_message_state s_posix_message_state = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
};

AUTK_API autk_status_t
autk_console_init(AUTK_UNUSED autk_console_mode_t mode)
{
    static const char *const known_color_terms[] = {
        /* clang-format off */
        "linux",
        "rxvt",
        "screen",
        "st",
        "tmux",
        "xterm",
        /* clang-format on */
    };
    static char stderr_buf[BUFSIZ];

    char *term = getenv("TERM");
    char *no_color = getenv("NO_COLOR");
    char *force_color = getenv("FORCE_COLOR");

    // Buffered stderr improves logging performance by minimizing write() syscalls.
    // Line buffering ensures it's flushed at each line ending automatically.
    setvbuf(stderr, stderr_buf, _IOLBF, sizeof(stderr_buf));

    // Check if ANSI colors should be enabled.
    if (no_color && no_color[0]) {
        s_posix_message_state.ansi_escapes = false;
    } else if (force_color && force_color[0]) {
        s_posix_message_state.ansi_escapes = true;
    } else if (term && term[0] && isatty(STDERR_FILENO)) {
        s_posix_message_state.ansi_escapes = false;
        for (size_t i = 0; i < AUTK_LENGTHOF(known_color_terms); i++) {
            if (strcmp(term, known_color_terms[i]) == 0) {
                // Exact TERM match.
                s_posix_message_state.ansi_escapes = true;
                break;
            } else if (strstr(term, known_color_terms[i]) == term
                       && term[strlen(known_color_terms[i])] == '-')
            {
                // TERM- prefix match.
                s_posix_message_state.ansi_escapes = true;
                break;
            }
        }
    } else {
        s_posix_message_state.ansi_escapes = false;
    }

    return AUTK_OK;
}

AUTK_API void
autk_message_default(AUTK_UNUSED void *unused_ctx, autk_message_severity_t severity,
                     const char *module_name, const autk_source_location_t *location,
                     const char *message)
{
    if (pthread_mutex_lock(&s_posix_message_state.mutex) != 0) {
        return;
    }
    autk_message_stderr_impl(s_posix_message_state.ansi_escapes ? AUTK_MESSAGE_ANSI_ESCAPES : 0,
                             severity, module_name, location, message);
    pthread_mutex_unlock(&s_posix_message_state.mutex);
}
