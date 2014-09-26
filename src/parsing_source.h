//
//  parsing_source.h
//  pegVM
//

#ifndef pegVM_parsing_source_h
#define pegVM_parsing_source_h

typedef struct {
    char* name;
    char* sourceText;
    long pos;
}ParsingSource;

char* get_text(ParsingSource source, int length);

#endif
