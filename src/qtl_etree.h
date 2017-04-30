/******************************************************************************
 * Quantitative Trading Library                                               *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#ifndef __EVENT_TREE_H__
#define __EVENT_TREE_H__

#include <qtl.h>
#include "qtl_event.h"
#include <assert.h>
#include "qtl_queue.h"

typedef struct tree_node_s tree_node_t;

typedef struct event_tree_s {
  tree_node_t *root;
  tree_node_t *last;
} event_tree_t;

inline int event_tree_init(event_tree_t *tree);

inline void event_tree_destory(event_tree_t *tree);

inline bool event_tree_is_empty(event_tree_t *tree);

inline event_t *event_tree_read(event_tree_t *tree);

inline bool event_tree_add(event_tree_t *tree, event_queue_t *queue);

inline void event_tree_remove(event_tree_t *tree, event_queue_t *queue);

inline void event_tree_clear(event_tree_t *tree);

#endif // __EVENT_TREE_H__
