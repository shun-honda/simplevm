//
//  vm.c
//  pegVM
//

#include "pegvm.h"
#include "input_source.h"
#include <string.h>
#include <stdarg.h>
#include <assert.h>

static void ParserContext_SetError(ParserContext *context, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));

void ParserContext_Init(ParserContext *context)
{
    memset(context, 0, sizeof(*context));
}

void ParserContext_Destruct(ParserContext *context)
{
}

static void ParserContext_SetError(ParserContext *context, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(context->last_error, PARSER_CONTEXT_MAX_ERROR_LENGTH, fmt, ap);
    va_end(ap);
}

int ParserContext_LoadSyntax(ParserContext *context, const char *file)
{
    InputSource is;
    if (InputSource_Init(&is, file) == NULL) {
        return 1;
    }
    context->instructions = ByteCodeLoader_Load(&is);
    InputSource_Dispose(&is);
    return 0;
}

int ParserContext_ParseFiles(ParserContext *context, int argc, char *const *argv)
{
    int i;
    InputSource is;
    for (i = 0; i < argc; i++) {
        if (InputSource_Init(&is, argv[i]) == NULL) {
            ParserContext_SetError(context,
                    "parse error: cannot load input file(%s)", argv[i]);
            return 1;
        }
        if (ParserContext_Execute(context, &is)) {
            ParserContext_SetError(context,
                    "parse error: input file(%s)", argv[i]);
            InputSource_Dispose(&is);
            return 1;
        }
        InputSource_Dispose(&is);
    }
    return 0;
}

int ParserContext_Execute(ParserContext *context, InputSource *input)
{
    PegVMInstruction *inst = context->instructions;
    while (1) {
        switch (inst->opcode) {
#define OP_CASE(OP) case PEGVM_OP_##OP:
#define DISPATCH_NEXT inst++; break
            OP_CASE(EXIT) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(JUMP) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(CALL) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(RET) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(IFSUCC) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(IFFAIL) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(NOP) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(Failure) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchText) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchByteChar) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchCharset) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchAnyChar) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchTextNot) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchByteCharNot) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchCharsetNot) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchOptionalText) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchOptionalByteChar) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchOptionalCharset) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(RememberPosition) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(CommitPosition) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(BacktrackPosition) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(RememberSequencePosition) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(CommitSequencePosition) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(BackTrackSequencePosition) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(RememberFailurePosition) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(UpdateFailurePosition) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(ForgetFailurePosition) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(StoreObject) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(DropStoredObject) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(RestoreObject) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(RestoreObjectIfFailure) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(RestoreNegativeObject) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(ConnectObject) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(DisableTransCapture) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(EnableTransCapture) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(NewObject) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(LeftJoinObject) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(CommitObject) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(RefreshStoredObject) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(Tagging) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(Value) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(Indent) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
        } // switch
    } // while
    return 1;
}
