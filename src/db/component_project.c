#if _DEBUG
#include <errno.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cli.h"
#include "graph.h"

#if _DEBUG
void bufdump(char *buf, int size);
#endif

component_t
component_project(component_t c, string_pool_t v_attr,string_pool_t e_attr)
{
    struct component c_projected;
    component_init(&c_projected);



//	a
	/* XXX Unlink the old vertex file from its temporary file name */
}
