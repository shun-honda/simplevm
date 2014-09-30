#include "input_source.h"
#include "loader.h"
#include "node.h"

#ifndef PEGVM_H
#define PEGVM_H
#define PARSER_CONTEXT_MAX_ERROR_LENGTH 256
#define PARSER_CONTEXT_MAX_STACK_LENGTH 1024

typedef struct ParserContext {
    long *stack_pointer;
    PegVMInstruction **call_stack_pointer;
    size_t failure_pos;
    NODE *current_node;
    InputSource *current_source;
    PegVMInstruction *instructions;
    long stack_pointer_base[PARSER_CONTEXT_MAX_STACK_LENGTH];
    PegVMInstruction *call_stack_pointer_base[PARSER_CONTEXT_MAX_STACK_LENGTH];
    char last_error[PARSER_CONTEXT_MAX_ERROR_LENGTH];
} ParserContext;

void ParserContext_Init(ParserContext *context);
void ParserContext_Dispose(ParserContext *context);
int ParserContext_LoadSyntax(ParserContext *context, const char *file);
int ParserContext_Execute(ParserContext *context, PegVMInstruction *inst, InputSource *input);
int ParserContext_ParseFiles(ParserContext *context, int argc, char *const *argv);

#define PEGVM_OP_EACH(OP)\
        OP(EXIT)\
        OP(JUMP)\
        OP(CALL)\
        OP(RET)\
        OP(IFSUCC)\
        OP(IFFAIL)\
        OP(NOP)\
        OP(Failure)\
        OP(MatchText)\
        OP(MatchByteChar)\
        OP(MatchCharset)\
        OP(MatchAnyChar)\
        OP(MatchTextNot)\
        OP(MatchByteCharNot)\
        OP(MatchCharsetNot)\
        OP(MatchOptionalText)\
        OP(MatchOptionalByteChar)\
        OP(MatchOptionalCharset)\
        OP(RememberPosition)\
        OP(CommitPosition)\
        OP(BacktrackPosition)\
        OP(RememberSequencePosition)\
        OP(CommitSequencePosition)\
        OP(BackTrackSequencePosition)\
        OP(RememberFailurePosition)\
        OP(UpdateFailurePosition)\
        OP(ForgetFailurePosition)\
        OP(StoreObject)\
        OP(DropStoredObject)\
        OP(RestoreObject)\
        OP(RestoreObjectIfFailure)\
        OP(RestoreNegativeObject)\
        OP(ConnectObject)\
        OP(DisableTransCapture)\
        OP(EnableTransCapture)\
        OP(NewObject)\
        OP(LeftJoinObject)\
        OP(CommitObject)\
        OP(RefreshStoredObject)\
        OP(Tagging)\
        OP(Value)\
        OP(Indent)

enum pegvm_opcode {
#define DEFINE_ENUM(NAME) PEGVM_OP_##NAME,
    PEGVM_OP_EACH(DEFINE_ENUM)
#undef DEFINE_ENUM
    PEGVM_OP_ERROR = -1
};

#endif /* end of include guard */
