//
// Created by adrianoii on 24/07/2021.
//

#include "lexical_analyzer.h"
#include <stdbool.h>
#include "../logs/logs.h"
#include "../exceptions/exceptions_handler.h"

typedef struct {
    bool appendChar;
    bool isTokenClassified;
    bool hasLexicalError;
    bool rollback_actual_char;
    bool eof_found;
} lexical_parser_states_t;

void lexical_parser_states_init(lexical_parser_states_t *states);

void try_start_token_parse(int actual_char, token_t *token);

void try_classify_token(lexical_parser_states_t *states, token_t *token, file_t *source_file);

void handle_lexical_error(token_t *token, bool stop_on_error);

bool get_next_token(file_t *source_file, token_t *token, bool stop_on_error)
{

    lexical_parser_states_t states;
    lexical_parser_states_init(&states);

    while (!states.isTokenClassified && !states.eof_found)
    {
        states.appendChar = true;

        if (source_file->actual_char != EOF)
        {
            file_get_next_char(source_file);
        }
        else
        {
            states.eof_found = true;
        }

        //Skip separators
        if (token->token_class == EMPTY_TOKEN_CLASS && is_separator(source_file->actual_char))
        {
            states.appendChar = false;
        }
        else if (is_token_valid(token))
        {
            try_classify_token(&states, token, source_file);
        }
        else
        {
            try_start_token_parse(source_file->actual_char, token);
        }

        if (states.appendChar)
        {
            append_char_to_token(token, source_file, source_file->actual_char);
        }

        if (states.rollback_actual_char)
        {
            file_rollback_byte(source_file);
            token->end_position.line = source_file->line;
            token->end_position.col = source_file->col;
        }
    }

    if (token->token_class != EMPTY_TOKEN_CLASS && states.hasLexicalError)
    {
        handle_lexical_error(token, stop_on_error);
    }

    return !states.eof_found;
}

void lexical_parser_states_init(lexical_parser_states_t *states)
{
    states->appendChar = true;
    states->isTokenClassified = false;
    states->hasLexicalError = true;
    states->rollback_actual_char = false;
    states->eof_found = false;
}

void try_start_token_parse(int actual_char, token_t *token)
{
    // Start of a bracket comment
    if (actual_char == '{')
    {
        token->token_class = BRACKET_COMMENT;
    }
        // Start of an identifier or keyword
    else if (is_alpha(actual_char))
    {
        token->token_class = IDENTIFIER;

    }
        // Start of an integer or a real
    else if (is_num(actual_char))
    {
        token->token_class = INTEGER;
    }
        // Start of a symbol or a slash comment
    else if (is_symbol(actual_char))
    {
        token->token_class = SYMBOL;
    }
}

void try_classify_token(lexical_parser_states_t *states, token_t *token, file_t *source_file)
{
    switch (token->token_class)
    {
        // Search for the end of a bracket comment
        case BRACKET_COMMENT:
            // FIXME: Dealt with log multiline comment's col
            // End of a bracket comment
            if (source_file->actual_char == '}')
            {
                token->end_position.line = source_file->line;
                token->end_position.col = source_file->col;
                states->isTokenClassified = true;
                states->hasLexicalError = false;
            }
                // Skip this char
            else
            {
                states->appendChar = false;
            }
            break;
            // Search for the end of a slash comment
        case SLASH_COMMENT:
            // FIXME: Dealt with log multiline comment's col
            // possible end o a slash comment
            if (string_equals_literal(token->value, "/**"))
            {
                if (source_file->actual_char == '/')
                {
                    token->end_position.line = source_file->line;
                    token->end_position.col = source_file->col;
                    states->isTokenClassified = true;
                    states->hasLexicalError = false;
                }
                else
                {
                    states->appendChar = false;
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
                states->appendChar = false;
            }
            break;
        case IDENTIFIER:
            if (!is_alphanum(source_file->actual_char))
            {
                if (is_keyword(token->value))
                {
                    token->token_class = KEYWORD;
                }

                token->end_position.line = source_file->line;
                token->end_position.col = source_file->col;
                states->isTokenClassified = true;
                states->appendChar = false;
                states->hasLexicalError = false;
                states->rollback_actual_char = true;
            }
            break;
        case INTEGER:
            if (source_file->actual_char == '.')
            {
                token->token_class = REAL;
            }
            else if (!is_num(source_file->actual_char))
            {
                token->end_position.line = source_file->line;
                token->end_position.col = source_file->col;
                states->isTokenClassified = true;
                states->appendChar = false;
                states->hasLexicalError = false;
                states->rollback_actual_char = true;
            }
            break;
        case REAL:
            if (!is_num(source_file->actual_char))
            {
                states->isTokenClassified = true;
                if (token->value->length > 2)
                {
                    token->end_position.line = source_file->line;
                    token->end_position.col = source_file->col;
                    states->appendChar = false;
                    states->hasLexicalError = false;
                    states->rollback_actual_char = true;
                }
            }
            break;
        case SYMBOL:
            if(make_double_symbol(source_file->actual_char, token->value))
            {
                states->isTokenClassified = true;
                token->end_position.line = source_file->line;
                token->end_position.col = source_file->col;
                states->hasLexicalError = false;
            }
                // /*
            else if ((source_file->actual_char == '*') && (string_equals_literal(token->value, "/")))
            {
                token->token_class = SLASH_COMMENT;
            }
            else
            {
                token->end_position.line = source_file->line;
                token->end_position.col = source_file->col;
                states->isTokenClassified = true;
                states->appendChar = false;
                states->hasLexicalError = false;
                states->rollback_actual_char = true;
            }
            break;
        default:
            log_with_color(RED, "INVALID LEXICAL STATE: ");
            printf("Invalid state with Token:\n");
            token_pretty_log(token);
            throw_exception(INVALID_LEXICAL_PARSER_STATE);
    }
}

void handle_lexical_error(token_t *token, bool stop_on_error)
{
    printf("%zu:%zu~%zu:%zu: ", token->start_position.line + 1, token->start_position.col + 1,
           token->end_position.line + 1, token->end_position.col + 1);
    log_with_color(RED, "LEXICAL_ERROR: ");

    exception_t exception = EMPTY_EXCEPTION;

    switch (token->token_class)
    {
        case BRACKET_COMMENT:
            // FIXME: log function with 3 args (error, expected, got)
            log_with_color_nl(WHTB, "Invalid bracket comment");
            log_with_color_nl(WHTB, "Expected:");
            printf("\t{ ... }\n");
            log_with_color(WHTB, "Got:\n");
            printf("\t%s\n", token->value->buffer);
            exception = LEX_INVALID_BRACKET_COMMENT;
            break;
        case SLASH_COMMENT:
            log_with_color_nl(WHTB, "Invalid slash comment");
            log_with_color_nl(WHTB, "Expected:");
            printf("\t/* ... */\n");
            log_with_color(WHTB, "Got:\n");
            printf("\t%s\n", token->value->buffer);
            exception = LEX_INVALID_SLASH_COMMENT;
            break;
        case REAL:
            log_with_color_nl(WHTB, "Invalid real");
            log_with_color_nl(WHTB, "Expected (ex.):");
            printf("\t1.23\n");
            log_with_color(WHTB, "Got:\n");
            printf("\t%s\n", token->value->buffer);
            exception = LEX_INVALID_FLOAT;
            break;
        case SYMBOL:
            log_with_color_nl(WHTB, "Invalid symbol");
            log_with_color_nl(WHTB, "Expected:");
            printf("\t( | ) | * | / | + | - | <> | >= | <= | > | < | $ | : | , | ; | = | := | .\n");
            log_with_color(WHTB, "Got:\n");
            printf("\t%s\n", token->value->buffer);
            exception = LEX_INVALID_SYMBOL;
            break;
        default:
            if (is_token_valid(token))
            {
                log_with_color_nl(RED, "INVALID LEXICAL ERROR: ");
                log_with_color_nl(WHTB, "Invalid state with Token:\n");
                token_pretty_log(token);
                throw_exception(INVALID_LEXICAL_ERROR_STATE);
            }
    }

    if (stop_on_error)
    {
        throw_exception(exception);
    }

    token->token_class = INVALID_TOKEN_CLASS;
}
