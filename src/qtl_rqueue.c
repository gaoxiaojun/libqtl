/******************************************************************************
 * Quantitative Trading Library                                               *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#include "qtl_rqueue.h"
#include "adt_tree.h"

typedef struct rb_node_s {
  RB_ENTRY(rb_node_s) rbentry;
  reminder_t *reminder_event;

} rb_node_t;

int rb_node_comp(rb_node_t *lhs, rb_node_t *rhs) {
  datetime_t lhs_time = event_get_time(lhs->reminder_event);
  datetime_t rhs_time = event_get_time(rhs->reminder_event);
  if (lhs_time < rhs_time)
    return -1;
  if (lhs_time > rhs_time)
    return 1;

  if (lhs->reminder_event->callback < rhs->reminder_event->callback)
    return -1;
  if (lhs->reminder_event->callback > rhs->reminder_event->callback)
    return 1;
  return 0;
}

RB_HEAD(rbtree, rb_node_s);

struct reminder_queue_s {
  struct rbtree tree;
};

RB_GENERATE_STATIC(rbtree, rb_node_s, rbentry, rb_node_comp)

static bool add_reminder(reminder_queue_t *q, reminder_t *r) {
  rb_node_t *node = malloc(sizeof(rb_node_t));
  if (!node)
    return false;

  RB_INSERT(rbtree, &q->tree, node);
}

reminder_queue_t *reminder_queue_new() {
  reminder_queue_t *q = malloc(sizeof(reminder_queue_t));
  if (!q)
    return NULL;
  RB_INIT(&q->tree);
  return q;
}

void reminder_queue_free(reminder_queue_t *q) {
  reminder_queue_clear(q);
  free(q);
}

/* if queue is full, push will return false */
bool reminder_queue_push(reminder_queue_t *q, reminder_t *e) {
  rb_node_t *node = malloc(sizeof(rb_node_t));
  if (!node)
    return false;
  node->reminder_event = e;
  RB_INSERT(rbtree, &q->tree, node);
}

/* if queue is empty, pop will return false */
bool reminder_queue_pop(reminder_queue_t *q, reminder_t **ritem) {
  rb_node_t *node = RB_MIN(rbtree, &q->tree);

  if (node) {
    *ritem = node->reminder_event;
    RB_REMOVE(rbtree, &q->tree, node);
    free(node);
    return true;
  }
  return false;
}

/* if queue is empty, peek will return null pointer */
reminder_t *reminder_queue_peek(reminder_queue_t *q) {
  rb_node_t *node = RB_MIN(rbtree, &q->tree);

  if (node)
    return node->reminder_event;
  else
    return NULL;
}

bool reminder_queue_is_empty(reminder_queue_t *q) { return RB_EMPTY(&q->tree); }

void reminder_queue_clear(reminder_queue_t *q) {
  rb_node_t *np;
  rb_node_t *nn;
  RB_FOREACH_SAFE(np, rbtree, &q->tree, nn) {
      RB_REMOVE(rbtree, &q->tree, np);
      free(np);
  }
}

bool reminder_queue_remove(reminder_queue_t *q, reminder_t *e) {
  struct rb_node_s node;
  node.reminder_event = e;
  struct rb_node_s *fnode;
  fnode = RB_FIND(rbtree, &q->tree, &node);
  if(fnode) {
      RB_REMOVE(rbtree, &q->tree, fnode);
      return true;
  }
  return false;
}
