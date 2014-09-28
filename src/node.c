#include "node.h"
#include <gc/gc.h>

NODE *NODE_New(unsigned type, unsigned flag)
{
    NODE *self = (NODE *) GC_malloc(sizeof(*self));
    self->type = type;
    self->flag = flag;
    return self;
}

void NODE_Dispose(NODE *self)
{
}
