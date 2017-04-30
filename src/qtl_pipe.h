/******************************************************************************
 * Quantitative Trading Library                                               *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#ifndef EVENT_PIPE_H
#define EVENT_PIPE_H

#include <stdint.h>
#include <stdbool.h>

#include "qtl_queue.h"

typedef struct event_pipe_s event_pipe_t;

event_pipe_t *event_pipe_new();
void event_pipe_free(event_pipe_t *p);

void event_pipe_add(event_pipe_t *p, event_queue_t *q);
bool event_pipe_remove(event_pipe_t *p, event_queue_t *q);

bool event_pipe_is_empty(event_pipe_t *p);

event_t *event_pipe_read(event_pipe_t *p);
void event_pipe_clear(event_pipe_t *p);

#endif // EVENT_PIPE_H
