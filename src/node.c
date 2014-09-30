#include "node.h"
#include "karray.h"
#include <gc/gc.h>
#include <stdio.h>

DEF_ARRAY_STRUCT0(NODE, unsigned short);
DEF_ARRAY_T(NODE);

struct NODE {
    uint8_t *tag;
    uint8_t *consume_text;
    unsigned pos;
    unsigned length;
    ARRAY(NODE) node_list;
};

DEF_ARRAY_OP(NODE);

NODE *NODE_New(unsigned type, size_t pos)
{
    NODE *self = (NODE *) GC_MALLOC(sizeof(NODE));
    self->pos = pos;
    self->length = 0;
    ARRAY_init(NODE, &self->node_list, 1);
    return self;
}

void NODE_SetTag(NODE *self, uint8_t *bdata, InputSource *input)
{
    self->tag = bdata;
    if (ARRAY_size(self->node_list) == 0) {
        self->length = input->pos - self->pos;
        self->consume_text = InputSource_CopyText(input, self->pos, self->length);
    }
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
    if (node) {
        for (i = 0; i < level; i++) {
            fprintf(stderr, "  ");
        }
        fprintf(stderr, "{#%s ", node->tag);
        if (ARRAY_size(node->node_list) == 0) {
            fprintf(stderr, "'%s'", NODE_GetConsumeText(node));
        }
        else {
            fprintf(stderr, "\n");
            FOR_EACH_ARRAY(node->node_list, x, e) {
                NODE_dump(x, level + 1);
            }
            for (i = 0; i < level; i++) {
                fprintf(stderr, "  ");
            }
        }
        fprintf(stderr, "}\n");
    }
    else {
        fprintf(stderr, "%p tag:null\n", node);
    }
}
