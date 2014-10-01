#include "node.h"
#include "karray.h"
#include <gc/gc.h>
#include <stdio.h>

typedef struct NODE *NODEPtr;
DEF_ARRAY_STRUCT0(NODEPtr, uintptr_t);
DEF_ARRAY_T(NODEPtr);

struct NODE {
    uint8_t *tag;
    uint8_t *text;
    unsigned pos;
    unsigned length;
    ARRAY(NODEPtr) node_list;
};

DEF_ARRAY_OP_NOPOINTER(NODEPtr);

NODE *NODE_New(unsigned type, size_t pos)
{
    NODE *self = (NODE *) GC_MALLOC(sizeof(NODE));
    self->pos = pos;
    self->length = 0;
    ARRAY_init(NODEPtr, &self->node_list, 0);
    return self;
}

void NODE_SetTag(NODE *self, uint8_t *bdata, InputSource *input)
{
    self->tag = bdata;
    if (ARRAY_size(self->node_list) == 0) {
        self->length = input->pos - self->pos;
        self->text = InputSource_CopyText(input, self->pos, self->length);
    }
}

void NODE_AppendChild(NODE *parent, NODE *node)
{
    ARRAY_add(NODEPtr, &parent->node_list, node);
}

NODE **Node_GetChildren(NODE *node, unsigned *length)
{
    *length = ARRAY_size(node->node_list);
    return ARRAY_list(node->node_list);
}

uint8_t *NODE_GetTag(NODE *self)
{
    return self->tag;
}

uint8_t *NODE_GetText(NODE *self)
{
    return self->text;
}

void NODE_Dispose(NODE *self)
{
    ARRAY_dispose(NODEPtr, &self->node_list);
    if (self->text) {
        self->text = NULL;
    }
}

void NODE_Dump(NODE *node, int level)
{
    int i;
    if (node) {
        for (i = 0; i < level; i++) {
            fprintf(stderr, "  ");
        }
        fprintf(stderr, "{#%s ", node->tag);
        if (ARRAY_size(node->node_list) == 0) {
            fprintf(stderr, "'%s'", node->text);
        }
        else {
            NODE **x, **e;
            fprintf(stderr, "\n");
            FOR_EACH_ARRAY(node->node_list, x, e) {
                NODE_Dump(*x, level + 1);
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
