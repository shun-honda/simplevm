//
//  input_source.c
//  pegVM
//

#include <stdio.h>
#include <gc/gc.h>
#include <string.h>
#include "input_source.h"

static char *loadFile(const char *filename, size_t *length)
{
    size_t len = 0;
    FILE *fp = fopen(filename, "rb");
    char *source;
    if (!fp) {
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    len = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    source = (char *)malloc(len + 1);
    if (len != fread(source, 1, len, fp)) {
        fprintf(stderr, "fread error\n");
        exit(EXIT_FAILURE);
    }
    source[len] = '\0';
    fclose(fp);
    *length = len;
    return source;
}

InputSource *InputSource_Init(InputSource *is, const char *filename)
{
    is->filename = filename;
    is->pos = is->length = 0;
    is->source = loadFile(filename, &is->length);
    if (!is->source) {
        return NULL;
    }
    return is;
}

void InputSource_Dispose(InputSource *is)
{
    if (is->source) {
        free((char *)is->source);
        is->pos = is->length = 0;
        is->source = NULL;
    }
}

uint8_t *InputSource_CopyText(InputSource *input, size_t pos, size_t length)
{
    uint8_t *text = (uint8_t *)GC_MALLOC(length + 1);
    memcpy(text, input->source + pos, length);
    text[length] = '\0';
    return text;
}
