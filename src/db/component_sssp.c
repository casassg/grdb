#include "graph.h"
#include "cli.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <priority_queue.h>
#include <dinamic_array.h>
#include <curses.h>

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
//    Start with 2 * vertexid size as the edge file starts with 2 id files
    while (weight_attribute) {
        if (weight_attribute->bt == INTEGER) {
            break;
        }
        weight_attribute = weight_attribute->next;
    }
    if (!weight_attribute) {
        return -1;
    }

    char filename[BUFSIZE];
    memset(filename, 0, BUFSIZE);
    sprintf(filename, "%s/%d/%d/e", grdbdir, gno, cno);
    c->efd = open(filename, O_RDONLY);

    Array a;
    initArray(&a, 5);
    read_all_edges(c);
    struct edge *edge = c->e;
    while (edge != NULL) {
        int found_id1 = FALSE;
        int found_id2 = FALSE;
        int i;
        for (i = 0; i < a.used; i++) {
            if (a.array[i] == edge->id1) {
                found_id1 = TRUE;
            }
            if (a.array[i] == edge->id2) {
                found_id2 = TRUE;
            }
        }

        if (!found_id1) {
            insertArray(&a, edge->id1);
        }


        if (!found_id2) {
            insertArray(&a, edge->id2);
        }
        edge = edge->next;

    }
    int i;
    for (i = 0; i < a.used; i++) {
        printf("%d\n", a.array[i]);
    }

    /*
     * Execute Dijkstra on the attribute you found for the specified
     * component
     */


    heap_t *h = create_heap();
    vertexid_t current = origin;
    int dist_curr = 0;

    while (current) {
        if (current == destination) {
            break;
        }

        current = pop_heap(h, &dist_curr);

    }
    if (current) {
        total_weight = &dist_curr;
        return 0;
    }










    /* Change this as needed */
    return (-1);
}
