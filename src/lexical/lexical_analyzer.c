//
// Created by adrianoii on 24/07/2021.
//

#include "lexical_analyzer.h"
#include <stdbool.h>
#include "../logs/logs.h"
#include "../exceptions/exceptions_handler.h"

void treat_lexical_error(token_t *token, bool stop_on_error);

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
        }
        else
        {
            eof_found = true;
        }

        //Skip separators
        if (token->token_class == EMPTY_TOKEN_CLASS && is_separator(source_file->actual_char))
        {
            appendChar = false;
        }
        else if (is_token_valid(token))
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
                    }
                    else
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
            }
            else if (token->token_class == IDENTIFIER)
            {
                if (!is_alphanum(source_file->actual_char))
                {
                    if (is_keyword(token->value))
                    {
                        token->token_class = KEYWORD;
                    }

                    token->end_position = source_file->col;
                    isTokenClassified = true;
                    appendChar = false;
                    hasLexicalError = false;
                    rollback_actual_char = true;
                }
            }
            else if (token->token_class == INTEGER)
            {
                if (source_file->actual_char == '.')
                {
                    token->token_class = REAL;
                }
                else if (!is_num(source_file->actual_char))
                {
                    token->end_position = source_file->col;
                    isTokenClassified = true;
                    appendChar = false;
                    hasLexicalError = false;
                    rollback_actual_char = true;
                }
            }
            else if (token->token_class == REAL)
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
            }
            else if (token->token_class == SYMBOL)
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
                }
                else
                {
                    token->end_position = source_file->col;
                    isTokenClassified = true;
                    appendChar = false;
                    hasLexicalError = false;
                    rollback_actual_char = true;
                }
            }
        }
        else
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
        treat_lexical_error(token, stop_on_error);
    }

    return !eof_found;
}

void treat_lexical_error(token_t *token, bool stop_on_error)
{
    switch (token->token_class)
    {
        case BRACKET_COMMENT:
            log_with_color(RED, "LEXICAL_ERROR: ");
            log_with_color(GRN, "Invalid bracket comment at line ");
            printf("%zu", token->line + 1);
            log_with_color(GRN, ".\n");
            if (stop_on_error)
            {
                throw_exception(LEX_INVALID_BRACKET_COMMENT);
            }
            break;
        case SLASH_COMMENT:
            log_with_color(RED, "LEXICAL_ERROR: ");
            log_with_color(GRN, "Invalid slash comment at line ");
            printf("%zu", token->line + 1);
            log_with_color(GRN, ".\n");
            if (stop_on_error)
            {
                throw_exception(LEX_INVALID_SLASH_COMMENT);
            }
            break;
        case REAL:
            log_with_color(RED, "LEXICAL_ERROR: ");
            log_with_color(GRN, "Invalid real at line ");
            printf("%zu", token->line + 1);
            log_with_color(GRN, ".\n");
            if (stop_on_error)
            {
                throw_exception(LEX_INVALID_FLOAT);
            }
            break;
        case SYMBOL:
            log_with_color(RED, "LEXICAL_ERROR: ");
            log_with_color(GRN, "Invalid symbol at line ");
            printf("%zu", token->line + 1);
            log_with_color(GRN, ".\n");
            if (stop_on_error)
            {
                throw_exception(LEX_INVALID_SYMBOL);
            }
            break;
        default:
            if (is_token_valid(token))
            {
                log_with_color(RED, "INVALID LEXICAL ERROR: ");
                printf("Invalid state with Token class = %s.\n", lexical_token_class_to_string(token->token_class));
                throw_exception(INVALID_LEXICAL_ERROR_STATE);
            }
    }
    token->token_class = INVALID_TOKEN_CLASS;
}
