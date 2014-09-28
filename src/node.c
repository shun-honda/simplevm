#include "node.h"
#include "karray.h"
#include <gc/gc.h>

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
