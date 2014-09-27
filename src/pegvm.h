#ifndef PEGVM_H
#define PEGVM_H

typedef struct ParserContext {
} ParserContext;

void ParserContext_Init(ParserContext *context);
void ParserContext_Destruct(ParserContext *context);
int ParserContext_LoadSyntax(ParserContext *context, const char *file);
int ParserContext_Execute(ParserContext *context, const char *input);

#endif /* end of include guard */
