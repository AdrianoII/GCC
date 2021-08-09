//
// Created by adrianoii on 23/07/2021.
//

#include "lexical_token.h"
#include <stdio.h>
#include <stdlib.h>
#include "../exceptions/exceptions_handler.h"

const char *lexical_token_class_to_string(lexical_token_class_t class)
{
    //TODO: Maybe set the vector as a global
    const char *token_class_mapper[] = {
            "EMPTY_TOKEN_CLASS",
            "INVALID_TOKEN_CLASS",
            "BRACKET_COMMENT",
            "SLASH_COMMENT",
            "IDENTIFIER",
            "INTEGER",
            "REAL",
            "KEYWORD",
            "SYMBOL",
            "SEPARATORS",
    };

    return token_class_mapper[class];
}

void token_position_init(token_position_t *pos)
{
    pos->line = 0;
    pos->col = 0;
}

token_t *token_init(void)
{
    token_t *new_token = calloc(1, sizeof(token_t));

    if (new_token == NULL)
    {
        throw_exception(ALLOCATION_FAILED);
    }

    new_token->value = string_init();

    token_reset(new_token);

    return new_token;
}

void token_reset(token_t *token)
{
    token_position_init(&token->start_position);
    token_position_init(&token->end_position);
    token->class = EMPTY_TOKEN_CLASS;
    string_reset(token->value);
    token->is_consumed = true;
}

bool is_token_valid(token_t *token)
{
    return token->class != EMPTY_TOKEN_CLASS && token->class != INVALID_TOKEN_CLASS;
}

void token_destroy(token_t *token)
{
    if (token != NULL)
    {
        string_destroy(token->value);
    }
    free(token);
}

void token_log(token_t *token)
{
    printf("TOKEN:\n");
    if (token == NULL)
    {
        printf("{}\n");
        return;
    }

    printf("{\n");
    printf("\ttoken_class: \"%s\"\n", lexical_token_class_to_string(token->class));
    printf("\tstart_position: {\n");
    printf("\t\tline :%zu\n", token->start_position.line);
    printf("\t\tcol: %zu", token->start_position.col);
    printf("\t}\n");
    printf("\tend_position: {\n");
    printf("\t\tline :%zu\n", token->end_position.line);
    printf("\t\tcol: %zu", token->end_position.col);
    printf("\t}\n");
    printf("\tvalue: ");
    string_log(token->value);
    printf("}\n");
}

void token_pretty_log(token_t *token)
{
    printf("< %s : %s >\n", lexical_token_class_to_string(token->class), token->value->buffer);
}

void append_char_to_token(token_t *token, file_t *file, const int c)
{
    if (string_is_empty(token->value))
    {
        token->start_position.line = file->line;
        token->start_position.col = file->col - 1;
        token->end_position.line = file->line;
        token->end_position.col = file->col - 1;
    }

    string_append_char(token->value, (char) c);
}
