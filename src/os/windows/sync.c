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

#include <windows.h>

#include <os/sync.h>

AUTK_HIDDEN autk_status_t
autk_semaphore_init(autk_semaphore_t *sem, uint32_t value)
{
    if (sizeof(uint32_t) >= sizeof(LONG) && value > (uint32_t)MAXLONG) {
        return AUTK_ERR_UNSUPPORTED_FEATURE;
    }

    sem->handle = CreateSemaphoreW(NULL, (LONG)value, MAXLONG, NULL);
    if (sem->handle == NULL) {
        return AUTK_ERR_RUNTIME_FAILURE;
    }

    return AUTK_OK;
}

AUTK_HIDDEN void
autk_semaphore_fini(autk_semaphore_t *sem)
{
    if (sem->handle != NULL) {
        CloseHandle(sem->handle);
        sem->handle = NULL;
    }
}

AUTK_HIDDEN autk_status_t
autk_semaphore_try_acquire(autk_semaphore_t *sem)
{
    switch (WaitForSingleObject(sem->handle, 0)) {
        case WAIT_OBJECT_0:
            return AUTK_OK;
        case WAIT_TIMEOUT:
            return AUTK_ERR_WOULD_BLOCK;
        default:
            return AUTK_ERR_RUNTIME_FAILURE;
    }
}

AUTK_HIDDEN autk_status_t
autk_semaphore_acquire(autk_semaphore_t *sem)
{
    switch (WaitForSingleObject(sem->handle, INFINITE)) {
        case WAIT_OBJECT_0:
            return AUTK_OK;
        default:
            return AUTK_ERR_RUNTIME_FAILURE;
    }
}

AUTK_HIDDEN autk_status_t
autk_semaphore_release(autk_semaphore_t *sem)
{
    if (!ReleaseSemaphore(sem->handle, 1, NULL)) {
        return AUTK_ERR_RUNTIME_FAILURE;
    }

    return AUTK_OK;
}
