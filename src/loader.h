//
//  loader.h
//

// #include "pegvm_inst.h"
#include "input_source.h"
#ifndef LOADER_H
#define LOADER_H
typedef void PegVMInstruction;
PegVMInstruction *ByteCodeLoader_Load(InputSource *input);
#endif
