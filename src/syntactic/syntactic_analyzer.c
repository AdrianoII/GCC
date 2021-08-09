//
// Created by adrianoii on 06/08/2021.
//

#include "syntactic_analyzer.h"
#include <stdbool.h>
#include "../lexical/lexical_analyzer.h"
#include "../file_handler/file_handler.h"
#include "../logs/logs.h"
#include "../exceptions/exceptions_handler.h"

void throw_syntactic_error(token_t *token, char const *error_msg, char const *expected_msg,
                           exception_t exception, bool stop_on_error, size_t *num_errors);
// Nonterminals
void programa(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void corpo(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void dc(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void mais_dc(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void dc_v(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void variaveis(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void mais_var(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void tipo_var(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void dc_p(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void parametros(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void lista_par(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void mais_par(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void corpo_p(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void dc_loc(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void mais_dcloc(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void comandos(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void comando(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void condicao(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void expressao(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void termo(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void op_un(token_t *token, file_t *file, cli_args_t const *args);
void fator(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void mais_fatores(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void op_mul(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void outros_termos(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void op_ad(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void relacao(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void pfalsa(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void restoIdent(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void lista_arg(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void argumentos(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void mais_ident(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);
void mais_comandos(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);

void start_syntactic_analysis(token_t *const token, file_t *const file, cli_args_t const *const args
                              , size_t *const num_errors)
{
    programa(token, file, args, num_errors);
}

void programa(token_t *const token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == KEYWORD && string_equals_literal(token->value, "program"))
    {
        token->is_consumed = true;
        get_next_token(file, token, args->stop_on_error);
        if (token->class == IDENTIFIER)
        {
            token->is_consumed = true;
            corpo(token, file, args, num_errors);
            get_next_token(file, token, args->stop_on_error);
            if (token->class == SYMBOL && string_equals_literal(token->value, "."))
            {
                token->is_consumed = true;
                return;
            }
            else
            {
                throw_syntactic_error(token, "Invalid syntax: Missing . at the end of source file",
                                      ".", SYN_ERROR, args->stop_on_error, num_errors);
            }
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid identifier for program",
                                  "A valid identifier, e.g. \"square_root\"", SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid key word",
                              "program", SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void corpo(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    dc(token, file, args, num_errors);
    get_next_token(file, token, args->stop_on_error);
    if (token->class == KEYWORD && string_equals_literal(token->value, "begin"))
    {
        token->is_consumed = true;
        comandos(token, file, args, num_errors);
        get_next_token(file, token, args->stop_on_error);
        if (token->class == KEYWORD && string_equals_literal(token->value, "end"))
        {
            token->is_consumed = true;
            return;
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid key word",
                                  "end", SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid key word",
                              "begin", SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void dc(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    // lookahead
    if (token->class == KEYWORD && string_equals_literal(token->value, "var"))
    {
        dc_v(token, file, args, num_errors);
        mais_dc(token, file, args, num_errors);
    }
    else if (token->class == KEYWORD && string_equals_literal(token->value, "procedure"))
    {
        dc_p(token, file, args, num_errors);
        mais_dc(token, file, args, num_errors);
    }
    // Ɛ
}

void mais_dc(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL && string_equals_literal(token->value, ";"))
    {
        token->is_consumed = true;
        dc(token, file, args, num_errors);
    }
    // Ɛ
}

void dc_v(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == KEYWORD && string_equals_literal(token->value, "var"))
    {
        token->is_consumed = true;
        variaveis(token, file, args, num_errors);

        get_next_token(file, token, args->stop_on_error);
        if (token->class == SYMBOL && string_equals_literal(token->value, ":"))
        {
            token->is_consumed = true;
            tipo_var(token, file, args, num_errors);
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid symbol",
                                  ":", SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid key word",
                              "var", SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void variaveis(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == IDENTIFIER)
    {
        token->is_consumed = true;
        mais_var(token, file, args, num_errors);
        return;
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid identifier for variable",
                              "A valid identifier, e.g. \"square_root\"",
                              SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void mais_var(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL && string_equals_literal(token->value, ","))
    {
        token->is_consumed = true;
        variaveis(token, file, args, num_errors);
    }
    // Ɛ
}

void tipo_var(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == KEYWORD && string_equals_literal(token->value, "real"))
    {
        token->is_consumed = true;
        return;
    }
    else if (token->class == KEYWORD && string_equals_literal(token->value, "integer"))
    {
        token->is_consumed = true;
        return;
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid type name for variable",
                              "real | interger",
                              SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void dc_p(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == KEYWORD && string_equals_literal(token->value, "procedure"))
    {
        token->is_consumed = true;
        get_next_token(file, token, args->stop_on_error);
        if (token->class == IDENTIFIER)
        {
            token->is_consumed = true;
            parametros(token, file, args, num_errors);
            corpo_p(token, file, args, num_errors);
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid identifier for procedure",
                                  "A valid identifier, e.g. \"square_root\"",
                                  SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid keyword",
                              "procedure",
                              SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void parametros(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL && string_equals_literal(token->value, "("))
    {
        token->is_consumed = true;

        lista_par(token, file, args, num_errors);

        get_next_token(file, token, args->stop_on_error);
        if (token->class == SYMBOL && string_equals_literal(token->value, ")"))
        {
            token->is_consumed = true;
            return;
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid symbol in procedure declaration",
                                  ")",
                                  SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    // Ɛ
}

void mais_par(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL && string_equals_literal(token->value, ";"))
    {
        token->is_consumed = true;

        lista_par(token, file, args, num_errors);

        return;
    }
    // Ɛ
}

void lista_par(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    variaveis(token, file, args, num_errors);

    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL && string_equals_literal(token->value, ":"))
    {
        token->is_consumed = true;

        tipo_var(token, file, args, num_errors);
        mais_par(token, file, args, num_errors);

        return;
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid symbol in procedure declaration parameters",
                              ":",
                              SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void corpo_p(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    dc_loc(token, file, args, num_errors);

    get_next_token(file, token, args->stop_on_error);
    if (token->class == KEYWORD && string_equals_literal(token->value, "begin"))
    {
        token->is_consumed = true;

        comandos(token, file, args, num_errors);

        get_next_token(file, token, args->stop_on_error);
        if (token->class == KEYWORD && string_equals_literal(token->value, "end"))
        {
            token->is_consumed = true;
            return;
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid keyword in procedure definition",
                                  "end",
                                  SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid keyword in procedure definition",
                              "begin",
                              SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void dc_loc(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    // lookahead
    get_next_token(file, token, args->stop_on_error);
    if (token->class == KEYWORD && string_equals_literal(token->value, "var"))
    {
        dc_v(token, file, args, num_errors);
        mais_dcloc(token, file, args, num_errors);
    }
    // Ɛ
}

void mais_dcloc(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL && string_equals_literal(token->value, ";"))
    {
        token->is_consumed = true;
        dc_loc(token, file, args, num_errors);
    }
    // Ɛ
}

void comandos(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    comando(token, file, args, num_errors);
    mais_comandos(token, file, args, num_errors);
}

void comando(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);

    if (token->class == KEYWORD && string_equals_literal(token->value, "read"))
    {
        token->is_consumed = true;

        get_next_token(file, token, args->stop_on_error);
        if (token->class == SYMBOL && string_equals_literal(token->value, "("))
        {
            token->is_consumed = true;

            variaveis(token, file, args, num_errors);

            get_next_token(file, token, args->stop_on_error);
            if (token->class == SYMBOL && string_equals_literal(token->value, ")"))
            {
                token->is_consumed = true;
                return;
            }
            else
            {
                throw_syntactic_error(token, "Invalid syntax: Invalid read command",
                                      ")",
                                      SYN_ERROR, args->stop_on_error, num_errors);
            }
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid read command",
                                  "(",
                                  SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    else if (token->class == KEYWORD && string_equals_literal(token->value, "write"))
    {
        token->is_consumed = true;

        get_next_token(file, token, args->stop_on_error);
        if (token->class == SYMBOL && string_equals_literal(token->value, "("))
        {
            token->is_consumed = true;

            variaveis(token, file, args, num_errors);

            get_next_token(file, token, args->stop_on_error);
            if (token->class == SYMBOL && string_equals_literal(token->value, ")"))
            {
                token->is_consumed = true;
                return;
            }
            else
            {
                throw_syntactic_error(token, "Invalid syntax: Invalid write command",
                                      ")",
                                      SYN_ERROR, args->stop_on_error, num_errors);
            }
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid write command",
                                  "(",
                                  SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    else if (token->class == KEYWORD && string_equals_literal(token->value, "while"))
    {
        token->is_consumed = true;

        condicao(token, file, args, num_errors);

        get_next_token(file, token, args->stop_on_error);
        if (token->class == KEYWORD && string_equals_literal(token->value, "do"))
        {
            token->is_consumed = true;

            comandos(token, file, args, num_errors);

            get_next_token(file, token, args->stop_on_error);
            if (token->class == SYMBOL && string_equals_literal(token->value, "$"))
            {
                token->is_consumed = true;

            }
            else
            {
                throw_syntactic_error(token, "Invalid syntax: Invalid while command",
                                      "$",
                                      SYN_ERROR, args->stop_on_error, num_errors);
            }
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid while command",
                                  "do",
                                  SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    else if (token->class == KEYWORD && string_equals_literal(token->value, "if"))
    {
        token->is_consumed = true;

        condicao(token, file, args, num_errors);

        get_next_token(file, token, args->stop_on_error);
        if (token->class == KEYWORD && string_equals_literal(token->value, "then"))
        {
            token->is_consumed = true;

            comandos(token, file, args, num_errors);

            pfalsa(token, file, args, num_errors);

            get_next_token(file, token, args->stop_on_error);
            if (token->class == SYMBOL && string_equals_literal(token->value, "$"))
            {
                token->is_consumed = true;

            }
            else
            {
                throw_syntactic_error(token, "Invalid syntax: Invalid if command",
                                      "$",
                                      SYN_ERROR, args->stop_on_error, num_errors);
            }
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid if command",
                                  "then",
                                  SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    else if (token->class == IDENTIFIER)
    {
        token->is_consumed = true;

        restoIdent(token, file, args, num_errors);
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid command",
                              "read | write | while | if | A valid identifier, e.g. \"square_root\"",
                              SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void condicao(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    expressao(token, file, args, num_errors);
    relacao(token, file, args, num_errors);
    expressao(token, file, args, num_errors);
}

void expressao(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    termo(token, file, args, num_errors);
    outros_termos(token, file, args, num_errors);
}

void termo(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    op_un(token, file, args);
    fator(token, file, args, num_errors);
    mais_fatores(token, file, args, num_errors);
}

void op_un(token_t *token, file_t *const file, cli_args_t const *const args)
{
    get_next_token(file, token, args->stop_on_error);

    if (token->class == SYMBOL && string_equals_literal(token->value, "+"))
    {
        token->is_consumed = true;
        return;
    }
    else if (token->class == SYMBOL && string_equals_literal(token->value, "-"))
    {
        token->is_consumed = true;
        return;
    }
    // Ɛ
}

void fator(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);

    if (token->class == IDENTIFIER)
    {
        token->is_consumed = true;
        return;
    }
    else if (token->class == INTEGER)
    {
        token->is_consumed = true;
        return;
    }
    else if (token->class == REAL)
    {
        token->is_consumed = true;
        return;
    }
    else if (token->class == SYMBOL && string_equals_literal(token->value, "("))
    {
        token->is_consumed = true;

        expressao(token, file, args, num_errors);

        get_next_token(file, token, args->stop_on_error);
        if (token->class == SYMBOL && string_equals_literal(token->value, ")"))
        {
            token->is_consumed = true;
            return;
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid factor",
                                  ")",
                                  SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid factor",
                              "A valid identifier, e.g. \"square_root\" |\n\t"
                              "A valid integer, e.g. 123 |\n\t"
                              "A valid real, e.g. 3,14 |\n\t"
                              "A valid expression between parenthesis, e.g. (2+3)",
                              SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void mais_fatores(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    // lookahead
    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL && (string_equals_literal(token->value, "*") ||
                                   string_equals_literal(token->value, "/")
    ))
    {
        op_mul(token, file, args, num_errors);
        fator(token, file, args, num_errors);
        mais_fatores(token, file, args, num_errors);
    }
    // Ɛ
}

void op_mul(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);

    if (token->class == SYMBOL && string_equals_literal(token->value, "*"))
    {
        token->is_consumed = true;
        return;
    }
    else if (token->class == SYMBOL && string_equals_literal(token->value, "/"))
    {
        token->is_consumed = true;
        return;
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid operator",
                              "* | /",
                              SYN_ERROR, args->stop_on_error, num_errors);
    }
}


void outros_termos(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    // lookahead
    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL && (string_equals_literal(token->value, "+") ||
                                   string_equals_literal(token->value, "-")
    ))
    {
        op_ad(token, file, args, num_errors);
        termo(token, file, args, num_errors);
        outros_termos(token, file, args, num_errors);
    }
    // Ɛ
}

void op_ad(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);

    if (token->class == SYMBOL && string_equals_literal(token->value, "+"))
    {
        token->is_consumed = true;
        return;
    }
    else if (token->class == SYMBOL && string_equals_literal(token->value, "-"))
    {
        token->is_consumed = true;
        return;
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid operator",
                              "+ | -",
                              SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void relacao(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL &&
        (string_equals_literal(token->value, "=") || string_equals_literal(token->value, "<>") ||
         string_equals_literal(token->value, ">=") || string_equals_literal(token->value, "<=") ||
         string_equals_literal(token->value, ">") || string_equals_literal(token->value, "<")))
    {
        token->is_consumed = true;
        return;
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid relational operator",
                              "= | <> | >= | <= | > | <",
                              SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void pfalsa(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);

    if (token->class == KEYWORD && string_equals_literal(token->value, "else"))
    {
        token->is_consumed = true;

        comandos(token, file, args, num_errors);
    }
    // Ɛ
}

void restoIdent(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);

    if (token->class == SYMBOL && string_equals_literal(token->value, ":="))
    {
        token->is_consumed = true;

        expressao(token, file, args, num_errors);
    }

    lista_arg(token, file, args, num_errors);
}

void lista_arg(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL && string_equals_literal(token->value, "("))
    {
        token->is_consumed = true;

        argumentos(token, file, args, num_errors);

        get_next_token(file, token, args->stop_on_error);
        if (token->class == SYMBOL && string_equals_literal(token->value, ")"))
        {
            token->is_consumed = true;
            return;
        }
        else
        {
            throw_syntactic_error(token, "Invalid syntax: Invalid symbol in procedure call",
                                  ")",
                                  SYN_ERROR, args->stop_on_error, num_errors);
        }
    }
    // Ɛ
}

void argumentos(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == IDENTIFIER)
    {
        token->is_consumed = true;

        mais_ident(token, file, args, num_errors);
    }
    else
    {
        throw_syntactic_error(token, "Invalid syntax: Invalid arguments in procedure call",
                              ")",
                              SYN_ERROR, args->stop_on_error, num_errors);
    }
}

void mais_ident(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL && string_equals_literal(token->value, ";"))
    {
        token->is_consumed = true;

        argumentos(token, file, args, num_errors);
    }
    // Ɛ
}

void mais_comandos(token_t *token, file_t *const file, cli_args_t const *const args, size_t *const num_errors)
{
    get_next_token(file, token, args->stop_on_error);
    if (token->class == SYMBOL && string_equals_literal(token->value, ";"))
    {
        token->is_consumed = true;

        comandos(token, file, args, num_errors);
    }
    // Ɛ
}

void throw_syntactic_error(token_t *const token, char const *const error_msg, char const *const expected_msg,
                           exception_t exception, bool stop_on_error, size_t *num_errors)
{
    ++(*num_errors);
    printf("%zu:%zu~%zu:%zu: ", token->start_position.line + 1, token->start_position.col + 1,
           token->end_position.line + 1, token->end_position.col + 1);
    log_with_color(RED, "SYNTACTIC ERROR: ");
    printf("%s\n", error_msg);
    printf("Expected:\n");
    printf("\t%s\n", expected_msg);
    printf("Got:\n");
    printf("\t%s\n", token->value->buffer);
    if (stop_on_error)
    {
        throw_exception(exception);
    }

    token->is_consumed = true;
}


