//
//  loader.h
//

#ifndef _loader_h
#define _loader_h

#include "parsing_source.h"
#include "opcode.h"
#include "stringbuilder.h"
#include "parsing_sorce.h"

char* loadByteCodeFile(char *fileName);
ParsingSource* loadSource(char *fileName);

#endif
