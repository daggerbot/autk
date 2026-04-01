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
#include <stdatomic.h>

#include <autk/diagnostics.h>
#include <os/sync.h>

#include "job_queue.h"

static_assert((AUTK_JOB_QUEUE_MAX & (AUTK_JOB_QUEUE_MAX - 1)) == 0,
              "AUTK_JOB_QUEUE_MAX must be a power of two");

AUTK_HIDDEN autk_status_t
autk_job_queue_init(autk_job_queue_t *queue)
{
    *queue = (autk_job_queue_t){0};
    AUTK_TRY(autk_semaphore_init(&queue->n_free_nodes_sem, AUTK_JOB_QUEUE_MAX));
    return AUTK_OK;
}

AUTK_HIDDEN void
autk_job_queue_fini(autk_job_queue_t *queue)
{
    autk_semaphore_fini(&queue->n_free_nodes_sem);
}

static void
do_push(autk_job_queue_t *queue, autk_job_t job)
{
    size_t node_index;
    autk_job_queue_node_t *node;

    node_index =
        atomic_fetch_add_explicit(&queue->nodes_back, 1, memory_order_relaxed) % AUTK_JOB_QUEUE_MAX;
    node = &queue->node_pool[node_index];
    node->job = job;
    atomic_store_explicit(&node->ready, true, memory_order_release);
    atomic_fetch_add_explicit(&queue->n_queued_jobs, 1, memory_order_release);
}

AUTK_HIDDEN autk_status_t
autk_job_queue_try_push(autk_job_queue_t *queue, autk_job_t job)
{
    AUTK_TRY(autk_semaphore_try_acquire(&queue->n_free_nodes_sem));
    do_push(queue, job);
    return AUTK_OK;
}

AUTK_HIDDEN autk_status_t
autk_job_queue_push(autk_job_queue_t *queue, autk_job_t job)
{
    AUTK_TRY(autk_semaphore_acquire(&queue->n_free_nodes_sem));
    do_push(queue, job);
    return AUTK_OK;
}

AUTK_HIDDEN autk_status_t
autk_job_queue_try_pop(autk_job_queue_t *queue, autk_job_t *out_job,
                       autk_instance_t *message_instance)
{
    autk_job_queue_node_t *node;
    autk_status_t status;

    // Check if any jobs are queued.
    if (atomic_load_explicit(&queue->n_queued_jobs, memory_order_acquire) == 0) {
        return AUTK_ERR_QUEUE_EMPTY;
    }

    // Check if the job is ready.
    node = &queue->node_pool[queue->nodes_front];
    if (!atomic_load_explicit(&node->ready, memory_order_acquire)) {
        return AUTK_ERR_TRY_AGAIN;
    }

    // Consume the job.
    *out_job = node->job;
    atomic_store_explicit(&node->ready, false, memory_order_relaxed);
    queue->nodes_front = (queue->nodes_front + 1) % AUTK_JOB_QUEUE_MAX;
    atomic_fetch_sub_explicit(&queue->n_queued_jobs, 1, memory_order_relaxed);

    // Notify producers that a free node is available.
    status = autk_semaphore_release(&queue->n_free_nodes_sem);
    if (status != AUTK_OK && message_instance) {
        AUTK_ERROR(message_instance,
                   "autk_semaphore_release() failed: %s -- Message producers may freeze",
                   autk_status_to_string(status));
    }

    return AUTK_OK;
}
