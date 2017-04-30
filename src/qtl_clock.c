/******************************************************************************
 * Quantitative Trading Library                                               *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#include "qtl_clock.h"
#include <assert.h>

struct clock_s {
  ClockType type;
  ClockMode mode;
  datetime_t time;
  reminder_queue_t *queue;
};

ctf_clock_t *clock_new(ClockType type) {
  ctf_clock_t *c = malloc(sizeof(ctf_clock_t));
  if (!c)
    return NULL;

  c->type = type;
  c->mode = kClockModeSimulation;
  c->queue = reminder_queue_new();
  if (!c->queue) {
    free(c);
    return NULL;
  }
  return c;
}

void clock_free(ctf_clock_t *c) {
  assert(c);
  reminder_queue_free(c->queue);
  free(c);
}

datetime_t clock_get_datetime(ctf_clock_t *c) {
  if (c->type == kClockTypeLocal)
    return c->time;
  if (c->mode != kClockModeRealtime)
    return c->time;
  return utc_now();
}

void clock_set_datetime(ctf_clock_t *c, datetime_t time) {
  if (c->type == kClockTypeExchange) {
    if (time < c->time) {
      printf("(Exchange) incorrect set orde\n");
      return;
    }
    c->time = time;
  } else {
    if (c->mode != kClockModeSimulation) {
      printf(
          "Can not set dateTime because Clock is not in the Simulation mode\n");
      return;
    }
    if (time < c->time) {
      printf("(Local) incorrect set orde\n");
      return;
    }
    c->time = time;
  }
}

ClockType clock_get_type(ctf_clock_t *c) { return c->type; }

ClockMode clock_get_mode(ctf_clock_t *c) { return c->mode; }

void clock_set_mode(ctf_clock_t *c, ClockMode mode) {
  c->mode = mode;
  if (mode == kClockModeSimulation)
    c->time = MIN_DATE_TIME;
}

void clock_clear(ctf_clock_t *c) {
    reminder_queue_clear(c->queue);
    c->time = MIN_DATE_TIME;
}

reminder_t *clock_add_reminder(ctf_clock_t *c, datetime_t time,
                               reminder_cb callback, void *data) {
  // TODO:
  return NULL;
}

void clock_remove_reminder(ctf_clock_t *c, datetime_t time, reminder_cb callback) {}

reminder_queue_t *clock_get_queue(ctf_clock_t *c) { return c->queue; }
