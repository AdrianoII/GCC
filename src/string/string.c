//
// Created by adrianoii on 23/07/2021.
//

#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../s_mem_alloc/s_mem_alloc.h"

bool literal_equals(const char *const s1, const char *const s2)
{
    return strcmp(s1, s2) == 0;
}

string_t *string_init(void)
{
    string_t *new_string = s_mem_alloc(1, sizeof(string_t));

    new_string->max_size = INIT_STRING_SIZE;
    new_string->buffer = s_mem_alloc(new_string->max_size, sizeof(char));

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
        remove_elem_free_list(string->buffer, true);
    }
    remove_elem_free_list(string, true);
}

void string_realloc(string_t *string)
{
    string_realloc_to_n(string, string->max_size * STRING_REALLOC_FACTOR);
}

void string_realloc_to_n(string_t *string, size_t const n)
{
    string->buffer = s_mem_realloc(string->buffer, string->max_size, n);
    string->max_size = n;
    if (string->length > n)
    {
        string->buffer[n - 1] = '\0';
         string->length = n - 1;
    }
}

bool string_equals(string_t const *const s1, string_t const *const s2)
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

void string_log(string_t *string, size_t ident_level)
{
    char *ident = calloc(ident_level + 1, sizeof(char));

    memset(ident,  '\t', ident_level);

    if (string == NULL)
    {
        printf("%s{},\n", ident);
        return;
    }

    printf("%s{\n", ident);
    printf("%s\t\"length\": %zu,\n", ident, string->length);
    printf("%s\t\"max_size\": %zu,\n\t", ident, string->max_size);
    if (string->buffer == NULL)
    {
        printf("%s\"buffer\": null,\n", ident);
    }
    else
    {
        printf("%s\"buffer\": \"%s\",\n", ident, string->buffer);
    }

    printf("%s},\n", ident);
    free(ident);
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
    return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
}

bool is_alpha(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c < 'Z');
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

bool make_double_symbol(int c, string_t *string)
{
    return ((c == '>') && (string_equals_literal(string, "<"))) //<>
           || ((c == '=') && (string_equals_literal(string, ">"))) // >=
           || ((c == '=') && (string_equals_literal(string, "<"))) // <=
           || ((c == '=') && (string_equals_literal(string, ":"))); // :=
}

string_t *string_copy(string_t const *const string)
{
    string_t *new_s = string_init();

    string_realloc_to_n(new_s, string->max_size);
    memcpy(new_s->buffer, string->buffer, string->max_size);
    new_s->max_size = string->max_size;
    new_s->length = string->length;

    return new_s;
}
