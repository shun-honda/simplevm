//
//  vm.c
//  pegVM
//

#include "pegvm.h"
#include "input_source.h"
#include <gc/gc.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#define DUMP_PARSED_NODE 0
#define PEGVM_DEBUG 0
static inline void PUSH_IP(ParserContext *context, PegVMInstruction *ip);

static void ParserContext_SetError(ParserContext *context, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));

void ParserContext_Init(ParserContext *context)
{
    memset(context, 0, sizeof(*context));
    context->stack_pointer_base = (long *) GC_MALLOC(sizeof(long) * PARSER_CONTEXT_MAX_STACK_LENGTH);
    context->call_stack_pointer_base = (PegVMInstruction **) GC_MALLOC(sizeof(PegVMInstruction *) * PARSER_CONTEXT_MAX_STACK_LENGTH);
    context->stack_pointer = &context->stack_pointer_base[0];
    context->call_stack_pointer = &context->call_stack_pointer_base[0];
}

void ParserContext_Dispose(ParserContext *context)
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

static PegVMInstruction *ParserContext_PrepareIP(ParserContext *context)
{
    PegVMInstruction *insts;
    // load EXIT
    insts = context->instructions;
    PUSH_IP(context, insts - 1);
    insts += 1; // skip EXIT inst
    return insts;
}

int ParserContext_ParseFiles(ParserContext *context, int argc, char **argv)
{
    int i;
    InputSource is;
    PegVMInstruction *insts;
    assert(context->instructions && "load syntax file before parsing file");
    for (i = 0; i < argc; i++) {
        if (InputSource_Init(&is, argv[i]) == NULL) {
            ParserContext_SetError(context,
                    "parse error: cannot load input file(%s)", argv[i]);
            return 1;
        }

        insts = ParserContext_PrepareIP(context);

        // create root node
        context->current_node = NODE_New(NODE_TYPE_DEFAULT, is.pos);

        if (ParserContext_Execute(context, insts, &is)) {
            ParserContext_SetError(context,
                    "parse error: input file(%s)", argv[i]);
            InputSource_Dispose(&is);
            return 1;
        }
        if (PEGVM_DEBUG || DUMP_PARSED_NODE) {
            fprintf(stderr, "\nparsed:\n\n");
            NODE_Dump(context->current_node, 0);
            fprintf(stderr, "\n");
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
static inline void ParserContext_RecordFailurePos(ParserContext *context, InputSource *input, size_t consumed)
{
    context->failure_pos = MAX(input->pos, context->failure_pos);
    context->current_node = NULL;
    input->pos -= consumed;
}

// #define INC_SP(N) (context->stack_pointer += (N))
// #define DEC_SP(N) (context->stack_pointer -= (N))
static inline long INC_SP(ParserContext *context, int N)
{
    context->stack_pointer += (N);
    assert(context->stack_pointer >= context->stack_pointer_base &&
            context->stack_pointer < &context->stack_pointer_base[PARSER_CONTEXT_MAX_STACK_LENGTH]);
    return *context->stack_pointer;
}

static inline long DEC_SP(ParserContext *context, int N)
{
    context->stack_pointer -= N;
    assert(context->stack_pointer >= context->stack_pointer_base &&
            context->stack_pointer < &context->stack_pointer_base[PARSER_CONTEXT_MAX_STACK_LENGTH]);
    return *context->stack_pointer;
}

static inline void PUSH_IP(ParserContext *context, PegVMInstruction *ip)
{
    *context->call_stack_pointer++ = ip;
    assert(context->call_stack_pointer >= context->call_stack_pointer_base &&
            context->call_stack_pointer < &context->call_stack_pointer_base[PARSER_CONTEXT_MAX_STACK_LENGTH]);
}

static inline PegVMInstruction *POP_IP(ParserContext *context)
{
    --context->call_stack_pointer;
    assert(context->call_stack_pointer >= context->call_stack_pointer_base &&
            context->call_stack_pointer < &context->call_stack_pointer_base[PARSER_CONTEXT_MAX_STACK_LENGTH]);
    return *context->call_stack_pointer;
}

#define PUSH_SP(INST) (*context->stack_pointer = (INST), INC_SP(context, 1))
#define POP_SP(INST) (DEC_SP(context, 1))
#define TOP_SP() (*context->stack_pointer)
#define JUMP(DST) inst = (DST); goto L_head;

int ParserContext_Execute(ParserContext *context, Instruction *inst, InputSource *input)
{
    while (1) {
L_head:
        switch (inst->opcode) {

#define OP_CASE(OP) case PEGVM_OP_##OP: if (PEGVM_DEBUG) { PegVMInstruction_dump(inst, 1); } else {}
#define DISPATCH_NEXT ++inst; break
            OP_CASE(EXIT) {
                return 0;
            }
            OP_CASE(JUMP) {
                JUMP(inst->dst);
            }
            OP_CASE(CALL) {
                PUSH_IP(context, inst);
                JUMP(inst->dst);
            }
            OP_CASE(RET) {
                inst = POP_IP(context);
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
                if (PEGVM_DEBUG) {
                    fprintf(stderr, "T c='%c', n='%c'\n", (char)c, (char)inst->ndata);
                }
                if (c != (uint8_t)inst->ndata) {
                    ParserContext_RecordFailurePos(context, input, 1);
                }
                DISPATCH_NEXT;
            }
            OP_CASE(MatchByteChar) {
                assert(0 && "Not implemented");
                DISPATCH_NEXT;
            }
            OP_CASE(MatchCharset) {
                uint8_t c = InputSource_GetUint8(input);
                uint8_t *charset = inst->bdata;
                if (PEGVM_DEBUG) {
                    fprintf(stderr, "Charset c='%c'\n", (char)c);
                }
                if (!(charset[c / 8] & (1 << (c % 8)))) {
                    ParserContext_RecordFailurePos(context, input, 1);
                }
                DISPATCH_NEXT;
            }
            OP_CASE(MatchAnyChar) {
                uint8_t c = InputSource_GetUint8(input);
                if (PEGVM_DEBUG) {
                    fprintf(stderr, "A c='%c'\n", (char)c);
                }
                if (c == (uint8_t)-1) {
                    // FIXME support unicode
                    ParserContext_RecordFailurePos(context, input, 1);
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
                input->pos = POP_SP();//input->pos
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
                    //XXX(imasahiro) Is it ok to set the consumed length to 0?
                    ParserContext_RecordFailurePos(context, input, 0);
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
                context->current_node = NODE_New(NODE_TYPE_DEFAULT, input->pos);
                DISPATCH_NEXT;
            }
            OP_CASE(LeftJoinObject) {
                NODE *parent = NODE_New(NODE_TYPE_DEFAULT, input->pos);
                NODE_AppendChild(parent, context->current_node);
                context->current_node = parent;
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
                NODE_SetTag(context->current_node, inst->bdata, input);
                if (PEGVM_DEBUG) {
                    fprintf(stderr, "tag '%s'\ntext '%s'\n", inst->bdata, NODE_GetText(context->current_node));
                }
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
