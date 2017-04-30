/******************************************************************************
 * Quantitative Trading Library                                               *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#include "qtl_bus.h"
#include <uv.h>
#include "spsc_queue.h"
#include "qtl_event.h"
#include "qtl_rqueue.h"
#include <stdlib.h>
#include "util.h"

typedef enum { kBefore, kAfter } ReminderOrder;

struct event_bus_s {
  event_pipe_t *command_pipe;
  event_pipe_t *data_pipe;
  event_pipe_t *execution_pipe;
  event_pipe_t *service_pipe;
  event_pipe_t *historical_pipe;
  reminder_queue_t *local_clock_queue;
  reminder_queue_t *exchange_clock_queue;
  datetime_t local_clock_datetime;
  datetime_t exchange_clock_datetime;
  ReminderOrder reminder_order;
  EventBusMode mode;
  EventBusIdleMode idle_mode;
  int queue_count;
  event_queue_t *queues[1024];
  uv_mutex_t mutex;
  uv_cond_t manual_reset_event_slim;
  bool is_simulation_stop;
  bool show_warnings;
  bool clear_reminders;
  bool read_reminders;
  event_t *saved_event;
  uv_thread_t this_thread;
  spsc_queue_t pending;
};

event_bus_t *event_bus_new() {
  event_bus_t *bus = malloc(sizeof(event_bus_t));
  if (!bus)
    return NULL;
  bus->show_warnings = true;
  bus->read_reminders = true;
  bus->idle_mode = kIdleWait;
  bus->mode = kSimulation;
  bus->command_pipe = event_pipe_new();
  bus->data_pipe = event_pipe_new();
  bus->execution_pipe = event_pipe_new();
  bus->service_pipe = event_pipe_new();
  bus->historical_pipe = event_pipe_new();

  uv_mutex_init(&bus->mutex);
  uv_cond_init(&bus->manual_reset_event_slim);
  return bus;
}

void event_bus_free(event_bus_t *bus) {
  if (bus->command_pipe)
    event_pipe_free(bus->command_pipe);
  if (bus->data_pipe)
    event_pipe_free(bus->data_pipe);
  if (bus->execution_pipe)
    event_pipe_free(bus->execution_pipe);
  if (bus->service_pipe)
    event_pipe_free(bus->service_pipe);
  if (bus->historical_pipe)
    event_pipe_free(bus->historical_pipe);

  uv_mutex_destroy(&bus->mutex);
  uv_cond_destroy(&bus->manual_reset_event_slim);
  free(bus);
}

void event_bus_attach(event_bus_t *bus) {}

void event_bus_deattach(event_bus_t *bus) {}

static event_t *enqueue_attach(event_bus_t *bus) {
  event_t *e = bus->saved_event;
  bus->saved_event = NULL;
  /*for (int i = 0; i < bus->queue_count; i++) {
    EventType etype = event_get_type(e);
    if (etype != kOnQueueClosed && etype != kOnQueueOpen) {
      bus->queues[i]->enqueue(e);
    }
  }*/
  return e;
}

static event_t *simulation_dequeue(event_bus_t *bus) {
  while (true) {
    bool data_is_empty = event_pipe_is_empty(bus->data_pipe);
    if (!data_is_empty && !bus->saved_event) {
      event_t *e = event_pipe_read(bus->data_pipe);
      EventType etype = event_get_type(e);
      datetime_t etime = event_get_time(e);
      if (etype == kOnSimulatorStop) {
        bus->is_simulation_stop = true;
        continue;
      }
      if (etime < event_bus_get_datetime(bus)) {
      }
      bus->saved_event = e;
    }

    if (!event_pipe_is_empty(bus->execution_pipe)) {
      return event_pipe_read(bus->execution_pipe);
    }

    if (!event_queue_is_empty(bus->local_clock_queue)) {
    }

    if (!event_queue_is_empty(bus->exchange_clock_queue)) {
    }

    if (!event_pipe_is_empty(bus->command_pipe)) {
      return event_pipe_read(bus->command_pipe);
    }

    if (!event_pipe_is_empty(bus->service_pipe)) {
      return event_pipe_read(bus->service_pipe);
    }

    if (bus->saved_event) {
      return enqueue_attach(bus);
    }

    if (bus->is_simulation_stop) {
      bus->saved_event = (event_t*)new_on_simulation_stop_event();
      bus->saved_event->timestamp = event_bus_get_datetime(bus);
      bus->is_simulation_stop = false;
      return enqueue_attach(bus);
    }

    uv_sleep(1000);
  }
}

static inline void timewait(event_bus_t *bus, uint64_t timeout)
{
    uv_mutex_lock(&bus->mutex);
    uv_cond_timedwait(&bus->manual_reset_event_slim, &bus->mutex, timeout);
    uv_mutex_unlock(&bus->mutex);
}

void event_bus_wakeup(event_bus_t *bus)
{
    uv_mutex_lock(&bus->mutex);
    uv_cond_signal(&bus->manual_reset_event_slim);
    uv_mutex_unlock(&bus->mutex);
}

static event_t *realtime_dequeue(event_bus_t *bus) {
  while (true) {
    bool data_is_empty = event_pipe_is_empty(bus->data_pipe);

    if (!data_is_empty && !bus->saved_event) {
      bus->saved_event = event_pipe_read(bus->data_pipe);
    }

    if (!event_queue_is_empty(bus->local_clock_queue)) {
    }

    if (!event_queue_is_empty(bus->exchange_clock_queue)) {
    }

    if (!event_pipe_is_empty(bus->execution_pipe)) {
      return event_pipe_read(bus->execution_pipe);
    }

    if (!event_pipe_is_empty(bus->command_pipe)) {
      return event_pipe_read(bus->command_pipe);
    }

    if (!event_pipe_is_empty(bus->service_pipe)) {
      return event_pipe_read(bus->service_pipe);
    }

    if (bus->saved_event) {
      event_t *e = bus->saved_event;
      bus->saved_event = NULL;
      return e;
    }

    if (bus->idle_mode != kIdleSleep) {
      if (bus->idle_mode == kIdleWait) {
            timewait(bus, 1);
      }
    } else {
      sleep(1);
    }
  }
}

event_t *event_bus_dequeue(event_bus_t *bus) {
  if (bus->mode == kSimulation)
    return simulation_dequeue(bus);
  else
    return realtime_dequeue(bus);
}

void event_bus_clear(event_bus_t *bus) {
  free(bus->saved_event);
  bus->saved_event = NULL;
  reminder_queue_clear(bus->local_clock_queue);
  reminder_queue_clear(bus->exchange_clock_queue);
  event_pipe_clear(bus->service_pipe);
  event_pipe_clear(bus->data_pipe);
  event_pipe_clear(bus->execution_pipe);
  event_pipe_clear(bus->historical_pipe);
  //
  bus->queue_count = 0;
  bus->is_simulation_stop = false;
}

bool event_bus_get_clear_reminders(event_bus_t *bus) {
  return bus->clear_reminders;
}

void event_bus_set_clear_reminders(event_bus_t *bus, bool clearReminders) {
  bus->clear_reminders = clearReminders;
}

bool event_bus_get_show_warnings(event_bus_t *bus) {
  return bus->show_warnings;
}

void event_bus_set_show_warnings(event_bus_t *bus, bool showWarings) {
  bus->show_warnings = showWarings;
}

EventBusIdleMode event_bus_get_idle_mode(event_bus_t *bus) {
  return bus->idle_mode;
}

void event_bus_set_idle_mode(event_bus_t *bus, EventBusIdleMode mode) {
  bus->idle_mode = mode;
}

EventBusMode event_bus_get_mode(event_bus_t *bus) { return bus->mode; }

void event_bus_set_mode(event_bus_t *bus, EventBusMode mode) {
  bus->mode = mode;
}

event_pipe_t *event_bus_get_data_pipe(event_bus_t *bus) {
  return bus->data_pipe;
}

event_pipe_t *event_bus_get_execution_pipe(event_bus_t *bus) {
  return bus->execution_pipe;
}

event_pipe_t *event_bus_get_service_pipe(event_bus_t *bus) {
  return bus->service_pipe;
}

event_pipe_t *event_bus_get_historical_pipe(event_bus_t *bus) {
  return bus->historical_pipe;
}


bool event_bus_add_queue(event_bus_t *bus, event_queue_t *q, EventBusPipe pipe)
{
    switch (pipe) {
    case kDataPipe:
        event_pipe_add(bus->data_pipe, q);
        break;
    case kExecutionPipe:
        event_pipe_add(bus->execution_pipe, q);
        break;
    case kServicePipe:
        event_pipe_add(bus->service_pipe, q);
        break;
    case kHistoricalPipe:
        event_pipe_add(bus->historical_pipe, q);
        break;
    }
    return true;
}

datetime_t event_bus_get_datetime(event_bus_t *bus)
{
    if(bus->mode == kRealtime)
        return bus->local_clock_datetime;
    else
        return utc_now();
}

void event_bus_set_datetime(event_bus_t *bus, datetime_t time)
{
    if (bus->mode != kSimulation) {
        printf("Can not set dateTime because Clock is not in the Simulation mode\n");
        return;
    }

    if(time < bus->local_clock_datetime) {
        printf("%s\n", "local clock datetime out of order");
        return;
    }

    bus->local_clock_datetime = time;
}

datetime_t event_bus_get_exchange_datetime(event_bus_t *bus)
{
    return bus->exchange_clock_datetime;
}

void event_bus_set_exchange_datetime(event_bus_t *bus, datetime_t time)
{
    if (time < bus->exchange_clock_datetime) {
        printf("%s\n", "exchange clock datetime out of order");
        return;
    }
    bus->exchange_clock_datetime = time;
}
