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

#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <unistd.h>

#include <autk/diagnostics.h>
#include <autk/instance.h>
#include <core/types.h>
#include <os/compat.h>

#include "job_queue.h"

AUTK_HIDDEN autk_status_t
autk_posix_job_queue_init(autk_posix_job_queue_t *queue)
{
    int pipe_fds[2];
    int flags;

    *queue = (autk_posix_job_queue_t){
        .wakeup_read_fd = -1,
        .wakeup_write_fd = -1,
    };

    // Initialize the underlying queue.
    AUTK_TRY(autk_job_queue_init(&queue->queue));

    // Initialize the wakeup pipe.
    if (pipe(pipe_fds) != 0) {
        autk_posix_job_queue_fini(queue);
        return AUTK_ERR_IO_FAILURE;
    }

    queue->wakeup_read_fd = pipe_fds[0];
    queue->wakeup_write_fd = pipe_fds[1];

    for (int i = 0; i < 2; i++) {
        // Do not share the pipe with child processes.
        if (fcntl(pipe_fds[i], F_SETFD, FD_CLOEXEC) == -1) {
            autk_posix_job_queue_fini(queue);
            return AUTK_ERR_IO_FAILURE;
        }

        // Make the pipe non-blocking.
        flags = fcntl(pipe_fds[i], F_GETFL, 0);
        if (flags == -1 || fcntl(pipe_fds[i], F_SETFL, flags | O_NONBLOCK) == -1) {
            autk_posix_job_queue_fini(queue);
            return AUTK_ERR_IO_FAILURE;
        }
    }

    return AUTK_OK;
}

AUTK_HIDDEN void
autk_posix_job_queue_fini(autk_posix_job_queue_t *queue)
{
    autk_job_queue_fini(&queue->queue);

    if (queue->wakeup_read_fd >= 0) {
        close(queue->wakeup_read_fd);
        queue->wakeup_read_fd = -1;
    }
    if (queue->wakeup_write_fd >= 0) {
        close(queue->wakeup_write_fd);
        queue->wakeup_write_fd = -1;
    }
}

static autk_status_t
notify_wakeup_pipe(autk_posix_job_queue_t *queue)
{
    while (1) {
        if (write(queue->wakeup_write_fd, "", 1) >= 0) {
            return AUTK_OK;
        } else {
            switch (errno) {
                case EAGAIN:
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
                case EWOULDBLOCK:
#endif
                    return AUTK_OK;
                case EINTR:
                    break;
                default:
                    return AUTK_ERR_IO_FAILURE;
            }
        }
    }
}

AUTK_HIDDEN autk_status_t
autk_posix_job_queue_try_push(autk_posix_job_queue_t *queue, autk_job_t job, bool *queued)
{
    *queued = false;
    AUTK_TRY(autk_job_queue_try_push(&queue->queue, job));
    *queued = true;
    return notify_wakeup_pipe(queue);
}

AUTK_HIDDEN autk_status_t
autk_posix_job_queue_push(autk_posix_job_queue_t *queue, autk_job_t job, bool *queued)
{
    *queued = false;
    AUTK_TRY(autk_job_queue_push(&queue->queue, job));
    *queued = true;
    return notify_wakeup_pipe(queue);
}

AUTK_HIDDEN autk_status_t
autk_posix_job_queue_try_pop(autk_posix_job_queue_t *queue, autk_job_t *out_job,
                             autk_instance_t *message_instance)
{
    return autk_job_queue_try_pop(&queue->queue, out_job, message_instance);
}

static autk_status_t
drain_wakeup_pipe(autk_posix_job_queue_t *queue)
{
    char buf[64]; // size is arbitrary

    while (1) {
        ssize_t read_result = read(queue->wakeup_read_fd, buf, sizeof(buf));
        if (read_result == 0) {
            if (read_result == sizeof(buf)) {
                continue;
            }
            return AUTK_OK;
        } else if (read_result < 0) {
            switch (errno) {
                case EAGAIN:
                case EINTR:
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
                case EWOULDBLOCK:
#endif
                    return AUTK_OK;
                default:
                    return AUTK_ERR_IO_FAILURE;
            }
        }
    }
}

AUTK_HIDDEN autk_status_t
autk_posix_job_queue_poll(autk_posix_job_queue_t *queue, int display_fd, int timeout,
                          int *queue_result, int *display_result)
{
    struct pollfd poll_fds[2] = {
        {.fd = queue->wakeup_read_fd, .events = POLLIN},
        {.fd = display_fd, .events = POLLIN},
    };
    nfds_t nfds = 1;
    int poll_result;

    *queue_result = 0;
    *display_result = 0;

    if (display_fd >= 0) {
        nfds++;
    }

    // Wait for input.
    poll_result = poll(poll_fds, nfds, timeout);
    if (poll_result < 0) {
        switch (errno) {
            case EINTR:
                return AUTK_ERR_INTERRUPTED;
            default:
                return AUTK_ERR_IO_FAILURE;
        }
    } else if (poll_result == 0) {
        return AUTK_ERR_TIMEOUT;
    }

    // Check each fd for errors.
    if (poll_fds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
        *queue_result = -1;
    }
    if (display_fd >= 0 && (poll_fds[1].revents & (POLLERR | POLLHUP | POLLNVAL))) {
        *display_result = -1;
    }

    // Check each fd for input. Overwrite error results.
    if (poll_fds[0].revents & POLLIN) {
        *queue_result = 1;
        AUTK_TRY(drain_wakeup_pipe(queue));
    }
    if (display_fd >= 0 && poll_fds[1].revents & POLLIN) {
        *display_result = 1;
    }

    return AUTK_OK;
}
