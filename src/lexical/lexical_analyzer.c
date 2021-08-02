//
// Created by adrianoii on 24/07/2021.
//

#include "lexical_analyzer.h"
#include <stdbool.h>
#include "../logs/logs.h"
#include "../exceptions/exceptions_handler.h"

bool is_separator(int c);
bool is_alpha(int c);
bool is_num(int c);
bool is_alphanum(int c);
bool is_keyword(token_t *token);
bool is_symbol(int c);

void append_char_to_token(token_t *token, file_t *file, int c);


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

bool is_keyword(token_t *token)
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
        token_is_keyword = string_equals_literal(token->value, keywords[i]);
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

void append_char_to_token(token_t *token, file_t *file, const int c)
{
    if (string_is_empty(token->value))
    {
        token->line = file->line;
        token->start_position = file->col - 1;
        token->end_position = file->col - 1;
    }

    // deal with new lines and update file handler???
    string_append_char(token->value, c);
}


bool get_next_token(file_t *source_file, token_t *token, bool stop_on_error)
{
    bool appendChar = true;
    bool isTokenClassified = false;
    bool hasLexicalError = true;
    bool rollback_actual_char = false;
    bool eof_found = false;

    while (!isTokenClassified && !eof_found)
    {
        appendChar = true;

        if (source_file->actual_char != EOF)
        {
            file_get_next_char(source_file);
        } else
        {
            eof_found = true;
        }

        //Skip separators
        if (token->token_class == INVALID_TOKEN_CLASS && is_separator(source_file->actual_char))
        {
            appendChar = false;
        } else if (token->token_class != INVALID_TOKEN_CLASS)
        {
            // Search for the end of a bracket comment
            if (token->token_class == BRACKET_COMMENT)
            {
                // FIXME: Dealt with log multiline comment's col
                // End of a bracket comment
                if (source_file->actual_char == '}')
                {
                    token->end_position = source_file->col;
                    isTokenClassified = true;
                    hasLexicalError = false;
                }
                    // Skip this char
                else
                {
                    appendChar = false;
                }
            }
                // Search for the end of a slash comment
            else if (token->token_class == SLASH_COMMENT)
            {
                // FIXME: Dealt with log multiline comment's col
                // possible end o a slash comment
                if (string_equals_literal(token->value, "/**"))
                {
                    if (source_file->actual_char == '/')
                    {
                        token->end_position = source_file->col;
                        isTokenClassified = true;
                        hasLexicalError = false;
                    } else
                    {
                        appendChar = false;
                        // Deal with sequence of '*'
                        if (source_file->actual_char != '*')
                        {
                            string_pop_char(token->value);
                        }
                    }
                }
                    // Try to find the end of a slash comment
                else if (source_file->actual_char == '*')
                {
                    // For now just append to the value
                }
                    // Skip this char
                else
                {
                    appendChar = false;
                }
            } else if (token->token_class == IDENTIFIER)
            {
                if (!is_alphanum(source_file->actual_char))
                {
                    if (is_keyword(token))
                    {
                        token->token_class = KEYWORD;
                    }

                    token->end_position = source_file->col;
                    isTokenClassified = true;
                    appendChar = false;
                    hasLexicalError = false;
                    rollback_actual_char = true;
                }
            } else if (token->token_class == INTEGER)
            {
                if (source_file->actual_char == '.')
                {
                    token->token_class = REAL;
                } else if (!is_num(source_file->actual_char))
                {
                    token->end_position = source_file->col;
                    isTokenClassified = true;
                    appendChar = false;
                    hasLexicalError = false;
                    rollback_actual_char = true;
                }
            } else if (token->token_class == REAL)
            {
                if (!is_num(source_file->actual_char))
                {
                    isTokenClassified = true;
                    if (token->value->length > 2)
                    {
                        token->end_position = source_file->col;
                        appendChar = false;
                        hasLexicalError = false;
                        rollback_actual_char = true;
                    }
                }
            } else if (token->token_class == SYMBOL)
            {
                // <>
                if ((source_file->actual_char == '>') && (string_equals_literal(token->value, "<")))
                {
                    isTokenClassified = true;
                    token->end_position = source_file->col;
                    hasLexicalError = false;
                }
                    // >=
                else if ((source_file->actual_char == '=') && (string_equals_literal(token->value, ">")))
                {
                    isTokenClassified = true;
                    token->end_position = source_file->col;
                    hasLexicalError = false;
                }
                    // <=
                else if ((source_file->actual_char == '=') && (string_equals_literal(token->value, "<")))
                {
                    isTokenClassified = true;
                    token->end_position = source_file->col;
                    hasLexicalError = false;
                }
                    // :=
                else if ((source_file->actual_char == '=') && (string_equals_literal(token->value, ":")))
                {
                    isTokenClassified = true;
                    token->end_position = source_file->col;
                    hasLexicalError = false;
                }
                    // /*
                else if ((source_file->actual_char == '*') && (string_equals_literal(token->value, "/")))
                {
                    token->token_class = SLASH_COMMENT;
                } else
                {
                    token->end_position = source_file->col;
                    isTokenClassified = true;
                    appendChar = false;
                    hasLexicalError = false;
                    rollback_actual_char = true;
                }
            }
        } else
        {
            // Start of a bracket comment
            if (source_file->actual_char == '{')
            {
                token->token_class = BRACKET_COMMENT;
            }
                // Start of an identifier or keyword
            else if (is_alpha(source_file->actual_char))
            {
                token->token_class = IDENTIFIER;

            }
                // Start of an integer or a real
            else if (is_num(source_file->actual_char))
            {
                token->token_class = INTEGER;
            }
                // Start of a symbol or a slash comment
            else if (is_symbol(source_file->actual_char))
            {
                token->token_class = SYMBOL;
            }
        }

        if (appendChar)
        {
            append_char_to_token(token, source_file, source_file->actual_char);
        }

        if (rollback_actual_char)
        {
            file_rollback_byte(source_file);
            token->end_position = source_file->col;
        }
    }

    if (hasLexicalError)
    {
        if (token->token_class == BRACKET_COMMENT)
        {
            log_with_color(RED, "LEXICAL_ERROR: ");
            log_with_color(GRN, "Invalid bracket comment at line ");
            printf("%zu", token->line + 1);
            log_with_color(GRN, ".\n");
            if (stop_on_error)
            {
                throw_exception(LEX_INVALID_BRACKET_COMMENT);
            }
        } else if (token->token_class == SLASH_COMMENT)
        {
            log_with_color(RED, "LEXICAL_ERROR: ");
            log_with_color(GRN, "Invalid slash comment at line ");
            printf("%zu", token->line + 1);
            log_with_color(GRN, ".\n");
            if (stop_on_error)
            {
                throw_exception(LEX_INVALID_SLASH_COMMENT);
            }
        } else if (token->token_class == REAL)
        {
            log_with_color(RED, "LEXICAL_ERROR: ");
            log_with_color(GRN, "Invalid real at line ");
            printf("%zu", token->line + 1);
            log_with_color(GRN, ".\n");
            if (stop_on_error)
            {
                throw_exception(LEX_INVALID_FLOAT);
            }
        } else if (token->token_class == SYMBOL)
        {
            log_with_color(RED, "LEXICAL_ERROR: ");
            log_with_color(GRN, "Invalid symbol at line ");
            printf("%zu", token->line + 1);
            log_with_color(GRN, ".\n");
            if (stop_on_error)
            {
                throw_exception(LEX_INVALID_SYMBOL);
            }
        }
        token->token_class = INVALID_TOKEN_CLASS;
    }

    return !eof_found;
}