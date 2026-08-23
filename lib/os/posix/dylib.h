// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#ifndef AUTK_OS_POSIX_DYLIB_H_
#define AUTK_OS_POSIX_DYLIB_H_

#include <autk/types.h>

typedef struct autk_dylib autk_dylib_t;
typedef struct autk_dylib_symbol autk_dylib_symbol_t;

struct autk_dylib {
    const char *lib_name;
    const autk_dylib_symbol_t *symbols;
    uint32_t symbol_count;
    void *dlhandle;
    bool loaded;
};

struct autk_dylib_symbol {
    const char *name;
    void **ptr;
};

AUTK_HIDDEN autk_status_t
autk_dylib_load(autk_dylib_t *dylib);

#endif // AUTK_OS_POSIX_DYLIB_H_
