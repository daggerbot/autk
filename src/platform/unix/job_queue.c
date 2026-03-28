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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <sys/stat.h>
#include <unistd.h>

#include <autk/diagnostics.h>
#include <autk/instance.h>

#include "../../impl_types.h"
#include "../../os_compat.h"
#include "job_queue.h"

AUTK_HIDDEN autk_status_t
autk_unix_job_queue_create(autk_instance_t *instance, autk_unix_job_queue_t **out_queue)
{
    autk_unix_job_queue_t *queue;
    int pipe_fds[2];
    int flags;

    // Allocate the queue.
    queue = autk_instance_alloc(instance, NULL, 0, sizeof(autk_unix_job_queue_t),
                                AUTK_MEMORY_TAG_QUEUE);
    if (!queue) {
        return AUTK_ERR_OUT_OF_MEMORY;
    }

    // Initialize the queue's trivial fields.
    *queue = (autk_unix_job_queue_t){
        .instance = instance,
        .ref_count = 1,
        .wakeup_read_fd = -1,
        .wakeup_write_fd = -1,
    };

    // Create the free slots semaphore.
    if (sem_init(&queue->n_free_nodes_sem, 0, AUTK_UNIX_JOB_QUEUE_MAX) != 0) {
        autk_unix_job_queue_release(queue);
        return AUTK_ERR_SYNC_FAILURE;
    }
    queue->n_free_nodes_sem_was_init = true;

    // Create the wakeup pipe.
    if (pipe(pipe_fds) != 0) {
        autk_unix_job_queue_release(queue);
        return AUTK_ERR_IO_FAILURE;
    }

    queue->wakeup_read_fd = pipe_fds[0];
    queue->wakeup_write_fd = pipe_fds[1];

    for (int i = 0; i < 2; i++) {
        // Do not share the pipe with child processes.
        if (fcntl(pipe_fds[i], F_SETFD, FD_CLOEXEC) == -1) {
            autk_unix_job_queue_release(queue);
            return AUTK_ERR_IO_FAILURE;
        }

        // Make the pipe non-blocking.
        flags = fcntl(pipe_fds[i], F_GETFL, 0);
        if (flags == -1 || fcntl(pipe_fds[i], F_SETFL, flags | O_NONBLOCK) == -1) {
            autk_unix_job_queue_release(queue);
            return AUTK_ERR_IO_FAILURE;
        }
    }

    // Success!
    *out_queue = queue;
    return AUTK_OK;
}

AUTK_HIDDEN void
autk_unix_job_queue_retain(autk_unix_job_queue_t *queue)
{
    atomic_fetch_add_explicit(&queue->ref_count, 1, memory_order_relaxed);
}

AUTK_HIDDEN void
autk_unix_job_queue_release(autk_unix_job_queue_t *queue)
{
    autk_unix_job_queue_node_t *node;

    if (atomic_fetch_sub_explicit(&queue->ref_count, 1, memory_order_release) == 1) {
        atomic_thread_fence(memory_order_acquire);

        // Drop all remaining jobs.
        while (queue->n_queued_jobs) {
            node = &queue->node_pool[queue->nodes_front];
            queue->nodes_front = (queue->nodes_front + 1) % AUTK_UNIX_JOB_QUEUE_MAX;
            if (node->job.fini) {
                node->job.fini(node->job.ctx);
            }
            queue->n_queued_jobs--;
        }

        // Close the wakeup pipe.
        if (queue->wakeup_read_fd >= 0) {
            close(queue->wakeup_read_fd);
            queue->wakeup_read_fd = -1;
        }
        if (queue->wakeup_write_fd >= 0) {
            close(queue->wakeup_write_fd);
            queue->wakeup_write_fd = -1;
        }

        // Destroy the free slots semaphore.
        if (queue->n_free_nodes_sem_was_init) {
            sem_destroy(&queue->n_free_nodes_sem);
            queue->n_free_nodes_sem_was_init = false;
        }

        // Free the job queue object.
        autk_instance_alloc(queue->instance, queue, sizeof(autk_unix_job_queue_t), 0,
                            AUTK_MEMORY_TAG_QUEUE);
    }
}

static autk_status_t
autk_unix_job_queue_push(autk_unix_job_queue_t *queue, autk_job_t job, bool block,
                         const struct timespec *timeout, bool *queued)
{
    int wait_result;
    int errnum;
    char errbuf[256];
    size_t node_index;
    autk_unix_job_queue_node_t *node;

    *queued = false;

    // Wait for a free node.
    while (1) {
        if (block) {
            if (timeout) {
                wait_result = sem_timedwait(&queue->n_free_nodes_sem, timeout);
            } else {
                wait_result = sem_wait(&queue->n_free_nodes_sem);
            }
        } else {
            wait_result = sem_trywait(&queue->n_free_nodes_sem);
        }

        if (wait_result == 0) {
            break;
        }

        errnum = errno;
        switch (errnum) {
            case EAGAIN:
                return AUTK_ERR_TRY_AGAIN;
            case ETIMEDOUT:
                return AUTK_ERR_TIMEOUT;
            case EINTR:
                break;
            default:
                if (queue->instance->flags & AUTK_INSTANCE_FLAGS_THREAD_SAFE_MESSAGE) {
                    AUTK_ERROR(queue->instance, "sem_*wait() failed: %s",
                               autk_strerror_r(errnum, errbuf, sizeof(errbuf)));
                }
                return AUTK_ERR_SYNC_FAILURE;
        }
    }

    // Acquire and fill the node. The pool size must be a power of 2 to ensure that `nodes_back`
    // always wraps correctly.
    static_assert((AUTK_UNIX_JOB_QUEUE_MAX & (AUTK_UNIX_JOB_QUEUE_MAX - 1)) == 0,
                  "Job queue pool size must be a power of 2");
    node_index = atomic_fetch_add_explicit(&queue->nodes_back, 1, memory_order_relaxed)
                 % AUTK_UNIX_JOB_QUEUE_MAX;
    node = &queue->node_pool[node_index];
    node->job = job;
    atomic_store_explicit(&node->ready, true, memory_order_release);
    atomic_fetch_add_explicit(&queue->n_queued_jobs, 1, memory_order_release);
    *queued = true;

    // Write to the wakeup pipe.
    while (1) {
        if (write(queue->wakeup_write_fd, "", 1) >= 0) {
            return AUTK_OK;
        } else {
            errnum = errno;
            switch (errnum) {
                case EAGAIN:
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
                case EWOULDBLOCK:
#endif
                    return AUTK_OK;
                case EINTR:
                    break;
                default:
                    if (queue->instance->flags & AUTK_INSTANCE_FLAGS_THREAD_SAFE_MESSAGE) {
                        AUTK_ERROR(queue->instance, "write() failed: %s",
                                   autk_strerror_r(errnum, errbuf, sizeof(errbuf)));
                    }
                    return AUTK_ERR_IO_FAILURE;
            }
        }
    }
}

AUTK_HIDDEN autk_status_t
autk_unix_job_queue_push_blocking(autk_unix_job_queue_t *queue, autk_job_t job, bool *queued)
{
    return autk_unix_job_queue_push(queue, job, true, NULL, queued);
}

AUTK_HIDDEN autk_status_t
autk_unix_job_queue_push_nonblocking(autk_unix_job_queue_t *queue, autk_job_t job, bool *queued)
{
    return autk_unix_job_queue_push(queue, job, false, NULL, queued);
}

AUTK_HIDDEN autk_status_t
autk_unix_job_queue_pop_nonblocking(autk_unix_job_queue_t *queue, autk_job_t *out_job)
{
    autk_unix_job_queue_node_t *node;
    char errbuf[256];

    // Check if any jobs are queued.
    if (atomic_load_explicit(&queue->n_queued_jobs, memory_order_acquire) == 0) {
        return AUTK_ERR_QUEUE_EMPTY;
    }

    // Check if the front job is ready.
    node = &queue->node_pool[queue->nodes_front];
    if (!atomic_load_explicit(&node->ready, memory_order_acquire)) {
        return AUTK_ERR_TRY_AGAIN;
    }

    // Acquire the job.
    *out_job = node->job;
    atomic_store_explicit(&node->ready, false, memory_order_relaxed);
    queue->nodes_front = (queue->nodes_front + 1) % AUTK_UNIX_JOB_QUEUE_MAX;
    atomic_fetch_sub_explicit(&queue->n_queued_jobs, 1, memory_order_relaxed);

    // Notify producers that a free node is available.
    if (sem_post(&queue->n_free_nodes_sem) != 0) {
        if (queue->instance->flags & AUTK_INSTANCE_FLAGS_THREAD_SAFE_MESSAGE) {
            autk_strerror_r(errno, errbuf, sizeof(errbuf));
            AUTK_ERROR(queue->instance, "sem_post() failed: %s -- Message producers may freeze",
                       errbuf);
        }
    }

    return AUTK_OK;
}

AUTK_HIDDEN autk_status_t
autk_unix_job_queue_poll(autk_unix_job_queue_t *queue, int display_fd, int timeout,
                         int *queue_result, int *display_result)
{
    struct pollfd poll_fds[2] = {
        {.fd = queue->wakeup_read_fd, .events = POLLIN},
        {.fd = display_fd, .events = POLLIN},
    };
    nfds_t nfds = 1;
    int poll_result;
    int errnum;
    char errbuf[256];
    char wakeup_buf[AUTK_UNIX_JOB_QUEUE_MAX]; // size is arbitrary

    *queue_result = 0;
    *display_result = 0;

    if (display_fd >= 0) {
        nfds++;
    }

    // Wait for input.
    poll_result = poll(poll_fds, nfds, timeout);

    if (poll_result < 0) {
        errnum = errno;
        switch (errnum) {
            case EINTR:
                return AUTK_ERR_INTERRUPTED;
            default:
                if (queue->instance->flags & AUTK_INSTANCE_FLAGS_THREAD_SAFE_MESSAGE) {
                    autk_strerror_r(errnum, errbuf, sizeof(errbuf));
                    AUTK_ERROR(queue->instance, "poll() failed: %s", errbuf);
                }
                return AUTK_ERR_IO_FAILURE;
        }
    } else if (poll_result == 0) {
        return AUTK_ERR_TIMEOUT;
    }

    // Check for per-fd errors.
    if (poll_fds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
        if (queue->instance->flags & AUTK_INSTANCE_FLAGS_THREAD_SAFE_MESSAGE) {
            AUTK_ERROR(queue->instance, "poll() returned error for queue wakeup fd: %d",
                       poll_fds[0].revents);
        }
        *queue_result = -1;
    }

    if (display_fd >= 0 && (poll_fds[1].revents & (POLLERR | POLLHUP | POLLNVAL))) {
        if (queue->instance->flags & AUTK_INSTANCE_FLAGS_THREAD_SAFE_MESSAGE) {
            AUTK_ERROR(queue->instance, "poll() returned error for display fd: %d",
                       poll_fds[1].revents);
        }
        *display_result = -1;
    }

    // Check for available input. Overwrite error results.
    if (poll_fds[0].revents & POLLIN) {
        *queue_result = 1;

        // Drain the wakeup pipe.
        if (read(queue->wakeup_read_fd, wakeup_buf, sizeof(wakeup_buf)) < 0) {
            errnum = errno;
            switch (errnum) {
                case EAGAIN:
                case EINTR:
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
                case EWOULDBLOCK:
#endif
                    break;
                default:
                    if (queue->instance->flags & AUTK_INSTANCE_FLAGS_THREAD_SAFE_MESSAGE) {
                        autk_strerror_r(errnum, errbuf, sizeof(errbuf));
                        AUTK_ERROR(queue->instance, "read() failed: %s", errbuf);
                    }
                    return AUTK_ERR_IO_FAILURE;
            }
        }
    }

    if (display_fd >= 0 && poll_fds[1].revents & POLLIN) {
        *display_result = 1;
    }

    return AUTK_OK;
}
