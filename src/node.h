#include <stdint.h>
#include "input_source.h"
#ifndef NODE_H
#define NODE_H

typedef struct NODE NODE;

NODE *NODE_New(unsigned type, size_t pos);
void NODE_Dispose(NODE *self);
void NODE_SetTag(NODE *self, uint8_t *bdata, InputSource *input);
uint8_t *NODE_GetConsumeText(NODE *self);
void NODE_AppendChild(NODE *parent, NODE *node);
void NODE_dump(NODE *node, int level);

enum node_type {
  NODE_TYPE_DEFAULT
};
#endif /* end of include guard */
