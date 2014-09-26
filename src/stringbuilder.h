//
//  stringbuilder.h
//

#ifndef _stringbuilder_h
#define _stringbuilder_h

typedef struct {
	char *str;
	int len;
	int extent;
} StringBuilder;

void sb_create(StringBuilder *sb);
void sb_delete(StringBuilder *sb);
void sb_append(StringBuilder *sb, const char *str);

#endif
