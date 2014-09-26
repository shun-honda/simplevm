//
//  simplevm_parsingcontext.c
//  pegVM
//

#include <stdio.h>
#include <string.h>
#include "simplevm_parsingcontext.h"
#include "opcode.h"
#include "parsing_source.h"

ParsingObject createParsingObject(SimpleVmParsingContext *c) {
    ParsingObject left;
    left.source = c->source;
    left.TagName = "empty";
    left.failureflag = 0;
    return left;
}

void consume_text(SimpleVmParsingContext *c, int length) {
    c->pos += length;
}
int match(char c, char* charset) {
    int8_t s, e;
    s = (int8_t) charset[0];
    return 0;
}
void lpush(SimpleVmParsingContext *c, long v) {
    c->lstack[c->lstacktop] = v;
}

void lpop(SimpleVmParsingContext *c) {
    c->lstacktop--;
}

int lpop2i(SimpleVmParsingContext *c) {
    c->lstacktop--;
    return (int)(c->lstack[c->lstacktop]);
}

void opush(SimpleVmParsingContext *c, ParsingObject left) {
    c->ostack[c->ostacktop] = left;
    c->ostacktop++;
}

ParsingObject opop(SimpleVmParsingContext *c) {
    c->ostacktop--;
    return c->ostack[c->ostacktop];
}

void rollback(SimpleVmParsingContext *c, long pos) {
    c->pos = pos;
}

int isFailure(SimpleVmParsingContext *c) {
    if (c->left.failureflag == 1) {
        return 1;
    }
    return 0;
}
void vm_opMatchText(SimpleVmParsingContext *c, char *bdata) {
    if (strcmp(bdata,get_text(c->source, strlen(bdata)))) {
        consume_text(c, strlen(bdata));
    }
}
void vm_opMatchCharset(SimpleVmParsingContext *c, char *bdata) {
    
}
void vm_opMatchAnyChar(SimpleVmParsingContext *c);
void vm_opRememberPosition(SimpleVmParsingContext *c) {
    lpush(c, c->pos);
}
void vm_opCommitPosition(SimpleVmParsingContext *c) {
    lpop(c);
}
void vm_opBacktrackPosition(SimpleVmParsingContext *c) {
    lpop(c);
    rollback(c, c->lstack[c->lstacktop]);
}
void vm_opFailure(SimpleVmParsingContext *c) {
    c->fpos = c->pos;
}
void vm_opRememberFailurePosition(SimpleVmParsingContext *c) {
    lpush(c, c->fpos);
}
void vm_opUpdateFailurePosition(SimpleVmParsingContext *c) {
    lpop(c);
}
void vm_opForgetFailurePosition(SimpleVmParsingContext *c) {
    lpop(c);
    c->fpos = c->lstack[c->lstacktop];
}
void vm_opRememberSequencePosition(SimpleVmParsingContext *c) {
    lpush(c, c->pos);
    lpush(c, c->pos);
    opush(c, c->left);
}
void vm_opCommitSequencePosition(SimpleVmParsingContext *c) {
    opop(c);
    lpop(c);
    lpop(c);
    rollback(c, c->lstack[c->lstacktop]);
}
void vm_opBackTrackSequencePosition(SimpleVmParsingContext *c) {
    c->left = opop(c);
    lpop(c);
    lpop(c);
}
void vm_opStoreObject(SimpleVmParsingContext *c) {
    opush(c, c->left);
}
void vm_opDropStoredObject(SimpleVmParsingContext *c) {
    opop(c);
}
void vm_opRestoreObject(SimpleVmParsingContext *c) {
    c->left = opop(c);
}
void vm_opRestoreNegativeObject(SimpleVmParsingContext *c) {
    if(isFailure(c)) {
        c->left = opop(c);
    }
    else {
        opop(c);
        vm_opFailure(c);
    }
}
void vm_opConnectObject(SimpleVmParsingContext *c, long ndata) {
    
}
void vm_opNewObject(SimpleVmParsingContext *c) {
    c->left = createParsingObject(c);
}
void vm_opCommitObject(SimpleVmParsingContext *c) {
    opop(c);
}
void vm_opTagging(SimpleVmParsingContext *c, Opcode *op) {
    
}
void vm_opIndent(SimpleVmParsingContext *c) {
    
}

