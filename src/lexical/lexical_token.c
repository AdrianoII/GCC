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

token_t *token_init()
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
    token->start_position = 0;
    token->end_position = 0;
    token->line = 0;
    token->token_class = INVALID_TOKEN_CLASS;
    string_reset(token->value);
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
    printf("\ttoken_class: \"%s\"\n", lexical_token_class_to_string(token->token_class));
    printf("\tline: %zu\n", token->line);
    printf("\tstart_position: %zu\n", token->start_position);
    printf("\tend_position: %zu\n", token->end_position);
    printf("\tvalue: ");
    string_log(token->value);
    printf("}\n");
}

void token_pretty_log(token_t *token)
{
    printf("< %s : %s >\n", lexical_token_class_to_string(token->token_class), token->value->buffer);
}

void append_char_to_token(token_t *token, file_t *file, const int c)
{
    if (string_is_empty(token->value))
    {
        token->line = file->line;
        token->start_position = file->col - 1;
        token->end_position = file->col - 1;
    }

    // deal with new lines and update file handler???
    string_append_char(token->value, (char) c);
}
