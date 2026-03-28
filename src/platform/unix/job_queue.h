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

#ifndef AUTK_PLATFORM_UNIX_JOB_QUEUE_H_
#define AUTK_PLATFORM_UNIX_JOB_QUEUE_H_

#include <semaphore.h>
#include <stdatomic.h>

#include <autk/types.h>

#define AUTK_UNIX_JOB_QUEUE_MAX 32

typedef struct autk_unix_job_queue_node {
    autk_job_t job;
    _Atomic bool ready;
} autk_unix_job_queue_node_t;

typedef struct autk_unix_job_queue {
    autk_instance_t *instance;
    _Atomic size_t ref_count;
    _Atomic size_t n_queued_jobs;
    sem_t n_free_nodes_sem;
    bool n_free_nodes_sem_was_init;
    int wakeup_read_fd;
    int wakeup_write_fd;
    autk_unix_job_queue_node_t node_pool[AUTK_UNIX_JOB_QUEUE_MAX];
    size_t nodes_front;
    _Atomic size_t nodes_back;
} autk_unix_job_queue_t;

AUTK_HIDDEN autk_status_t
autk_unix_job_queue_create(autk_instance_t *instance, autk_unix_job_queue_t **out_queue);

AUTK_HIDDEN void
autk_unix_job_queue_retain(autk_unix_job_queue_t *queue);

AUTK_HIDDEN void
autk_unix_job_queue_release(autk_unix_job_queue_t *queue);

AUTK_HIDDEN autk_status_t
autk_unix_job_queue_push_blocking(autk_unix_job_queue_t *queue, autk_job_t job, bool *queued);

AUTK_HIDDEN autk_status_t
autk_unix_job_queue_push_nonblocking(autk_unix_job_queue_t *queue, autk_job_t job, bool *queued);

AUTK_HIDDEN autk_status_t
autk_unix_job_queue_pop_nonblocking(autk_unix_job_queue_t *queue, autk_job_t *out_job);

AUTK_HIDDEN autk_status_t
autk_unix_job_queue_poll(autk_unix_job_queue_t *queue, int display_fd, int timeout,
                         int *queue_result, int *display_result);

#endif // AUTK_PLATFORM_UNIX_JOB_QUEUE_H_
