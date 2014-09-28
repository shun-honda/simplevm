#include "node.h"
#include "karray.h"
#include <gc/gc.h>
#include <stdio.h>

DEF_ARRAY_STRUCT0(NODE, unsigned short);
DEF_ARRAY_T(NODE);

struct NODE {
    uint8_t *tag;
    ARRAY(NODE) node_list;
};

DEF_ARRAY_OP(NODE);

NODE *NODE_New(unsigned type)
{
    NODE *self = (NODE *) GC_MALLOC(sizeof(NODE));
    ARRAY_init(NODE, &self->node_list, 1);
    return self;
}

void NODE_SetTag(NODE *self, uint8_t *bdata, int length)
{
    self->tag = bdata;
    // ARRAY_ensureSize(NODE, self->node_list, length);
}

void NODE_AppendChild(NODE *parent, NODE *node)
{
    ARRAY_add(NODE, &parent->node_list, node);
}

void NODE_Dispose(NODE *self)
{
    ARRAY_dispose(NODE, &self->node_list);
}

void NODE_dump(NODE *node, int level)
{
    NODE *x, *e;
    int i;
    for (i = 0; i < level; i++) {
        fprintf(stderr, "  ");
    }
    if (node) {
        fprintf(stderr, "%p tag:%s\n", node, node->tag);
        FOR_EACH_ARRAY(node->node_list, x, e) {
            NODE_dump(x, level + 1);
        }
    }
    else {
        fprintf(stderr, "%p tag:null\n", node);
    }
}
