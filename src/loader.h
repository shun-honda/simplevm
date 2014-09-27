//
//  loader.h
//

// #include "pegvm_inst.h"
#include "input_source.h"
typedef void PegVMInstruction;
#ifndef LOADER_H
#define LOADER_H
PegVMInstruction *ByteCodeLoader_Load(InputSource *input);
#endif
