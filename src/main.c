//
//  main.c
//  pegVM

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <gc/gc.h>
#include <stdlib.h>
#include "pegvm.h"

static void pegvm_usage(const char *file)
{
    fprintf(stderr, "Usage: %s -f peg_bytecode target_file\n", file);
    exit(EXIT_FAILURE);
}

static void pegvm_error(const char *errmsg)
{
    fprintf(stderr, "%s\n", errmsg);
    exit(EXIT_FAILURE);
}

int main(int argc, char * const argv[])
{
    ParserContext context;
    const char *syntax_file = NULL;
    const char *orig_argv0 = argv[0];
    int opt;
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
        case 'f':
            syntax_file = optarg;
            break;
        default: /* '?' */
            pegvm_usage(orig_argv0);
        }
    }
    if (syntax_file == NULL) {
        pegvm_usage(orig_argv0);
    }
    argc -= optind;
    argv += optind;
    if (argc == 0) {
        pegvm_usage(orig_argv0);
    }
    ParserContext_Init(&context);
    if (!ParserContext_LoadSyntax(&context, syntax_file)) {
        pegvm_error("invalid bytecode");
    }
    if (!ParserContext_ParseFiles(&context, argc, argv)) {
        pegvm_error(context.last_error);
    }
    ParserContext_Destruct(&context);
    return 0;
}
