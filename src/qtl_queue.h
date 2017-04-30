/******************************************************************************
 * Quantitative Trading Library                                               *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#ifndef CTF_EVENT_QUEUE_H
#define CTF_EVENT_QUEUE_H

#include <stdbool.h>
#include "qtl_event.h"

/*
 * lock-free spsc event queue
 */
typedef enum { kAll, kData, kExecution, kReminder, kService } EventQueueId;
typedef enum { kMaster, kSlave } EventQueueType;
typedef enum { kHighest, kHigh, kNormal, kLow, kLowest } EventQueuePriority;

typedef struct event_queue_s event_queue_t;
typedef struct event_bus_s event_bus_t;

event_queue_t *event_queue_new(EventQueueId id, EventQueueType type, EventQueuePriority priority, unsigned int size, event_bus_t *bus);
void event_queue_free(event_queue_t *q);
void event_queue_clear(event_queue_t *q);

EventQueueId event_queue_get_id(event_queue_t *q);
EventQueueType event_queue_get_type(event_queue_t *q);
EventQueuePriority event_queue_get_priority(event_queue_t *q);
char *event_queue_get_name(event_queue_t *q);
bool event_queue_is_synched(event_queue_t *q);
void event_queue_set_synched(event_queue_t *q, bool is_synched);

/* if queue is full, push will return false */
bool event_queue_push(event_queue_t *q, event_t *);

/* if queue is empty, pop will return false */
bool event_queue_pop(event_queue_t *q, event_t **);

/* if queue is empty, peek will return null pointer */
event_t *event_queue_peek(event_queue_t *q);
datetime_t event_queue_peek_time(event_queue_t *q);

bool event_queue_is_empty(event_queue_t *q);
bool event_queue_is_full(event_queue_t *q);

/* If called by consumer, then true size may be more (because producer may
   be adding items concurrently).
   If called by producer, then true size may be less (because consumer may
   be removing items concurrently).
   It is undefined to call this from any other thread.
*/
unsigned int event_queue_size(event_queue_t *q);
unsigned int event_queue_capacity(event_queue_t *q);

#endif // CTF_EVENT_QUEUE_H
