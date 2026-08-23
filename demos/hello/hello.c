// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#include <autk/autk.h>

AUTK_MAIN(argc, argv)
{
    autk_console_init(AUTK_CONSOLE_MODE_ATTACH);
    autk_set_min_message_severity(AUTK_MESSAGE_SEVERITY_DEBUG);
    AUTK_EXPECT(autk_init());

    autk_shutdown();
    return 0;
}
