#include "node.h"
#include "karray.h"
#include <gc/gc.h>
#include <stdio.h>

DEF_ARRAY_STRUCT0(NODE, unsigned short);
DEF_ARRAY_T(NODE);

struct NODE {
    uint8_t *tag;
    size_t pos_node;
    uint8_t *consume_text;
    ARRAY(NODE) node_list;
};

DEF_ARRAY_OP(NODE);

NODE *NODE_New(unsigned type, size_t pos)
{
    NODE *self = (NODE *) GC_MALLOC(sizeof(NODE));
    self->pos_node = pos;
    ARRAY_init(NODE, &self->node_list, 1);
    return self;
}

void NODE_SetTag(NODE *self, uint8_t *bdata, InputSource *input)
{
    self->tag = bdata;
    int length = input->pos - self->pos_node;
    self->consume_text = InputSource_GetText(input, self->pos_node, length);
    // ARRAY_ensureSize(NODE, self->node_list, length);
}

uint8_t *NODE_GetConsumeText(NODE *self)
{
    return self->consume_text;
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
        fprintf(stderr, "\n{");
        fprintf(stderr, "%s  '%s'",node->tag, NODE_GetConsumeText(node));
        FOR_EACH_ARRAY(node->node_list, x, e) {
            NODE_dump(x, level + 1);
        }
        fprintf(stderr, "}\n");
    }
    else {
        fprintf(stderr, "%p tag:null\n", node);
    }
}
