#include <stdint.h>
#ifndef NODE_H
#define NODE_H

typedef struct NODE NODE;

NODE *NODE_New(unsigned type);
void NODE_Dispose(NODE *self);
void NODE_SetTag(NODE *self, uint8_t *bdata, int length);
void NODE_AppendChild(NODE *parent, NODE *node);

enum node_type {
  NODE_TYPE_DEFAULT
};
#endif /* end of include guard */
