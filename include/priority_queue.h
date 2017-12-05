//
// Created by Admin on 11/28/17.
//

#ifndef GRDB_PRIORITY_QUEUE_H
#define GRDB_PRIORITY_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include "vertexid.h"

typedef struct {
    int priority;
    vertexid_t id;
    long prev;
} node_t;

typedef struct {
    node_t *nodes;
    int len;
    int size;
} heap_t;

void push_heap(heap_t *h, int priority, vertexid_t id, long prev);

vertexid_t pop_heap(heap_t *h, int *priority, long *prev);

heap_t *create_heap();

#endif //GRDB_PRIORITY_QUEUE_H
