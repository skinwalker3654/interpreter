#include "../header/lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

Lexer *lexer_create(char *source) {
    Lexer *new_lx = malloc(sizeof(Lexer));
    if(!new_lx) {
        printf("Failed to create the lexer\n");
        return NULL;
    }

    new_lx->source = strdup(source);
    if(!new_lx->source) {
        printf("Failed to put the source into the lexer\n");
        free(new_lx);
        return NULL;
    }

    new_lx->pos = 0;
    new_lx->line = 1;

    return new_lx;
}

void lexer_destroy(Lexer *lx) {
    if(!lx) return;

    free(lx->source);
    free(lx);
    lx = NULL;
}

char lexer_peek(Lexer *lx) {
    if(!lx) return -1;
    return lx->source[lx->pos];
}

char lexer_advance(Lexer *lx) {
    if(!lx) return -1;

    char current_ch = lx->source[lx->pos++];
    if(current_ch == '\n') 
        lx->line++; 

    return current_ch;
}

void lexer_skip_ws(Lexer *lx) {
    if(!lx) return;

    while(isspace(lexer_peek(lx))) {
        lexer_advance(lx);
    }
}
