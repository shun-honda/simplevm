//
//  vm.c
//  pegVM
//

#include "pegvm.h"
#include "input_source.h"
#include <string.h>
#include <stdarg.h>

static void ParserContext_SetError(ParserContext *context, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));

void ParserContext_Init(ParserContext *context)
{
    memset(context, 0, sizeof(*context));
}

void ParserContext_Destruct(ParserContext *context)
{
}

static void ParserContext_SetError(ParserContext *context, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(context->last_error, PARSER_CONTEXT_MAX_ERROR_LENGTH, fmt, ap);
    va_end(ap);
}

int ParserContext_LoadSyntax(ParserContext *context, const char *file)
{
    InputSource is;
    if (InputSource_Init(&is, file) == NULL) {
        return 1;
    }
    InputSource_Dispose(&is);
    return 0;
}

int ParserContext_ParseFiles(ParserContext *context, int argc, char *const *argv)
{
    int i;
    InputSource is;
    for (i = 0; i < argc; i++) {
        if (InputSource_Init(&is, argv[i]) == NULL) {
            ParserContext_SetError(context,
                    "parse error: cannot load input file(%s)", argv[i]);
            return 1;
        }
        if (ParserContext_Execute(context, &is)) {
            ParserContext_SetError(context,
                    "parse error: input file(%s)", argv[i]);
            InputSource_Dispose(&is);
            return 1;
        }
        InputSource_Dispose(&is);
    }
    return 0;
}

int ParserContext_Execute(ParserContext *context, InputSource *input)
{
    return 0;
}

// static void run(SimpleVmParsingContext *c, int pc, Opcode** code) {
//     bool exit = false;
//     while(exit == false) {
//         Opcode *op = code[pc];
//         switch(op->opcode) {
//             case 0:
//                 exit = true;
//                 break;
//             case 1:
//                 pc = op->ndata;
//                 break;
//             case 2:
//                 lpush(c, pc);
//                 pc = op->ndata;
//                 break;
//             case 3:
//                 pc = lpop2i(c);
//                 break;
//             case 4:
//                 if(!isFailure(c)) {
//                     pc = op->ndata;
//                 }
//                 break;
//             case 5:
//                 if(isFailure(c)) {
//                     pc = op->ndata;
//                 }
//                 break;
//             case 7:
//                 vm_opMatchText(c, op->bdata);
//                 break;
//             case 9:
//                 vm_opMatchCharset(c, op->bdata);
//                 break;
//             case 10:
//                 vm_opMatchAnyChar(c);
//                 break;
//             case 17:
//                 vm_opRememberPosition(c);
//                 break;
//             case 18:
//                 vm_opCommitPosition(c);
//                 break;
//             case 19:
//                 vm_opBacktrackPosition(c);
//                 break;
//             case 20:
//                 vm_opRememberSequencePosition(c);
//                 break;
//             case 21:
//                 vm_opCommitSequencePosition(c);
//                 break;
//             case 22:
//                 vm_opBackTrackSequencePosition(c);
//                 break;
//             case 23:
//                 vm_opRememberFailurePosition(c);
//                 break;
//             case 24:
//                 vm_opUpdateFailurePosition(c);
//                 break;
//             case 25:
//                 vm_opForgetFailurePosition(c);
//                 break;
//             case 26:
//                 vm_opStoreObject(c);
//                 break;
//             case 27:
//                 vm_opDropStoredObject(c);
//                 break;
//             case 28:
//                 vm_opRestoreObject(c);
//                 break;
//             case 29:
//                 vm_opRestoreNegativeObject(c);
//                 break;
//             case 30:
//                 vm_opConnectObject(c, op->ndata);
//                 break;
//             case 33:
//                 vm_opNewObject(c);
//                 break;
//             case 35:
//                 vm_opCommitObject(c);
//                 break;
//             case 37:
//                 vm_opTagging(c, op);
//                 break;
//             case 39:
//                 vm_opIndent(c);
//                 break;
//             default:
//                 fprintf(stderr, "ERROR, unreachable\n");
//                 break;
//         }
//         pc = pc + 1;
//     }
// }
