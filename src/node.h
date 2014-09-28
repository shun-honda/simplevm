#ifndef NODE_H
#define NODE_H

typedef struct NODE {
    unsigned type;
    unsigned flag;
} NODE;

NODE *NODE_New(unsigned type, unsigned flag);
void NODE_Dispose(NODE *self);
#endif /* end of include guard */
