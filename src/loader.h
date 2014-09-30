//
//  loader.h
//

// #include "pegvm_inst.h"
#include "input_source.h"
#ifndef LOADER_H
#define LOADER_H

typedef struct Instruction {
    long opcode;
    union {
        unsigned long ndata;
        uint8_t *bdata;
        struct Instruction *dst;
    };
} PegVMInstruction, Instruction;

typedef struct ByteCodeInfo {
    long version;
    uint8_t *PegFileName;
    int64_t bytecode_length;
}ByteCodeInfo;

int pegvm_unconsume_charset(uint8_t *bits, uint8_t c);
PegVMInstruction *ByteCodeLoader_Load(InputSource *input);
void PegVMInstruction_dump(PegVMInstruction *code, size_t len);
void ByteCodeInfo_dump(ByteCodeInfo *info);
#endif
