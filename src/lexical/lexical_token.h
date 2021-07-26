//
// Created by adrianoii on 23/07/2021.
//

#ifndef GCC_LEXICAL_TOKEN_H
#define GCC_LEXICAL_TOKEN_H

#include <stddef.h>
#include "../string/string.h"

typedef enum
{
    INVALID_TOKEN_CLASS,
    BRACKET_COMMENT, // {}
    SLASH_COMMENT, // /**/
    IDENTIFIER, // i | abc |a2bc3
    INTEGER, // 1 | 123 | 11564869
    REAL, // 1.1 | 1.43333
    KEYWORD, // if | then | while | do | write | read | else | begin | end | program | real | integer | procedure | var
    SYMBOL, // ( | ) | * | / | + | - | <> | >= | <= | > | < | $ | : | , | ; | = | == | := | .
    SEPARATORS, // ' ' | \t | \n
} lexical_token_class_t;

const char *lexical_token_class_to_string(lexical_token_class_t class);

typedef struct Token
{
    lexical_token_class_t token_class;
    size_t line;
    size_t start_position;
    size_t end_position;
    string_t *value; // Optional
} token_t;

token_t *token_init();

void token_reset(token_t *token);

void token_destroy(token_t *token);

void token_log(token_t *token);

void token_pretty_log(token_t *token);

#endif //GCC_LEXICAL_TOKEN_H
