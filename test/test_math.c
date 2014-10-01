#include "pegvm.h"
#include "test_util.h"
#include <string.h>

static int NODE_EqualTag(NODE *node, const char *str)
{
    char *tag = (char *)NODE_GetTag(node);
    return tag && strncmp(tag, str, strlen(str)) == 0;
}


static int NODE_EqualText(NODE *node, const char *str)
{
    char *text = (char *)NODE_GetText(node);
    return text && strncmp(text, str, strlen(str)) == 0;
}

// usage: ARGV[0] /path/to/math.bin /path/to/math.txt
int main(int argc, char const *argv[])
{
    ParserContext context;
    NODE *node;
    NODE **children;
    NODE **children1;
    NODE **children2;
    unsigned len;
    unsigned len1;
    unsigned len2;

    assert(argc == 3 &&
            "wrong usage. usage: ARGV[0] /path/to/math.bin /path/to/math.txt");
    GC_INIT();
    ParserContext_Init(&context);

    assert(ParserContext_LoadSyntax(&context, argv[1]) == 0);
    assert(ParserContext_ParseFiles(&context, 1, (char **)&argv[2]) == 0);
    assert(context.current_node && "parse failed");

    // 1+2-3*4
    // =>
    // (- (+ 1 2) (* 3 4))
    node = context.current_node;
    children = Node_GetChildren(node, &len);
    assert(children && len == 2);
    children1 = Node_GetChildren(children[0], &len1);
    children2 = Node_GetChildren(children[1], &len2);
    assert(children1 && len1 == 2);
    assert(children2 && len2 == 2);
    assert(NODE_EqualTag(children[0], "Add"));
    assert(NODE_EqualTag(children[1], "Mul"));
    assert(NODE_EqualText(children1[0], "1"));
    assert(NODE_EqualText(children1[1], "2"));
    assert(NODE_EqualText(children2[0], "3"));
    assert(NODE_EqualText(children2[1], "4"));
    ParserContext_Dispose(&context);
    return 0;
}
