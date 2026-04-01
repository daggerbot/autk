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

#ifndef AUTK_UTILITY_JOB_QUEUE_H_
#define AUTK_UTILITY_JOB_QUEUE_H_

#include <stdatomic.h>

#include <os/types.h>

#define AUTK_JOB_QUEUE_MAX 32

typedef struct autk_job_queue autk_job_queue_t;
typedef struct autk_job_queue_node autk_job_queue_node_t;

struct autk_job_queue_node {
    autk_job_t job;
    _Atomic bool ready;
};

struct autk_job_queue {
    autk_job_queue_node_t node_pool[AUTK_JOB_QUEUE_MAX];
    size_t nodes_front;
    _Atomic size_t nodes_back;
    autk_semaphore_t n_free_nodes_sem;
    _Atomic size_t n_queued_jobs;
};

AUTK_HIDDEN autk_status_t
autk_job_queue_init(autk_job_queue_t *queue);

AUTK_HIDDEN void
autk_job_queue_fini(autk_job_queue_t *queue);

AUTK_HIDDEN autk_status_t
autk_job_queue_try_push(autk_job_queue_t *queue, autk_job_t job);

AUTK_HIDDEN autk_status_t
autk_job_queue_push(autk_job_queue_t *queue, autk_job_t job);

AUTK_HIDDEN autk_status_t
autk_job_queue_try_pop(autk_job_queue_t *queue, autk_job_t *out_job,
                       autk_instance_t *message_instance);

#endif // AUTK_UTILITY_JOB_QUEUE_H_
