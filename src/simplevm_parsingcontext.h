//
//  simplevm_parsingcontext.h
//  pegVM
//

#include "parsing_source.h"
#include "opcode.h"

#ifndef pegVM_simplevm_parsingcontext_h
#define pegVM_simplevm_parsingcontext_h

typedef Object;

typedef struct {
    ParsingSource source;
    long pospeg;
    int length;
    char* TagName;
    Object parent;
    Object *AST;
    int failureflag;
}ParsingObject;

typedef struct {
    ParsingObject left;
    ParsingSource source;
    long lstack[4096*8];
    int lstacktop;
    ParsingObject ostack[4096];
    int ostacktop;
    long pos;
    long fpos;
}SimpleVmParsingContext;

ParsingObject createParsingObject(SimpleVmParsingContext *c);
void lpush(SimpleVmParsingContext *c, long v);
void lpop(SimpleVmParsingContext *c);
int lpop2i(SimpleVmParsingContext *c);
int isFailure(SimpleVmParsingContext *c);
void consume_text(SimpleVmParsingContext *c, int length);
void consume_charset(SimpleVmParsingContext *c);
void vm_opMatchText(SimpleVmParsingContext *c, char *bdata);
void vm_opMatchCharset(SimpleVmParsingContext *c, char *bdata);
void vm_opMatchAnyChar(SimpleVmParsingContext *c);
void vm_opRememberPosition(SimpleVmParsingContext *c);
void vm_opCommitPosition(SimpleVmParsingContext *c);
void vm_opBacktrackPosition(SimpleVmParsingContext *c);
void vm_opFailure(SimpleVmParsingContext *c);
void vm_opRememberFailurePosition(SimpleVmParsingContext *c);
void vm_opUpdateFailurePosition(SimpleVmParsingContext *c);
void vm_opForgetFailurePosition(SimpleVmParsingContext *c);
void vm_opRememberSequencePosition(SimpleVmParsingContext *c);
void vm_opCommitSequencePosition(SimpleVmParsingContext *c);
void vm_opBackTrackSequencePosition(SimpleVmParsingContext *c);
void vm_opStoreObject(SimpleVmParsingContext *c);
void vm_opDropStoredObject(SimpleVmParsingContext *c);
void vm_opRestoreObject(SimpleVmParsingContext *c);
void vm_opRestoreNegativeObject(SimpleVmParsingContext *c);
void vm_opConnectObject(SimpleVmParsingContext *c, long ndata);
void vm_opNewObject(SimpleVmParsingContext *c);
void vm_opCommitObject(SimpleVmParsingContext *c);
void vm_opTagging(SimpleVmParsingContext *c, Opcode *op);
void vm_opIndent(SimpleVmParsingContext *c);

#endif
