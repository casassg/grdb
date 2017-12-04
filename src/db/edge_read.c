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
read_all_edges(component_t c) {
//    Create buffer for id reading
    char ids_buf[sizeof(vertexid_t) << 1];

//    Generate edge filename and open file
    char filename[BUFSIZE];
    memset(filename, 0, BUFSIZE);
    sprintf(filename, "%s/%d/%d/e", grdbdir, gno, cno);
    int fd = open(filename, O_RDONLY);

//    Checking that edge schema is loaded
    if (c->se == NULL) {
        printf("Edge schema needs to be loaded before reading edges");
        return -1;
    }

//    Pre-calculating edge schema size and total edge size
    ssize_t size = schema_size(c->se);
    ssize_t total_size = (size + (sizeof(vertexid_t) << 1));
//    Initializing edge structure
    struct edge *prev;
    prev = NULL;
    struct edge tmp;
    struct edge *e = &tmp;
    edge_init(e);
//    Saving first edge address into component
    c->e = e;

    for (;;) {
//        Read ids
        ssize_t len = read(fd, ids_buf, sizeof(vertexid_t) << 1);
        if (len == 0) {
            close(fd);
            return 0;
        }
        if (len < 0) {
            close(fd);
            return -1;
        }

//        Assign ids
        e->id1 = *((vertexid_t *) ids_buf);
        e->id2 = *((vertexid_t *) (ids_buf + sizeof(vertexid_t)));
//        Initialize edge tuple
        tuple_init(&(e->tuple), c->se);
        memset(e->tuple->buf, 0, size);
//        Read edge tuple
        len = read(fd, e->tuple->buf, size);
        if (len < 0) {
            close(fd);
            return -1;
        }

//        Fix pointers for iteration
        if (prev != NULL) {
            prev->next = e;

        }
        e->prev = prev;
        e->next = NULL;

//        Prepare new edge to be read
        prev = e;
        e = (struct edge *) malloc(total_size);
        edge_init(e);
    }
    return 0;

}