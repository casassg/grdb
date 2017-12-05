#include "graph.h"
#include "cli.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <priority_queue.h>
#include <dinamic_array.h>
#include <curses.h>

long reverse_index(Array *mapping_array, vertexid_t current);

/* Place the code for your Dijkstra implementation in this file */


int
component_sssp(
        component_t c,
        vertexid_t origin,
        vertexid_t destination,
        int *n,
        int *total_weight,
        vertexid_t **path) {


    /*
     * Figure out which attribute in the component edges schema you will
     * use for your weight function
     */
    struct attribute *weight_attribute = c->se->attrlist;
    int off_attr_weight = 0;
//    Start with 2 * vertexid size as the edge file starts with 2 id files
    while (weight_attribute) {
        if (weight_attribute->bt == INTEGER) {
            break;
        }
        off_attr_weight += base_types_len[weight_attribute->bt];
        weight_attribute = weight_attribute->next;
    }
    if (!weight_attribute) {
        return -1;
    }

    char filename[BUFSIZE];
    memset(filename, 0, BUFSIZE);
    sprintf(filename, "%s/%d/%d/e", grdbdir, gno, cno);
    c->efd = open(filename, O_RDONLY);

    Array mapping_array;
    initArray(&mapping_array, 5);
    read_all_edges(c);
    struct edge *edge = c->e;
    while (edge != NULL) {
        int found_id1 = FALSE;
        int found_id2 = FALSE;
        int i;
        for (i = 0; i < mapping_array.used; i++) {
            if (mapping_array.array[i] == edge->id1)
                found_id1 = TRUE;
            if (mapping_array.array[i] == edge->id2)
                found_id2 = TRUE;
        }

        if (!found_id1) {
            insertArray(&mapping_array, edge->id1);
        }
        if (!found_id2) {
            insertArray(&mapping_array, edge->id2);
        }

        edge = edge->next;
    }

    long prev[mapping_array.used];

    for (int i = 0; i < mapping_array.used; i++) {
        prev[i] = -1;
    }

    /*
     * Execute Dijkstra on the attribute you found for the specified
     * component
     */


    heap_t *h = create_heap();
    vertexid_t current = origin;
    int dist_curr = 0;
    long c_id = -1;

    while (current) {
        if (current == destination) {
            break;
        }
        edge = c->e;
        while (edge != NULL) {
            int weight = tuple_get_int(edge->tuple->buf + off_attr_weight);
            if (edge->id1 == current) {
                push_heap(h, dist_curr + weight, edge->id2, edge->id1);
            }
            edge = edge->next;
        }
        long prev_curr = -1;
        current = pop_heap(h, &dist_curr, &prev_curr);
        c_id = reverse_index(&mapping_array, current);
        prev[c_id] = prev_curr;
    }

    if (current) {
        *total_weight = dist_curr;
        long exp = prev[c_id];
        long exp_id = reverse_index(&mapping_array,exp);
        Array p_array;
        initArray(&p_array, 5);
        insertArray(&p_array, current);
        while (exp != -1) {
            insertArray(&p_array, exp);
            exp = prev[exp_id];
            exp_id = reverse_index(&mapping_array,exp);
        }
        *n = p_array.used;
        *path = malloc(sizeof(vertexid_t) * (*n));
        for (int i = 0; i < p_array.used; i++) {
            (*path)[p_array.used - i - 1] = p_array.array[i];
        }
        return 0;
    }

    /* Change this as needed */
    return (-1);
}

long reverse_index(Array *mapping_array, vertexid_t current) {
    int i;
    for (i = 0; i < (*mapping_array).used; i++) {
        if ((*mapping_array).array[i] == current) {
            break;
        }
    }
    return i;
}
