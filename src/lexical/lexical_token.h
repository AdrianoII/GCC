//
// Created by adrianoii on 23/07/2021.
//

#ifndef GCC_LEXICAL_TOKEN_H
#define GCC_LEXICAL_TOKEN_H

#include <stddef.h>
#include "../string/string.h"
#include "../file_handler/file_handler.h"

typedef enum
{
    EMPTY_TOKEN_CLASS,
    INVALID_TOKEN_CLASS,
    BRACKET_COMMENT, // {}
    SLASH_COMMENT, // /**/
    IDENTIFIER, // i | abc |a2bc3
    INTEGER, // 1 | 123 | 11564869
    REAL, // 1.1 | 1.43333
    KEYWORD, // if | then | while | do | write | read | else | begin | end | program | real | integer | procedure | var
    SYMBOL, // ( | ) | * | / | + | - | <> | >= | <= | > | < | $ | : | , | ; | = | := | .
    SEPARATORS, // ' ' | \t | \n
} lexical_token_class_t;

const char *lexical_token_class_to_string(lexical_token_class_t class);

// FIXME: Maybe create a module for entities
typedef struct {
    size_t line;
    size_t col;
} token_position_t;

void token_position_init(token_position_t *pos);

typedef struct
{
    lexical_token_class_t class;
    token_position_t start_position;
    token_position_t end_position;
    string_t *value; // Optional
    bool is_consumed;
} token_t;

token_t *token_init(void);

bool is_token_valid(token_t *token);

void token_reset(token_t *token);

void token_destroy(token_t *token);

void token_log(token_t *token);

void token_pretty_log(token_t *token);

void append_char_to_token(token_t *token, file_t *file, int c);

#endif //GCC_LEXICAL_TOKEN_H
