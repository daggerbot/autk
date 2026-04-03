# Autk: A systems-level widget toolkit for traditional desktop applications.

[![License: ISC](https://img.shields.io/badge/License-ISC-blue.svg)](https://opensource.org/licenses/ISC)

## Key Features

- **Zero Bloat**: No GObject systems, no custom meta-object compilers, no high-level standard
  library replacement, and no "bringing web technology to the desktop".
- **Explicit API**: A Vulkan-inspired architecture using `create_params` structs and explicit handle
  management for total control over application state.
- **Desktop-First**: Not a ported mobile framework. Designed specifically for Linux, BSD, and
  Windows. (macOS support is a goal, but I don't currently have the means to provide it myself.)
- **ABI Stability Guaranteed**: Uses `struct_size` versioning to ensure minor and patch releases
  remain binary compatible. Future versions can add more struct fields as needed without requiring
  recompilation of existing applications.
- **Custom Memory Management**: Minimizes allocations and supports custom allocators for
  high-performance or constrained environments, or for better debugging.
- **Pluggable Aesthetics**: Designed to support drop-in widget styles, allowing for classic
  Win9x/Platinum/Motif looks or modern interfaces without the toolkit forcing a specific "brand" on
  the developer or the user.

## Status

Very early. Not even alpha. Right now I'm primarily writing X11 and Windows back ends. Once the
library matures a bit, I plan to add Wayland support. macOS support is desired, but I don't
currently have the means to provide it myself. I may reach out for help with this in the future.

## Hello World (WIP)

```c
#include <autk/autk.h>

int main(int argc, char *argv[])
{
    static const autk_instance_create_params_t instance_params = {
        .struct_size = sizeof(autk_instance_create_params_t),
        .message_func = &autk_stderr_message, // print log messages to stderr
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

    // Create the Autk library instance.
    // (`AUTK_EXPECT` is a wrapper for functions that return `autk_status_t` error codes.
    // On failure, it invokes the user-defined handler below).
    AUTK_EXPECT(autk_instance_create(&instance_params, &instance));

    // Create the client. This is our connection to the display server.
    // The `create_params` struct can be omitted to use defaults.
    AUTK_EXPECT(autk_client_create(instance, NULL, &client));

    // Create the main window.
    AUTK_EXPECT(autk_window_create(client, &window_params, &window));
    AUTK_EXPECT(autk_window_set_visible(window, true));

    // WIP: Add widgets.

    // Main loop.
    AUTK_EXPECT(autk_client_run(client));

    // Clean up.
    autk_window_destroy(window);
    autk_client_destroy(client);
    autk_instance_destroy(instance);

    return 0;
}

// Provide a handler for when `AUTK_EXPECT` fails.
void autk_expect_failed(const char *expr, autk_status_t status, const char *module_name,
                        const autk_source_location_t *location)
{
    // Use the default behavior. Writes to stderr. Shows a dialog box on some platforms.
    autk_default_expect_failed(expr, status, module_name, location);
}
```
