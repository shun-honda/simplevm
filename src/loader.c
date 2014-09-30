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
            for (last = get_next_char(bdata, &pos); c <= last; c++) {
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

static char *pegvm_dump_charset(uint8_t *bits)
{
    static char charset[256];
    size_t i, pos = 0;
    memset(charset, 0, 256);
    for (i = 0; i < 256; i++) {
        if ((bits[i / 8] & (1 << (i % 8))) == (1 << (i % 8))) {
            charset[pos++] = (char) i;
        }
    }
    return charset;
}

int pegvm_unconsume_charset(uint8_t *bits, uint8_t c)
{
    size_t i;
    for (i = 0; i < 256; i++) {
        if ((bits[i / 8] & (1 << (i % 8))) == (1 << (i % 8))) {
            if (i == c) {
                return 0;
            }
        }
    }
    return 1;
}

#include "loader.generated.c"

typedef Instruction *(*inst_load_t)(Instruction *self, uint32_t ndata, ARRAY(uint8_t) *bdata);
static inst_load_t f_inst[] = {
#define DEFINE_LOAD_F(OP) Emit_##OP,
    PEGVM_OP_EACH(DEFINE_LOAD_F)
#undef DEFINE_LOAD_F
};

typedef void (*inst_dump_t)(Instruction *self);
static inst_dump_t dump_inst[] = {
#define DEFINE_DUMP_F(OP) Dump_##OP,
    PEGVM_OP_EACH(DEFINE_DUMP_F)
#undef DEFINE_DUMP_F
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

void PegVMInstruction_dump(PegVMInstruction *code, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++) {
        Instruction *inst = &code[i];
        dump_inst[inst->opcode](inst);
    }
}

void ByteCodeInfo_dump(ByteCodeInfo *info)
{
    fprintf(stderr, "ByteCodeVersion=%ld\n", info->version);
    fprintf(stderr, "PEGFile=%s\n", info->PegFileName);
    fprintf(stderr, "LengthOfByteCode=%lld\n", info->bytecode_length);
    fprintf(stderr, "\n");
}

PegVMInstruction *ByteCodeLoader_Load(InputSource *input)
{
    size_t i;
    int idx = 0;
    Instruction inst, *instp;
    ByteCodeInfo *info = malloc(sizeof(ByteCodeInfo));
    ARRAY(uint8_t) buf;
    ARRAY(Instruction) insts = {};
    ARRAY_init(uint8_t, &buf, 1);
    ARRAY_init(Instruction, &insts, 1);

    PegVMInstruction *code = NULL;
    
    uint8_t version;
    version = InputSource_GetUint8(input);
    version = (version) | (InputSource_GetUint8(input) << 8);
    version = (version) | (InputSource_GetUint8(input) << 16);
    version = (version) | (InputSource_GetUint8(input) << 24);
    uint32_t length;
    length = InputSource_GetUint8(input);
    length = (length) | (InputSource_GetUint8(input) << 8);
    length = (length) | (InputSource_GetUint8(input) << 16);
    length = (length) | (InputSource_GetUint8(input) << 24);
    uint8_t *filename = malloc(sizeof(uint8_t) * length);
    for (i = 0; i < length; i++) {
        filename[i] = InputSource_GetUint8(input);
    }
    uint64_t bytecode_length;
    bytecode_length = InputSource_GetUint8(input);
    bytecode_length = (bytecode_length) | (InputSource_GetUint8(input) << 8);
    bytecode_length = (bytecode_length) | (InputSource_GetUint8(input) << 16);
    bytecode_length = (bytecode_length) | (InputSource_GetUint8(input) << 24);
    uint64_t in;
    for (i = 1; i < 5; i++) {
        in = InputSource_GetUint8(input);
        bytecode_length = (bytecode_length) | in << (8 * i);
    }
    info->version = version;
    info->PegFileName = filename;
    info->bytecode_length = bytecode_length;
    
    for (i = 0; i < bytecode_length; i++) {
        uint8_t opcode = InputSource_GetUint8(input);
        uint8_t bdata = 0;
        uint32_t ndata;
        ndata = InputSource_GetUint8(input);
        ndata = (ndata) | (InputSource_GetUint8(input) << 8);
        ndata = (ndata) | (InputSource_GetUint8(input) << 16);
        ndata = (ndata) | (InputSource_GetUint8(input) << 24);
        ARRAY_clear(buf);
        int j = 0;
        uint8_t code_length = InputSource_GetUint8(input);
        while (j < code_length) {
            bdata = InputSource_GetUint8(input);
            ARRAY_add(uint8_t, &buf, bdata);
            j++;
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
    ByteCodeInfo_dump(info);
    // PegVMInstruction_dump(code + 1, ARRAY_size(insts));
    ARRAY_dispose(uint8_t, &buf);
    ARRAY_dispose(Instruction, &insts);
    return code;
}
