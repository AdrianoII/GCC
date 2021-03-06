//
// Created by adrianoii on 23/07/2021.
//
#ifndef GCC_STRING_H
#define GCC_STRING_H

#include <stddef.h>
#include <stdbool.h>

#define INIT_STRING_SIZE 32
#define STRING_REALLOC_FACTOR 2

typedef struct string {
    char *buffer;
    size_t length;
    size_t max_size;
} string_t;

bool literal_equals(const char *s1, const char *s2);

string_t *string_init(void);

void string_reset(string_t *string);

void string_destroy(string_t *string);

void string_realloc(string_t *string);

bool string_equals(string_t const *s1, string_t const *s2);

bool string_equals_literal(string_t *s1, const char *literal);

bool string_is_empty(string_t *s);

void string_log(string_t *string, size_t ident_level);

void string_append_char(string_t *string, char c);

void string_pop_char(string_t *string);

void string_realloc_to_n(string_t *string, size_t n);

string_t *string_copy(string_t const *string);

// TODO: Find a better place to fit this
// Char utils
bool is_separator(int c);
bool is_alpha(int c);
bool is_num(int c);
bool is_alphanum(int c);
bool is_symbol(int c);
bool is_valid_char_value(int c);

// String utils
bool is_keyword(string_t *string);

//Mix utils
bool make_double_symbol(int c, string_t *string);
#endif //GCC_STRING_H
