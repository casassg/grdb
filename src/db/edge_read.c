#include <assert.h>
#include "cli.h"
#include <stdio.h>
#include <fcntl.h>

#if _DEBUG
#include <stdio.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "graph.h"

/*
 * The edge file is arranged as a packed list of edge records.  Each
 * record contains two 64-bit vertex ids followed by the edge tuple.  If
 * an error or some sort occurs, the value (-1) is returned.  The value
 * zero means the end-of-file was reached.  Otherwise, the number of bytes
 * read in for the vertex tuple are returned.
 */
ssize_t
edge_read(edge_t e, schema_t schema, int fd) {
    off_t off;
    ssize_t len, size;
    vertexid_t id1, id2;
    char buf[sizeof(vertexid_t) << 1];

    assert(e != NULL);
#if _DEBUG
    printf("edge_read: read edge (%llu,%llu)\n", e->id1, e->id2);
#endif
    if (schema == NULL)
        size = 0;
    else
        size = schema_size(schema);
#if _DEBUG
    printf("edge_read: schema size = %lu bytes\n", size);
#endif
    /* Search for edge in current component */
    for (off = 0;; off += (sizeof(vertexid_t) << 1) + size) {
        lseek(fd, off, SEEK_SET);
        len = read(fd, buf, sizeof(vertexid_t) << 1);
        if (len != sizeof(vertexid_t) << 1) {
#if _DEBUG
            printf("edge_read: ");
            printf("read %lu bytes to tuple buffer\n", len);
#endif
            return (-1);
        }
        id1 = *((vertexid_t *) buf);
        id2 = *((vertexid_t *) (buf + sizeof(vertexid_t)));

        if (id1 == e->id1 && id2 == e->id2) {
            if (e->tuple == NULL)
                tuple_init(&(e->tuple), schema);

            memset(e->tuple->buf, 0, size);
            len = read(fd, e->tuple->buf, size);
#if _DEBUG
            printf("edge_read: ");
            printf("read %lu bytes to tuple buffer\n", len);
#endif
            return len;
        }
    }
    return 0;
}


ssize_t
read_all_edges(schema_t schema, component_t c) {
    off_t off;
    ssize_t len, size;
    vertexid_t id1, id2;
    char buf[sizeof(vertexid_t) << 1];


    char filename[BUFSIZE];
    memset(filename, 0, BUFSIZE);
    sprintf(filename, "%s/%d/%d/e", grdbdir, gno, cno);
    int fd = open(filename, O_RDONLY);

    if (schema == NULL)
        size = 0;
    else
        size = schema_size(schema);

    int total_size = (size + (sizeof(vertexid_t) << 1));
    struct edge *prev;
    prev = NULL;
    struct edge tmp;
    struct edge *e = &tmp;
    edge_init(e);
    c->e = e;

    /* Search for edge in current component */
    for (off = 0;; off += total_size) {
        len = read(fd, buf, sizeof(vertexid_t) << 1);

        if (len == 0) {
            return 0;
        }
        if (len < 0) {
            return -1;
        }
        id1 = *((vertexid_t *) buf);
        id2 = *((vertexid_t *) (buf + sizeof(vertexid_t)));

        e->id1 = id1;
        e->id2 = id2;
        tuple_init(&(e->tuple), schema);

        memset(e->tuple->buf, 0, size);
        len = read(fd, e->tuple->buf, size);
        if (len < 0) {
            return -1;
        }
        if (prev != NULL) {
            prev->next = e;

        }
        e->prev = prev;
        e->next = NULL;
        prev = e;

        e = (struct edge *) malloc(total_size);
        edge_init(e);
    }
    return 0;

}