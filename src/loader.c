#include "loader.h"
#include "pegvm.h"
#include "karray.h"
#include <assert.h>

static const char *get_opname(uint8_t opcode)
{
    switch (opcode) {
#define OP_CASE(OP) case PEGVM_OP_##OP : return "" #OP;
        PEGVM_OP_EACH(OP_CASE);
        default:
        assert(0 && "UNREACHABLE");
        break;
#undef OP_CASE
    }
    return "";
}

typedef struct Instruction {
    long opcode;
    union {
        unsigned long ndata;
        uint8_t *bdata;
        struct Instruction *dst;
    };
} Instruction;

static Instruction *Emit_EXIT(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    self->ndata = 0;
    return self;
}
static Instruction *Emit_JUMP(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_CALL(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_RET(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_IFSUCC(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_IFFAIL(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_NOP(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_Failure(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchText(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchByteChar(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchCharset(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchAnyChar(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchTextNot(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchByteCharNot(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchCharsetNot(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchOptionalText(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchOptionalByteChar(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchOptionalCharset(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_RememberPosition(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_CommitPosition(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_BacktrackPosition(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_RememberSequencePosition(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_CommitSequencePosition(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_BackTrackSequencePosition(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_RememberFailurePosition(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_UpdateFailurePosition(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_ForgetFailurePosition(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_StoreObject(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_DropStoredObject(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_RestoreObject(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_RestoreObjectIfFailure(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_RestoreNegativeObject(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_ConnectObject(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_DisableTransCapture(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_EnableTransCapture(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_NewObject(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_LeftJoinObject(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_CommitObject(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_RefreshStoredObject(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_Tagging(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_Value(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_Indent(Instruction *self, uint32_t ndata, uint8_t *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}

typedef Instruction *(*inst_load_t)(Instruction *self, uint32_t ndata, uint8_t *bdata);
static inst_load_t f_inst[] = {
#define DEFINE_LOAD_F(OP) Emit_##OP,
    PEGVM_OP_EACH(DEFINE_LOAD_F)
#undef DEFINE_LOAD_F
};

DEF_ARRAY_STRUCT(uint8_t);
DEF_ARRAY_T(uint8_t);
DEF_ARRAY_OP_NOPOINTER(uint8_t);
DEF_ARRAY_STRUCT(Instruction);
DEF_ARRAY_T_OP(Instruction);

PegVMInstruction *ByteCodeLoader_Load(InputSource *input)
{
    size_t i;
    int idx = 0;
    Instruction inst, *instp;
    ARRAY(uint8_t) buf;
    ARRAY(Instruction) insts;
    ARRAY_init(uint8_t, &buf, 1);
    ARRAY_init(Instruction, &insts, 1);

    for (i = 0; i < input->length; i++) {
        uint8_t opcode = InputSource_GetUint8(input);
        uint8_t bdata = 0;
        uint32_t ndata;
        ndata = InputSource_GetUint8(input);
        ndata = (ndata) | (InputSource_GetUint8(input) << 8);
        ndata = (ndata) | (InputSource_GetUint8(input) << 16);
        ndata = (ndata) | (InputSource_GetUint8(input) << 24);
        if (opcode == 128) {
            break;
        }
        ARRAY_clear(buf);
        while ((bdata = InputSource_GetUint8(input)) != 0) {
            ARRAY_add(uint8_t, &buf, bdata);
        }
        fprintf(stderr, "[%04d] op=%-28s ndata=%08d ", idx, get_opname(opcode), ndata);
        fprintf(stderr, "bdata=%s", (char *)ARRAY_list(buf));
        inst.opcode = opcode;
        instp = f_inst[opcode](&inst, ndata, NULL);
        ARRAY_add(Instruction, &insts, instp);
        idx++;
    }
    ARRAY_dispose(uint8_t, &buf);
    ARRAY_dispose(Instruction, &insts);
    return NULL;
}
