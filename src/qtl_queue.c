/******************************************************************************
 * Quantitative Trading Library                                               *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#include "qtl_queue.h"
#include "spsc_queue.h"
#include "qtl_event.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

struct event_queue_s {
  EventQueueId id;
  EventQueueType type;
  EventQueuePriority priority;
  char *name;
  event_bus_t *bus;
  bool is_synced;
  spsc_queue_t spsc;
};

event_queue_t *event_queue_new(EventQueueId id, EventQueueType type,
                               EventQueuePriority priority, unsigned int size,
                               event_bus_t *bus) {
  assert(size >= 2);
  event_queue_t *q = malloc(sizeof(event_queue_t));
  if (!q)
    return NULL;
  if (spsc_queue_init(&q->spsc, size) < 0) {
    free(q);
    return NULL;
  }

  q->id = id;
  q->type = type;
  q->priority = priority;
  q->bus = bus;
  return q;
}

void event_queue_free(event_queue_t *q) {
  assert(q);
  event_queue_clear(q);
  spsc_queue_destroy(&q->spsc);
  free(q);
}

bool event_queue_push(event_queue_t *q, event_t *e) {
  assert(q);
  assert(e);
  bool is_empty = spsc_queue_is_empty(&q->spsc);
  bool ret = spsc_queue_write(&q->spsc, e);
  if (is_empty && ret && q->bus)
    event_bus_wakeup(q->bus);
  return ret;
}

bool event_queue_pop(event_queue_t *q, event_t **item) {
  assert(q);
  assert(item);
  return spsc_queue_read(&q->spsc, item);
}

event_t *event_queue_peek(event_queue_t *q) {
  assert(q);
  return spsc_queue_peek(&q->spsc);
}

bool event_queue_is_empty(event_queue_t *q) {
  assert(q);
  return spsc_queue_is_empty(&q->spsc);
}

bool event_queue_is_full(event_queue_t *q) {
  assert(q);
  return spsc_queue_is_full(&q->spsc);
}

unsigned int event_queue_size(event_queue_t *q) {
  assert(q);
  return spsc_queue_size(&q->spsc);
}

unsigned int event_queue_capacity(event_queue_t *q) {
  assert(q);
  return spsc_queue_capacity(&q->spsc);
}

EventQueueId event_queue_get_id(event_queue_t *q) {
  assert(q);
  return q->id;
}

EventQueueType event_queue_get_type(event_queue_t *q) {
  assert(q);
  return q->type;
}

EventQueuePriority event_queue_get_priority(event_queue_t *q) {
  assert(q);
  return q->priority;
}

char *event_queue_get_name(event_queue_t *q) {
  assert(q);
  return q->name;
}

bool event_queue_is_synched(event_queue_t *q) {
  assert(q);
  return q->is_synced;
}

void event_queue_set_synched(event_queue_t *q, bool is_synched) {
  assert(q);
  q->is_synced = is_synched;
}

datetime_t event_queue_peek_time(event_queue_t *q) {
  assert(q);
  event_t *e = event_queue_peek(q);
  assert(e);
  return event_get_time(e);
}

void event_queue_clear(event_queue_t *q)
{
    event_t *e;
    while(spsc_queue_read(&q->spsc, &e)) {
        assert(e);
        event_unref((event_t *)e);
    }
}
