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

PegVMInstruction *ByteCodeLoader_Load(InputSource *input);
void PegVMInstruction_dump(PegVMInstruction *code, size_t len);
#endif
