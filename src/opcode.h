//
//  opcode.h
//

#ifndef _opcode_h
#define _opcode_h

enum vm_opcode {
    EXIT = 0,
    JUMP,
    CALL,
    RET,
    IFSUCC,
    IFFAIL,
    opMatchText,
    opMatchCharset,
    opMatchAnyChar,
    opRememberPosition,
    opCommitPosition,
    opBacktrackPosition,
    opRememberFailurePosition,
    opUpdateFailurePosition,
    opForgetFailurePosition,
    opRememberSequencePosition,
    opCommitSequencePosition,
    opBackTrackSequencePosition,
    opStoreObject,
    opDropStoredObject,
    opRestoreObject,
    opRestoreNegativeObject,
    opConnectObject,
    opNewObject,
    opCommitObject,
    opTagging,
    opIndent,
    ERROR = -1
};
typedef struct {
    int opcode;
    union {
        int ndata;
        char *bdata;
    };
} Opcode;

#endif
