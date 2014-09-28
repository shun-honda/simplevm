#include "loader.h"
#include "pegvm.h"
#include "karray.h"
#include <assert.h>

DEF_ARRAY_STRUCT(uint8_t);
DEF_ARRAY_T(uint8_t);
DEF_ARRAY_OP_NOPOINTER(uint8_t);
DEF_ARRAY_STRUCT(Instruction);
DEF_ARRAY_T_OP(Instruction);

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

static uint8_t *pegvm_string_copy(ARRAY(uint8_t) *src)
{
    uint8_t *str;
    if (ARRAY_last(*src) != 0) {
        ARRAY_add(uint8_t, src, 0);
    }
    str = (uint8_t *) malloc(ARRAY_size(*src));
    memcpy(str, ARRAY_list(*src), ARRAY_size(*src));
    return str;
}

static uint8_t get_next_char(ARRAY(uint8_t) *bdata, size_t *pos)
{
    uint8_t c = ARRAY_get(uint8_t, bdata, *pos);
    if (c) {
        if (c == '\\' &&
                *pos + 1< ARRAY_size(*bdata) &&
                (c = ARRAY_get(uint8_t, bdata, *pos + 1))) {
            // peg4d only support \f, \n, \r, \t, \v
            *pos = *pos + 1;
            switch (c) {
            case 'f':  c = '\f';   /* ff */ break;
            case 'n':  c = '\n';   /* nl */ break;
            case 'r':  c = '\r';   /* cr */ break;
            case 't':  c = '\t';   /* ht */ break;
            case 'v':  c = '\013'; /* vt */ break;
            default:
                       assert(0 && "FIXME");
            }
        }
    }
    return c;
}

static uint8_t *pegvm_make_charset(ARRAY(uint8_t) *bdata)
{
    size_t pos = 0;
    int flip_bit = 0;
    // FIXME support unicode, hexcode
    uint8_t bits[32]; // 8 * 32 = 256
    uint8_t c = 0, start;
    uint8_t *set = malloc(32);
    if (ARRAY_size(*bdata) && ARRAY_get(uint8_t, bdata, 0) == '^') {
        memset(bits, 255, 32);
        flip_bit = 1;
        pos++;
    }
    else {
        memset(bits, 0, 32);
    }
    for (pos = 0; pos < ARRAY_size(*bdata); pos++) {
        if (ARRAY_get(uint8_t, bdata, pos) == '-' &&
                pos < ARRAY_size(*bdata) + 1) {
            uint8_t last;
            c = start;
            pos++;
            for (last = get_next_char(bdata, &pos); c < last; c++) {
                if (flip_bit) {
                    bits[c / 8] &= ~(1 << (c % 8));
                }
                else {
                    bits[c / 8] |= 1 << (c % 8);
                }
            }
        }
        else {
            c = get_next_char(bdata, &pos);
            if (flip_bit) {
                bits[c / 8] &= ~(1 << (c % 8));
            }
            else {
                bits[c / 8] |= 1 << (c % 8);
            }
            start = c;
        }
    }
    memcpy(set, bits, 32);
    return set;
}

static Instruction *Emit_EXIT(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->ndata = 0;
    return self;
}
static Instruction *Emit_JUMP(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->ndata = ndata;
    return self;
}
static Instruction *Emit_CALL(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->ndata = ndata;
    return self;
}
static Instruction *Emit_RET(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->ndata = 0;
    return self;
}
static Instruction *Emit_IFSUCC(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->ndata = ndata;
    return self;
}
static Instruction *Emit_IFFAIL(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->ndata = ndata;
    return self;
}
static Instruction *Emit_NOP(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->ndata = 0;
    return self;
}
static Instruction *Emit_Failure(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchText(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    if (ndata) {
        self->ndata = ndata;
    }
    else {
        assert(0 && "Not Implemented");
        self->bdata = pegvm_string_copy(bdata);
    }
    return self;
}
static Instruction *Emit_MatchByteChar(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}

static uint8_t get_next_char(ARRAY(uint8_t) *bdata, size_t *pos)
{
    uint8_t c = ARRAY_get(uint8_t, bdata, *pos);
    if (c) {
        if (c == '\\' &&
                *pos + 1< ARRAY_size(*bdata) &&
                (c = ARRAY_get(uint8_t, bdata, *pos + 1))) {
            // peg4d only support \f, \n, \r, \t, \v
            *pos = *pos + 1;
            switch (c) {
            case 'f':  c = '\f';   /* ff */ break;
            case 'n':  c = '\n';   /* nl */ break;
            case 'r':  c = '\r';   /* cr */ break;
            case 't':  c = '\t';   /* ht */ break;
            case 'v':  c = '\013'; /* vt */ break;
            default:
                       assert(0 && "FIXME");
            }
        }
    }
    return c;
}

static Instruction *Emit_MatchCharset(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    // FIXME support unicode, hexcode
    uint8_t bits[32]; // 8 * 32 = 256
    uint8_t c = 0, start;
    size_t pos = 0;
    int flip_bit = 0;
    if (ARRAY_size(*bdata) && ARRAY_get(uint8_t, bdata, 0) == '^') {
        memset(bits, 255, 32);
        flip_bit = 1;
        pos++;
    }
    else {
        memset(bits, 0, 32);
    }
    for (pos = 0; pos < ARRAY_size(*bdata); pos++) {
        if (ARRAY_get(uint8_t, bdata, pos) == '-' &&
                pos < ARRAY_size(*bdata) + 1) {
            uint8_t last;
            c = start;
            pos++;
            for (last = get_next_char(bdata, &pos); c < last; c++) {
                if (flip_bit) {
                    bits[c / 8] &= ~(1 << (c % 8));
                }
                else {
                    bits[c / 8] |= 1 << (c % 8);
                }
            }
        }
        else {
            c = get_next_char(bdata, &pos);
            if (flip_bit) {
                bits[c / 8] &= ~(1 << (c % 8));
            }
            else {
                bits[c / 8] |= 1 << (c % 8);
            }
            start = c;
        }
    }
    self->bdata = malloc(32);
    memcpy(self->bdata, bits, 32);
    return self;
}
static Instruction *Emit_MatchAnyChar(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->ndata = 0;
    return self;
}
static Instruction *Emit_MatchTextNot(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchByteCharNot(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchCharsetNot(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchOptionalText(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchOptionalByteChar(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_MatchOptionalCharset(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_RememberPosition(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->ndata = ndata;
    return self;
}
static Instruction *Emit_CommitPosition(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    return self;
}
static Instruction *Emit_BacktrackPosition(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    return self;
}
static Instruction *Emit_RememberSequencePosition(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->ndata = ndata;
    return self;
}
static Instruction *Emit_CommitSequencePosition(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    return self;
}
static Instruction *Emit_BackTrackSequencePosition(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    return self;
}
static Instruction *Emit_RememberFailurePosition(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_UpdateFailurePosition(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_ForgetFailurePosition(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    return self;
}
static Instruction *Emit_StoreObject(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    return self;
}
static Instruction *Emit_DropStoredObject(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    return self;
}
static Instruction *Emit_RestoreObject(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    return self;
}
static Instruction *Emit_RestoreObjectIfFailure(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_RestoreNegativeObject(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    return self;
}
static Instruction *Emit_ConnectObject(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->bdata = pegvm_string_copy(bdata);
    return self;
}
static Instruction *Emit_DisableTransCapture(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_EnableTransCapture(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_NewObject(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    return self;
}
static Instruction *Emit_LeftJoinObject(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_CommitObject(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_RefreshStoredObject(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_Tagging(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    self->bdata = pegvm_string_copy(bdata);
    return self;
}
static Instruction *Emit_Value(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}
static Instruction *Emit_Indent(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata)
{
    assert(0 && "Not Implemented");
    return self;
}

typedef Instruction *(*inst_load_t)(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata);
static inst_load_t f_inst[] = {
#define DEFINE_LOAD_F(OP) Emit_##OP,
    PEGVM_OP_EACH(DEFINE_LOAD_F)
#undef DEFINE_LOAD_F
};

static void PegVMInstruction_relocate(Instruction *code, ARRAY(Instruction) *insts)
{
    Instruction *x, *e;
    FOR_EACH_ARRAY(*insts, x, e) {
        if (PEGVM_OP_EXIT < x->opcode && x->opcode <= PEGVM_OP_IFFAIL) {
            uint32_t dst = x->ndata;
            //FIXME(imasahiro) bytecode generator emit wrong index
            x->dst = code + dst + 1;
        }
    }
    memcpy(code, ARRAY_list(*insts), sizeof(Instruction) * ARRAY_size(*insts));
}

static void PegVMInstruction_dump(PegVMInstruction *code, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++) {
        Instruction *inst = &code[i];
        int opcode = inst->opcode;
        uint32_t ndata = inst->ndata;
        uint8_t *bdata = inst->bdata;
        fprintf(stderr, "[%04d] op=%-28s", (int)i, get_opname(opcode));
        if (ndata) {
            fprintf(stderr, " ndata=%08d", ndata);
        }
        if (bdata) {
            fprintf(stderr, " bdata=%s", (char *)bdata);
        }
        fprintf(stderr, "bdata=\n");
    }
}

PegVMInstruction *ByteCodeLoader_Load(InputSource *input)
{
    size_t i;
    int idx = 0;
    Instruction inst, *instp;
    ARRAY(uint8_t) buf;
    ARRAY(Instruction) insts;
    ARRAY_init(uint8_t, &buf, 1);
    ARRAY_init(Instruction, &insts, 1);

    PegVMInstruction *code = NULL;
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
        inst.opcode = opcode;
        inst.ndata = 0;
        inst.bdata = NULL;
        instp = f_inst[opcode](&inst, ndata, &buf);
        ARRAY_add(Instruction, &insts, instp);
        idx++;
    }
    code = (PegVMInstruction *) malloc(sizeof(Instruction) * ARRAY_size(insts));
    PegVMInstruction_relocate(code, &insts);
    PegVMInstruction_dump(code, ARRAY_size(insts));
    code += 1; // Skip first EXIT opcode
    ARRAY_dispose(uint8_t, &buf);
    ARRAY_dispose(Instruction, &insts);
    return code;
}
