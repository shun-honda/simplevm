//
//  stringbuilder.c
//

#include <stdio.h>
#include <stdlib.h>
#include "stringbuilder.h"

void sb_create(StringBuilder *sb) {
	sb->len = 0;
	sb->extent = 16;
	sb->str = (char *)malloc(sb->extent);
	sb->str[0] = '\0';
}

void sb_delete(StringBuilder *sb) {
	free(sb->str);
}

void sb_append(StringBuilder *sb, const char *str) {
	int newLen;
	int newExtent;
    
	newLen = sb->len + strlen(str);
	newExtent = sb->extent;
	while (newExtent <= newLen) {
		newExtent <<= 1;
	}
	if (sb->extent < newExtent) {
		sb->extent = newExtent;
		sb->str = (char *)realloc(sb->str, sb->extent);
		//fprintf(stderr, "newExtent=[%d]\n", newExtent);
	}
	strcpy(sb->str + sb->len, str);
	sb->len = newLen;
}