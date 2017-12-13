#if _DEBUG
#include <errno.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "cli.h"
#include "graph.h"

#if _DEBUG
void bufdump(char *buf, int size);
#endif

int
component_project(component_t c, string_pool_t v_attr, string_pool_t e_attr, int out_cno) {
    char *buff[BUFSIZ];

    if (out_cno == cno) {
        printf("Project needs a new component");
        return -1;
    }

    /* Create new component folder */
    memset(buff, 0, BUFSIZE);
    sprintf(buff, "%s/%d/%d", grdbdir, gno, out_cno);
    mkdir(buff, 0755);

    /* Open vertex file for reading */
    memset(buff, 0, BUFSIZE);
    sprintf(buff, "%s/%d/%d/v", grdbdir, gno, cno);
    int in = open(buff, O_RDONLY | O_CREAT, 0644);
    if (in < 0)
        return -1;

    /* Open vertex file for writing */
    memset(buff, 0, BUFSIZE);
    sprintf(buff, "%s/%d/%d/v", grdbdir, gno, out_cno);
    int out = open(buff, O_WRONLY | O_CREAT, 0644);
    if (out < 0)
        return -1;

    for (;;) {
        /* Read vertex id for vertex and write it */
        memset(buff, 0, BUFSIZE);
        ssize_t len = read(in, buff, sizeof(vertexid_t));
        if (len == 0) {
            break;
        }
        if (len < 0) {
            close(in);
            close(out);
            return -1;
        }
        write(out, buff, sizeof(vertexid_t));

        /* Read attrs for vertex and write it if in string pool */
        if (c->sv == NULL) {
            continue;
        }
        struct attribute *curr_attr = c->sv->attrlist;
        while (curr_attr != NULL) {
            memset(buff, 0, BUFSIZE);
            len = read(in, buff, base_types_len[curr_attr->bt]);
            if (len == 0) {
                break;
            }
            if (len < 0) {
                close(in);
                close(out);
                return -1;
            }

            if (string_pool_find_idx_by_name(v_attr, curr_attr->name) >= 0)
                write(out, buff, base_types_len[curr_attr->bt]);
            curr_attr = curr_attr->next;
        }
    }
    close(in);
    close(out);


    /* Update vertex schema and write */
    if (c->sv) {
        struct attribute *curr_attr = c->sv->attrlist;
        struct attribute *begin_new_attr = NULL;
        struct attribute *last_new_attr = NULL;
        while (curr_attr != NULL) {
            if (string_pool_find_idx_by_name(v_attr, curr_attr->name) >= 0) {
                if (begin_new_attr == NULL) {
                    begin_new_attr = curr_attr;
                }
                if (last_new_attr != NULL) {
                    last_new_attr->next = curr_attr;
                }
                last_new_attr = curr_attr;
            }

            curr_attr = curr_attr->next;
        }
        if (last_new_attr!=NULL)
            last_new_attr->next = NULL;
        c->sv->attrlist = begin_new_attr;
        memset(buff, 0, BUFSIZE);
        sprintf(buff, "%s/%d/%d/sv", grdbdir, gno, out_cno);
        int out_schema = open(buff, O_WRONLY | O_CREAT, 0644);
        schema_write(c->sv, out_schema);
        close(out_schema);
    }

    /* Open edge file for reading */
    memset(buff, 0, BUFSIZE);
    sprintf(buff, "%s/%d/%d/e", grdbdir, gno, cno);
    in = open(buff, O_RDONLY | O_CREAT, 0644);
    if (in < 0)
        return -1;


    /* Open edge file for writing */
    memset(buff, 0, BUFSIZE);
    sprintf(buff, "%s/%d/%d/e", grdbdir, gno, out_cno);
    out = open(buff, O_WRONLY | O_CREAT, 0644);
    if (out < 0)
        return -1;

    for (;;) {
        /* Read vertex id for vertex and write it */
        memset(buff, 0, BUFSIZE);
        ssize_t len = read(in, buff, sizeof(vertexid_t)<<1);
        if (len == 0) {
            break;
        }
        if (len < 0) {
            close(in);
            close(out);
            return -1;
        }
        write(out, buff, sizeof(vertexid_t)<<1);

        /* Read attrs for vertex and write it if in string pool */
        if (c->se == NULL) {
            continue;
        }
        struct attribute *curr_attr = c->se->attrlist;
        while (curr_attr != NULL) {
            memset(buff, 0, BUFSIZE);
            len = read(in, buff, base_types_len[curr_attr->bt]);
            if (len == 0) {
                break;
            }
            if (len < 0) {
                close(in);
                close(out);
                return -1;
            }

            if (string_pool_find_idx_by_name(e_attr, curr_attr->name) >= 0)
                write(out, buff, base_types_len[curr_attr->bt]);
            curr_attr = curr_attr->next;
        }
    }
    close(in);
    close(out);


    /* Update vertex schema and write */
    if (c->se) {
        struct attribute *curr_attr = c->se->attrlist;
        struct attribute *begin_new_attr = NULL;
        struct attribute *last_new_attr = NULL;
        while (curr_attr != NULL) {
            if (string_pool_find_idx_by_name(e_attr, curr_attr->name) >= 0) {
                if (begin_new_attr == NULL) {
                    begin_new_attr = curr_attr;
                }
                if (last_new_attr != NULL) {
                    last_new_attr->next = curr_attr;
                }
                last_new_attr = curr_attr;
            }

            curr_attr = curr_attr->next;
        }
        if (last_new_attr!=NULL)
            last_new_attr->next = NULL;
        c->se->attrlist = begin_new_attr;
        memset(buff, 0, BUFSIZE);
        sprintf(buff, "%s/%d/%d/se", grdbdir, gno, out_cno);
        int out_schema = open(buff, O_WRONLY | O_CREAT, 0644);
        schema_write(c->se, out_schema);
        close(out_schema);
    }


    return 0;
}
