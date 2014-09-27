//
//  input_source.h
//  pegVM
//
#include <stdio.h>
#ifndef INPUT_SOURCE_H
#define INPUT_SOURCE_H

typedef struct InputSource {
  const char *source;
  size_t pos;
  size_t length;
  const char *filename;
} InputSource;

InputSource *InputSource_Init(InputSource *is, const char *filename);
void InputSource_Dispose(InputSource *is);

#endif /* end of include guard */
