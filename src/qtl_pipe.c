/******************************************************************************
 * Quantitative Trading Library                                               *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#include "qtl_pipe.h"
#include "spsc_queue.h"
#include "qtl_etree.h"

#include <uv.h>

struct event_pipe_s {
  spsc_queue_t pending_add;
  spsc_queue_t pending_remove;
  uv_thread_t this_thread;
  event_tree_t tree;

};

static inline void process_pending_add(event_pipe_t *p)
{
    event_queue_t *q;
    while(spsc_queue_read(&p->pending_add, &q)) {
        event_tree_add(&p->tree, q);
    }
}

void event_pipe_add(event_pipe_t *p, event_queue_t *q) {
  uv_thread_t call_thread = uv_thread_self();
  if (!uv_thread_equal(&call_thread, &p->this_thread)) {
    spsc_queue_write(&p->pending_add, q);
    return;
  }

  if(event_queue_is_synched)
      event_tree_add(&p->tree, q);
}

event_pipe_t *event_pipe_new() {
  event_pipe_t *p = malloc(sizeof(event_pipe_t));
  if (!p)
    return NULL;

  spsc_queue_init(&p->pending_add, 1024);
  p->this_thread = uv_thread_self();
}

void event_pipe_free(event_pipe_t *p) {
  if (p) {
    event_pipe_clear(p);
    free(p);
  }
}

bool event_pipe_remove(event_pipe_t *p, event_queue_t *q) {
    event_tree_remove(&p->tree, q);
}

bool event_pipe_is_empty(event_pipe_t *p) {
    process_pending_add(p);
    return event_tree_is_empty(&p->tree);
}

event_t *event_pipe_read(event_pipe_t *p) {
    process_pending_add(p);
    return event_tree_read(&p->tree);
}

void event_pipe_clear(event_pipe_t *p) {
    spsc_queue_destroy(&p->pending_add);
    event_tree_clear(&p->tree);
}
