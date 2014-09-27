#include "input_source.h"

#ifndef PEGVM_H
#define PEGVM_H
#define PARSER_CONTEXT_MAX_ERROR_LENGTH 256

typedef struct ParserContext {
    InputSource *current_source;
    char last_error[PARSER_CONTEXT_MAX_ERROR_LENGTH];
} ParserContext;

void ParserContext_Init(ParserContext *context);
void ParserContext_Destruct(ParserContext *context);
int ParserContext_LoadSyntax(ParserContext *context, const char *file);
int ParserContext_Execute(ParserContext *context, InputSource *input);
int ParserContext_ParseFiles(ParserContext *context, int argc, char *const *argv);

#endif /* end of include guard */
