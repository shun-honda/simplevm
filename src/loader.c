#include "loader.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

PegVMInstruction *ByteCodeLoader_Load(InputSource *input)
{
    return NULL;
}

//int loadByteCodeFile(char *fileName) {
//    FILE *file;
//    unsigned char buf[8192];
//    Opcode opList[1024];
//    int i, size, pos = 0, code_size = 0, ndata = 0;
//    char bdata[256];
//
//    for (int j = 0; j < 256; j++) {
//        bdata[j] = NULL;
//    }
//
//    file = fopen(fileName, "rb");
//    if(file == NULL) {
//        printf("error: file not found (%s)\n", fileName);
//        return -1;
//    }
//
//    size = fread(buf, sizeof(unsigned char), 8192, file);
//
//    for (i = 0; i < size; i++) {
//        if(pos > 4 && (int)buf[i] == 0) {
//            opList[code_size].bdata = bdata;
//            if (opList[code_size].opcode == 9) {
//                printf("[%s]", opList[code_size].bdata);
//            }
//            else if(opList[code_size].opcode == 7) {
//                printf("\'%s\'", opList[code_size].bdata);
//            }
//            else if(opList[code_size].bdata != NULL) {
//                printf("%s", opList[code_size].bdata);
//            }
//            printf("\n");
//            code_size++;
//            if((int)buf[i+1] == 128) {
//                break;
//            }
//            for (int j = 5; j < pos; j++) {
//                bdata[j-5] = NULL;
//            }
//            ndata = 0;
//            pos = 0;
//        }
//        else if(pos == 0){
//            opList[code_size].opcode = (int)buf[i];
//            printf("%d ", opList[code_size].opcode);
//            pos++;
//        }
//        else if(pos > 0 && pos < 4) {
//            ndata += (int)buf[i] << ((pos - 1)*8);
//            pos++;
//        }
//        else if(pos == 4) {
//            ndata += (int)buf[i] << ((pos - 1)*8);
//            opList[code_size].ndata = ndata;
//            printf("%ld ", opList[code_size].ndata);
//            pos++;
//        }
//        else {
//            bdata[pos-5] = buf[i];
//            pos++;
//        }
//    }
//
//    printf("%d\n", code_size);
//
//    fclose(file);
//    return 0;
//
//}

//ParsingSource* loadSource(char* fileName) {
//    ParsingSource *source = malloc(sizeof(ParsingSource));
//    StringBuilder *sb = malloc(sizeof(StringBuilder));
//    sb_create(sb);
//    FILE *fp;
//    char *filename = "json_test.json";
//    char *readline = malloc(sizeof(char)*256);
//
//    if ((fp = fopen(filename, "r")) == NULL) {
//        printf("error: file not found (%s)\n", fileName);
//        exit(0);
//    }
//
//    while ( fgets(readline, 256, fp) != NULL ) {
//        sb_append(sb, readline);
//    }
//    source->sourceText = sb->str;
//    printf("%s\n", sb->str);
//    return source;
//}
