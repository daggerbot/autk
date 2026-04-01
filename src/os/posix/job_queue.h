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

#ifndef AUTK_OS_POSIX_JOB_QUEUE_H_
#define AUTK_OS_POSIX_JOB_QUEUE_H_

#include <utility/job_queue.h>

typedef struct autk_posix_job_queue autk_posix_job_queue_t;

struct autk_posix_job_queue {
    autk_job_queue_t queue;
    int wakeup_read_fd;
    int wakeup_write_fd;
};

AUTK_HIDDEN autk_status_t
autk_posix_job_queue_init(autk_posix_job_queue_t *queue);

AUTK_HIDDEN void
autk_posix_job_queue_fini(autk_posix_job_queue_t *queue);

AUTK_HIDDEN autk_status_t
autk_posix_job_queue_try_push(autk_posix_job_queue_t *queue, autk_job_t job, bool *queued);

AUTK_HIDDEN autk_status_t
autk_posix_job_queue_push(autk_posix_job_queue_t *queue, autk_job_t job, bool *queued);

AUTK_HIDDEN autk_status_t
autk_posix_job_queue_try_pop(autk_posix_job_queue_t *queue, autk_job_t *out_job,
                             autk_instance_t *message_instance);

AUTK_HIDDEN autk_status_t
autk_posix_job_queue_poll(autk_posix_job_queue_t *queue, int display_fd, int timeout,
                          int *queue_result, int *display_result);

#endif // AUTK_OS_POSIX_JOB_QUEUE_H_
