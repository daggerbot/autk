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

#include <autk/autk.h>

// Provide a handler for when `AUTK_EXPECT` fails.
void
autk_expect_failed(const char *expr, autk_status_t status, const char *module_name,
                   const autk_source_location_t *location)
{
    // Use the default behavior.
    autk_default_expect_failed(expr, status, module_name, location);
}

int
main(int argc, char *argv[])
{
    static const autk_instance_create_params_t instance_params = {
        .struct_size = sizeof(autk_instance_create_params_t),
        .message_func = &autk_stderr_message, // print messages to stderr
    };
    static const autk_window_callbacks_t window_callbacks = {
        .struct_size = sizeof(autk_window_callbacks_t),
        .close_requested = &autk_window_callback_quit, // quit the app when the window is closed
    };
    static const autk_window_create_params_t window_params = {
        .struct_size = sizeof(autk_window_create_params_t),
        .title = "Hello!",
        .callbacks = &window_callbacks,
    };

    autk_instance_t *instance;
    autk_client_t *client;
    autk_window_t *window;

    (void)argc;
    (void)argv;

    // Create the Autk library instance.
    AUTK_EXPECT(autk_instance_create(&instance_params, &instance));

    // Create the client connection.
    AUTK_EXPECT(autk_client_create(instance, NULL, &client));

    // Create the main window.
    AUTK_EXPECT(autk_window_create(client, &window_params, &window));
    AUTK_EXPECT(autk_window_set_visible(window, true));

    // Main loop.
    AUTK_EXPECT(autk_client_run(client));

    // Clean up.
    autk_window_destroy(window);
    autk_client_destroy(client);
    autk_instance_destroy(instance);

    return 0;
}
