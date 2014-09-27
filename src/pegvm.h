#include "input_source.h"
#include "loader.h"

#ifndef PEGVM_H
#define PEGVM_H
#define PARSER_CONTEXT_MAX_ERROR_LENGTH 256

typedef struct ParserContext {
    InputSource *current_source;
    PegVMInstruction *instructions;
    char last_error[PARSER_CONTEXT_MAX_ERROR_LENGTH];
} ParserContext;

void ParserContext_Init(ParserContext *context);
void ParserContext_Destruct(ParserContext *context);
int ParserContext_LoadSyntax(ParserContext *context, const char *file);
int ParserContext_Execute(ParserContext *context, InputSource *input);
int ParserContext_ParseFiles(ParserContext *context, int argc, char *const *argv);

enum pegvm_opcode {
    PEGVM_OP_EXIT = 0,
    PEGVM_OP_JUMP,
    PEGVM_OP_CALL,
    PEGVM_OP_RET,
    PEGVM_OP_IFSUCC,
    PEGVM_OP_IFFAIL,
    PEGVM_OP_MatchText,
    PEGVM_OP_MatchCharset,
    PEGVM_OP_MatchAnyChar,
    PEGVM_OP_RememberPosition,
    PEGVM_OP_CommitPosition,
    PEGVM_OP_BacktrackPosition,
    PEGVM_OP_RememberFailurePosition,
    PEGVM_OP_UpdateFailurePosition,
    PEGVM_OP_ForgetFailurePosition,
    PEGVM_OP_RememberSequencePosition,
    PEGVM_OP_CommitSequencePosition,
    PEGVM_OP_BackTrackSequencePosition,
    PEGVM_OP_StoreObject,
    PEGVM_OP_DropStoredObject,
    PEGVM_OP_RestoreObject,
    PEGVM_OP_RestoreNegativeObject,
    PEGVM_OP_ConnectObject,
    PEGVM_OP_NewObject,
    PEGVM_OP_CommitObject,
    PEGVM_OP_Tagging,
    PEGVM_OP_Indent,
    PEGVM_OP_ERROR = -1
};

#endif /* end of include guard */
