#include "loader.h"
#include "pegvm.h"
#define KARRAY_MALLOC(N)        GC_MALLOC(N)
#define KARRAY_REALLOC(PTR, N)  GC_REALLOC(PTR, N)
#define KARRAY_FREE(PTR)        ((void)PTR)
#include "karray.h"
#include <assert.h>

DEF_ARRAY_STRUCT(uint8_t);
DEF_ARRAY_T(uint8_t);
DEF_ARRAY_OP_NOPOINTER(uint8_t);
DEF_ARRAY_STRUCT(Instruction);
DEF_ARRAY_T_OP(Instruction);

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

typedef struct ByteCodeInfo {
    uint32_t version;
    uint32_t pathlen;
    uint8_t filepath[MAX_PATH];
    int64_t bytecode_length;
} ByteCodeInfo;

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
    uint8_t *bits = malloc(32); // 8 * 32 = 256
    uint8_t c = 0, start;
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
    return bits;
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

static void ByteCodeInfo_dump(ByteCodeInfo *info)
{
    fprintf(stderr, "ByteCodeVersion=%u\n", info->version);
    fprintf(stderr, "PEGFile=%s\n", info->filepath);
    fprintf(stderr, "LengthOfByteCode=%llu\n", info->bytecode_length);
    fprintf(stderr, "\n");
}

static uint32_t read32(InputSource *input)
{
    uint32_t value = 0;
    value = InputSource_GetUint8(input);
    value = (value) | (InputSource_GetUint8(input) << 8);
    value = (value) | (InputSource_GetUint8(input) << 16);
    value = (value) | (InputSource_GetUint8(input) << 24);
    return value;
}

static uint64_t read64(InputSource *input)
{
    uint64_t value1 = read32(input);
    uint64_t value2 = read32(input);
    return value2 << 32 | value1;
}

PegVMInstruction *ByteCodeLoader_Load(InputSource *input)
{
    size_t i;
    Instruction inst, *instp;
    ByteCodeInfo info = {};
    PegVMInstruction *code = NULL;
    ARRAY(uint8_t) buf;
    ARRAY(Instruction) insts;

    ARRAY_init(uint8_t, &buf, 1);
    ARRAY_init(Instruction, &insts, 1);

    info.version = read32(input);
    info.pathlen  = read32(input);
    for (i = 0; i < info.pathlen; i++) {
        info.filepath[i] = InputSource_GetUint8(input);
    }
    info.bytecode_length = read64(input);

    for (i = 0; i < info.bytecode_length; i++) {
        uint8_t code_length;
        uint8_t opcode = InputSource_GetUint8(input);
        uint8_t bdata = 0;
        uint32_t ndata;
        ndata = read32(input);
        ARRAY_clear(buf);
        code_length = InputSource_GetUint8(input);
        while (code_length--) {
            bdata = InputSource_GetUint8(input);
            ARRAY_add(uint8_t, &buf, bdata);
        }
        inst.opcode = opcode;
        inst.ndata = 0;
        inst.bdata = NULL;
        instp = f_inst[opcode](&inst, ndata, &buf);
        ARRAY_add(Instruction, &insts, instp);
    }
    code = (PegVMInstruction *) GC_MALLOC(sizeof(Instruction) * ARRAY_size(insts));
    PegVMInstruction_relocate(code, &insts);
    ByteCodeInfo_dump(&info);
    if (0) {
        PegVMInstruction_dump(code + 1, ARRAY_size(insts));
    }
    ARRAY_dispose(uint8_t, &buf);
    ARRAY_dispose(Instruction, &insts);
    return code;
}
