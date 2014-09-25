//
//  parsing_sorce.h
//  pegVM
//

#ifndef pegVM_parsing_sorce_h
#define pegVM_parsing_sorce_h

typedef struct {
    char* name;
    char* sourceText;
    long pos;
}ParsingSource;

char* get_text(ParsingSource source, int length);

#endif
