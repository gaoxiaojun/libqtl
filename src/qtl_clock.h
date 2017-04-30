/******************************************************************************
 * Quantitative Trading Library                                               *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#ifndef CTF_CLOCK_H
#define CTF_CLOCK_H

#include <qtl.h>

#include "qtl_rqueue.h"

typedef struct clock_s ctf_clock_t;

typedef enum { kClockTypeLocal, kClockTypeExchange } ClockType;
typedef enum { kClockModeRealtime, kClockModeSimulation } ClockMode;

ctf_clock_t * clock_new(ClockType type);
void clock_free(ctf_clock_t *c);

datetime_t clock_get_datetime(ctf_clock_t *c);
void clock_set_datetime(ctf_clock_t *c, datetime_t time);
ClockType clock_get_type(ctf_clock_t *c);
ClockMode clock_get_mode(ctf_clock_t *c);
void clock_set_mode(ctf_clock_t *c, ClockMode mode);
void clock_clear(ctf_clock_t *c);
reminder_t* clock_add_reminder(ctf_clock_t *c, datetime_t time, reminder_cb callback, void *data);
void clock_remove_reminder(ctf_clock_t *c, datetime_t time, reminder_cb callback);
reminder_queue_t *clock_get_queue(ctf_clock_t *c);

#endif // CTF_CLOCK_H
