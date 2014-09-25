//
//  main.c
//  pegVM


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "opcode.h"

int loadByteCodeFile(char *fileName);

int main(int argc, const char * argv[])
{
    loadByteCodeFile(argv[0]);
    return 0;
}

int loadByteCodeFile(char *fileName) {
    FILE *file;
    unsigned char buf[8192];
    Opcode opList[1024];
    int i, size, pos = 0, code_size = 0, ndata = 0;
    char *bdata;

    
    file = fopen(fileName, "rb");
    if(file == NULL) {
        printf("error: file not found (%s)\n", fileName);
        return -1;
    }
    
    size = fread(buf, sizeof(unsigned char), 8192, file);
    
    for (i = 0; i < size; i++) {
        if(pos > 4 && (int)buf[i] == 0) {
            opList[code_size].bdata = bdata;
            printf("%s", opList[code_size].bdata);
            printf("\n");
            code_size++;
            if((int)buf[i+1] == 128) {
                break;
            }
            pos = 0;
        }
        else if(pos == 0){
            opList[code_size].opcode = (int)buf[i];
            printf("%d ", opList[code_size].opcode);
            pos++;
        }
        else if(pos > 0 && pos < 4) {
            ndata += (int)buf[i] << ((pos - 1)*8);
            pos++;
        }
        else if(pos == 4) {
            ndata += (int)buf[i] << ((pos - 1)*8);
            opList[code_size].ndata = ndata;
            printf("%ld ", opList[code_size].ndata);
            pos++;
        }
        else {
            bdata[pos-5] = buf[i];
            pos++;
        }
    }
    
    printf("%d\n", code_size);
    
    fclose(file);
    return 0;

}

