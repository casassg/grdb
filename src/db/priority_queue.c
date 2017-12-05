//
// Created by Admin on 11/28/17.
//

#include <priority_queue.h>

void push_heap(heap_t *h, int priority, vertexid_t id, long prev) {
    if (h->len + 1 >= h->size) {
        h->size = h->size ? h->size * 2 : 4;
        h->nodes = (node_t *)realloc(h->nodes, h->size * sizeof (node_t));
    }
    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->nodes[j].priority > priority) {
        h->nodes[i] = h->nodes[j];
        i = j;
        j = j / 2;
    }
    h->nodes[i].priority = priority;
    h->nodes[i].id = id;
    h->nodes[i].prev = prev;
    h->len++;
}

vertexid_t pop_heap (heap_t *h, int* priority, long *prev) {
    int i, j, k;
    if (!h->len) {
        return NULL;
    }
    vertexid_t data = h->nodes[1].id;
    *priority =  h->nodes[1].priority;
    *prev = h->nodes[1].prev;
    h->nodes[1] = h->nodes[h->len];
    h->len--;
    i = 1;
    while (1) {
        k = i;
        j = 2 * i;
        if (j <= h->len && h->nodes[j].priority < h->nodes[k].priority) {
            k = j;
        }
        if (j + 1 <= h->len && h->nodes[j + 1].priority < h->nodes[k].priority) {
            k = j + 1;
        }
        if (k == i) {
            break;
        }
        h->nodes[i] = h->nodes[k];
        i = k;
    }
    h->nodes[i] = h->nodes[h->len + 1];
    return data;
}

heap_t* create_heap() {
    return (heap_t *) calloc(1, sizeof(heap_t));
}
