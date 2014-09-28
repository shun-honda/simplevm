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
    context->stack_pointer = context->stack_pointer_base;
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
    PegVMInstruction *insts;
    for (i = 0; i < argc; i++) {
        if (InputSource_Init(&is, argv[i]) == NULL) {
            ParserContext_SetError(context,
                    "parse error: cannot load input file(%s)", argv[i]);
            return 1;
        }

        // load EXIT
        insts = context->instructions;
        PUSH_SP((long)insts);
        insts += 2; // skip dummy inst

        if (ParserContext_Execute(context, insts, &is)) {
            ParserContext_SetError(context,
                    "parse error: input file(%s)", argv[i]);
            InputSource_Dispose(&is);
            return 1;
        }
        InputSource_Dispose(&is);
    }
    return 0;
}

static inline int ParserContext_IsFailure(ParserContext *context)
{
    return context->current_node == NULL;
}

#define MAX(A, B) ((A)>(B)?(A):(B))
static inline void ParserContext_RecordFailurePos(ParserContext *context, InputSource *input)
{
    context->failure_pos = MAX(input->pos, context->failure_pos);
    context->current_node = NULL;
}

int ParserContext_Execute(ParserContext *context, Instruction *inst, InputSource *input)
{
    while (1) {
L_head:
        switch (inst->opcode) {
#define OP_CASE(OP) case PEGVM_OP_##OP: PegVMInstruction_dump(inst, 1);/*asm volatile("int3");*/
#define DISPATCH_NEXT ++inst; break
            OP_CASE(EXIT) {
                return 0;
            }
            OP_CASE(JUMP) {
                JUMP(inst->dst);
            }
            OP_CASE(CALL) {
                PUSH_SP((long)inst);
                JUMP(inst->dst);
            }
            OP_CASE(RET) {
                inst = (PegVMInstruction *)POP_SP();
                DISPATCH_NEXT;
            }
            OP_CASE(IFSUCC) {
                if (!ParserContext_IsFailure(context)) {
                    JUMP(inst->dst);
                }
                DISPATCH_NEXT;
            }
            OP_CASE(IFFAIL) {
                if (ParserContext_IsFailure(context)) {
                    JUMP(inst->dst);
                }
                DISPATCH_NEXT;
            }
            OP_CASE(NOP) {
                DISPATCH_NEXT;
            }
            OP_CASE(Failure) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchText) {
                uint8_t c = InputSource_GetUint8(input);
                if (c != (uint8_t)inst->ndata) {
                    ParserContext_RecordFailurePos(context, input);
                }
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
                uint8_t c = InputSource_GetUint8(input);
                if (c == (uint8_t)-1) {
                    // FIXME support unicode
                    ParserContext_RecordFailurePos(context, input);
                }
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
                PUSH_SP(input->pos);
                DISPATCH_NEXT;
            }
            OP_CASE(CommitPosition) {
                POP_SP();//input->pos
                DISPATCH_NEXT;
            }
            OP_CASE(BacktrackPosition) {
                size_t pos = (size_t)POP_SP();
                //FIXME add stats
                input->pos = pos;
                DISPATCH_NEXT;
            }
            OP_CASE(RememberSequencePosition) {
                PUSH_SP(input->pos);
                PUSH_SP(input->pos);
                PUSH_SP((long)context->current_node);
                DISPATCH_NEXT;
            }
            OP_CASE(CommitSequencePosition) {
                POP_SP();//(long)context->current_node
                POP_SP();//input->pos
                POP_SP();//input->pos
                DISPATCH_NEXT;
            }
            OP_CASE(BackTrackSequencePosition) {
                context->current_node = (NODE *)POP_SP();
                POP_SP();//input->pos
                POP_SP();//input->pos
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
                POP_SP();//input->pos
                DISPATCH_NEXT;
            }
            OP_CASE(StoreObject) {
                PUSH_SP((long)context->current_node);
                DISPATCH_NEXT;
            }
            OP_CASE(DropStoredObject) {
                POP_SP();//(long)context->current_node
                DISPATCH_NEXT;
            }
            OP_CASE(RestoreObject) {
                context->current_node = (NODE *)POP_SP();
                DISPATCH_NEXT;
            }
            OP_CASE(RestoreObjectIfFailure) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(RestoreNegativeObject) {
                NODE *node = (NODE *)POP_SP();
                if (ParserContext_IsFailure(context)) {
                    context->current_node = node;
                }
                else {
                    ParserContext_RecordFailurePos(context, input);
                }
                DISPATCH_NEXT;
            }
            OP_CASE(ConnectObject) {
                NODE *parent = (NODE *)POP_SP();
                NODE_AppendChild(parent, context->current_node);
                context->current_node = parent;
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
                context->current_node = NODE_New(NODE_TYPE_DEFAULT);
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
                long length = input->pos - TOP_SP();
                NODE_SetTag(context->current_node, inst->bdata, length);
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
