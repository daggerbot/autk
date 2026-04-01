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

#include <assert.h>
#include <errno.h>
#include <semaphore.h>

#include <os/sync.h>

AUTK_HIDDEN autk_status_t
autk_semaphore_init(autk_semaphore_t *sem, uint32_t value)
{
    assert(!sem->was_init);

    if (sem_init(&sem->handle, 0, value) != 0) {
        switch (errno) {
            case EINVAL:
                return AUTK_ERR_UNSUPPORTED_FEATURE;
            default:
                return AUTK_ERR_RUNTIME_FAILURE;
        }
    }

    sem->was_init = true;
    return AUTK_OK;
}

AUTK_HIDDEN void
autk_semaphore_fini(autk_semaphore_t *sem)
{
    if (!sem->was_init) {
        return;
    }

    sem_destroy(&sem->handle);
    sem->was_init = false;
}

AUTK_HIDDEN autk_status_t
autk_semaphore_try_acquire(autk_semaphore_t *sem)
{
    assert(sem->was_init);

    if (sem_trywait(&sem->handle) != 0) {
        switch (errno) {
            case EAGAIN:
                return AUTK_ERR_WOULD_BLOCK;
            default:
                return AUTK_ERR_RUNTIME_FAILURE;
        }
    }

    return AUTK_OK;
}

AUTK_HIDDEN autk_status_t
autk_semaphore_acquire(autk_semaphore_t *sem)
{
    assert(sem->was_init);

    if (sem_wait(&sem->handle) != 0) {
        switch (errno) {
            case EINTR:
                return AUTK_ERR_INTERRUPTED;
            case ETIMEDOUT:
                return AUTK_ERR_TIMEOUT;
            default:
                return AUTK_ERR_RUNTIME_FAILURE;
        }
    }

    return AUTK_OK;
}

AUTK_HIDDEN autk_status_t
autk_semaphore_release(autk_semaphore_t *sem)
{
    assert(sem->was_init);

    if (sem_post(&sem->handle) != 0) {
        return AUTK_ERR_RUNTIME_FAILURE;
    }

    return AUTK_OK;
}
