//
// Created by adrianoii on 23/07/2021.
//

#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../exceptions/exception.h"
#include "../exceptions/exceptions_handler.h"

string_t *string_init()
{
    string_t *new_string = calloc(1, sizeof(string_t));

    if (new_string == NULL)
    {
        throw_exception(ALLOCATION_FAILED);
    }

    new_string->max_size = INIT_STRING_SIZE;
    new_string->buffer = calloc(new_string->max_size, sizeof(char));

    if (new_string->buffer == NULL)
    {
        throw_exception(ALLOCATION_FAILED);
    }

    string_reset(new_string);

    return new_string;
}

void string_reset(string_t *string)
{
    string->length = 0;
    memset(string->buffer, '\0', string->max_size);
}

void string_destroy(string_t *string)
{
    if (string != NULL)
    {
        free(string->buffer);
    }
    free(string);
}

void string_realloc(string_t *string)
{
    size_t new_buffer_size = string->max_size * STRING_REALLOC_FACTOR;
    char *new_buffer = realloc(string->buffer, new_buffer_size);

    if (new_buffer == NULL)
    {
        throw_exception(ALLOCATION_FAILED);
    }

    size_t diff_buffer_size = new_buffer_size - string->max_size;
    char *p_diff_buffer_start = new_buffer + string->max_size;
    memset(p_diff_buffer_start, '\0', diff_buffer_size);
    string->buffer = new_buffer;
    string->max_size = new_buffer_size;
}

bool string_equals(string_t *s1, string_t *s2)
{
    return strcmp(s1->buffer, s2->buffer) == 0;
}

bool string_equals_literal(string_t *s1, const char *literal)
{
    return strcmp(s1->buffer, literal) == 0;
}

bool string_is_empty(string_t *s)
{
    return string_equals_literal(s, "");
}

void string_log(string_t *string)
{
    printf("STRING:\n");

    if (string == NULL)
    {
        printf("{}\n");
        return;
    }

    printf("{\n\tlength: %zu,\n\tmax_size: %zu,\n\t", string->length, string->max_size);
    if (string->buffer == NULL)
    {
        printf("buffer: null,\n");
    }
    else
    {
        printf("buffer: \"%s\",\n", string->buffer);
    }

    printf("}\n");
}

void string_append_char(string_t *string, char c)
{
    if (string->length + 1 == string->max_size)
    {
        string_realloc(string);
    }

    string->buffer[string->length++] = c;
}

void string_pop_char(string_t *string)
{
    string->buffer[(string->length--) - 1] = '\0';
}

bool is_separator(int c)
{
    return (c == ' ') || (c == '\t') || (c == '\n');
}

bool is_alpha(int c)
{
    return (c > 64 && c < 91) || (c > 96 && c < 122);
}

bool is_num(int c)
{
    return (c > 47 && c < 58);
}

bool is_alphanum(int c)
{
    return is_alpha(c) || is_num(c);
}

bool is_keyword(string_t *string)
{
    const char *keywords[14] = {
            "if",
            "then",
            "while",
            "do",
            "write",
            "read",
            "else",
            "begin",
            "end",
            "program",
            "real",
            "integer",
            "procedure",
            "var"
    };

    bool token_is_keyword = false;

    for (size_t i = 0; i < 14 && !token_is_keyword; i++)
    {
        token_is_keyword = string_equals_literal(string, keywords[i]);
    }

    return token_is_keyword;
}

bool is_symbol(int c)
{
    const char symbols[14] = {'(', ')', '*', '/', '+', '-', '<', '>', '$', ':', ',', ';', '=', '.'};

    bool char_is_symbol = false;

    for (size_t i = 0; i < 14 && !char_is_symbol; i++)
    {
        char_is_symbol = symbols[i] == c;
    }

    return char_is_symbol;
}

bool is_valid_char_value(int c)
{
    // Check if c is EOF or an ascii char.
    return c >= -1 && c <= 127;
}
