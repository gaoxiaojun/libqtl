/******************************************************************************
 * Quantitative Trading Library                                               *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#include "qtl_etree.h"
#include <qtl.h>
#include <assert.h>

struct tree_node_s {
  event_queue_t *queue;
  datetime_t timestamp;

  tree_node_t *left;
  tree_node_t *right;
  tree_node_t *next;
  tree_node_t *prev;
  tree_node_t *head;
};

static tree_node_t *node_new(event_queue_t *q) {
  tree_node_t *node = malloc(sizeof(tree_node_t));
  if (!node)
    return NULL;

  node->queue = q;
  node->head = node;
  return node;
}

void event_tree_rebalance(event_tree_t *tree, tree_node_t *item);

int event_tree_init(event_tree_t *tree) {
  assert(tree);
  tree->root = NULL;
  tree->last = NULL;
  return 0;
}

void event_tree_destory(event_tree_t *tree) {
  assert(tree);
  event_tree_clear(tree);
}

bool event_tree_is_empty(event_tree_t *tree) {
  assert(tree);
  if (!tree->root && !tree->last) {
    return true;
  }
  if (tree->last == NULL) {
    return false;
  }
  if (event_queue_is_empty(tree->last->queue)) {
    return true;
  }
  event_tree_rebalance(tree, tree->last);
  tree->last = NULL;
  return false;
}

event_t *event_tree_read(event_tree_t *tree) {
  assert(tree);
  assert(tree->last); // event_tree_read: Can not read from a tree with empty queue
  tree_node_t *current = tree->root;
  tree_node_t *parent = NULL;
  while (current->left) {
    parent = current;
    current = current->left;
  }
  if (current->prev) {
    tree_node_t *temp = current->head;
    current->head->next->prev = NULL;
    current->head = current->head->next;
    current = temp;
  } else {
    if (parent != NULL) {
      parent->left = current->right;
    } else {
      tree->root = current->right;
    }
    current->right = NULL;
  }

  event_t *e;
  if (!event_queue_pop(current->queue, &e))
      return NULL;

  if (event_get_type(e) == kOnQueueClosed) {
    if (!tree->root) {
      e = new_on_simulation_stop_event();
    }
  } else if (event_queue_is_empty(current->queue)) {
    tree->last = current;
  } else {
    event_tree_rebalance(tree, current);
  }

  return e;
}

void event_tree_rebalance(event_tree_t *tree, tree_node_t *item) {
  assert(tree);
  assert(item);
  item->right = NULL;
  item->left = NULL;
  item->next = NULL;
  item->prev = NULL;

  item->head = item;
  item->timestamp = event_queue_peek_time(item->queue);

  if (!tree->root) {
    tree->root = item;
    return;
  }

  tree_node_t *current = tree->root;
  tree_node_t *parent = NULL;

  while (!(current->timestamp == item->timestamp)) {
    if (item->timestamp > current->timestamp) {
      if (!current->right) {
        current->right = item;
        return;
      }
      parent = current;
      current = current->right;
    } else {
      if (!current->left) {
        current->left = item;
        return;
      }
      parent = current;
      current = current->left;
    }
  }
  item->prev = current;
  current->next = item;
  item->head = current->head;
  item->right = current->right;
  item->left = current->left;
  if (parent == NULL) {
    tree->root = item;
    return;
  }
  if (item->timestamp > parent->timestamp) {
    parent->right = item;
    return;
  }
  parent->left = item;
}

bool event_tree_add(event_tree_t *tree, event_queue_t *queue) {
  if (event_queue_is_empty(queue))
    return false;

  tree_node_t *item = node_new(queue);
  if (!item)
    return false;

  if (tree->root == NULL) {
    tree->root = item;
    return true;
  }
  event_tree_rebalance(tree, item);
  return true;
}

void event_tree_remove(event_tree_t *tree, event_queue_t *queue) {
  (void*)(tree);
  (void*)(queue);
}

void event_tree_clear(event_tree_t *tree) {
  assert(tree);
  //TODO:
  tree->root = NULL;
  tree->last = NULL;
}
