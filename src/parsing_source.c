//
//  parsing_source.c
//  pegVM
//

#include <stdio.h>
#include "simplevm_parsingcontext.h"
#include "parsing_source.h"

char* get_text(ParsingSource source, int length) {
    char text[length];
    for (int i = 0; i < length; i++) {
        text[i] = source.sourceText[source.pos + i];
    }
    return text;
}