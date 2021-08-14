//
// Created by adrianoii on 06/08/2021.
//

#include "syntactic_analyzer.h"
#include <stdbool.h>
#include "../lexical/lexical_analyzer.h"
#include "../logs/logs.h"
#include "../exceptions/exceptions_handler.h"

void throw_syntactic_error(char const *error_msg, char const *expected_msg, char const *tip,
                           exception_t exception, source_file_metadata_t *source_file_metadata);
// Nonterminals
void programa(source_file_metadata_t *source_file_metadata);
void corpo(source_file_metadata_t *source_file_metadata);
void dc(source_file_metadata_t *source_file_metadata);
void mais_dc(source_file_metadata_t *source_file_metadata);
void dc_v(source_file_metadata_t *source_file_metadata);
void variaveis(source_file_metadata_t *source_file_metadata);
void mais_var(source_file_metadata_t *source_file_metadata);
void tipo_var(source_file_metadata_t *source_file_metadata);
void dc_p(source_file_metadata_t *source_file_metadata);
void parametros(source_file_metadata_t *source_file_metadata);
void lista_par(source_file_metadata_t *source_file_metadata);
void mais_par(source_file_metadata_t *source_file_metadata);
void corpo_p(source_file_metadata_t *source_file_metadata);
void dc_loc(source_file_metadata_t *source_file_metadata);
void mais_dcloc(source_file_metadata_t *source_file_metadata);
void comandos(source_file_metadata_t *source_file_metadata);
void comando(source_file_metadata_t *source_file_metadata);
void condicao(source_file_metadata_t *source_file_metadata);
void expressao(source_file_metadata_t *source_file_metadata);
void termo(source_file_metadata_t *source_file_metadata);
void op_un(source_file_metadata_t *source_file_metadata);
void fator(source_file_metadata_t *source_file_metadata);
void mais_fatores(source_file_metadata_t *source_file_metadata);
void op_mul(source_file_metadata_t *source_file_metadata);
void outros_termos(source_file_metadata_t *source_file_metadata);
void op_ad(source_file_metadata_t *source_file_metadata);
void relacao(source_file_metadata_t *source_file_metadata);
void pfalsa(source_file_metadata_t *source_file_metadata);
void restoIdent(source_file_metadata_t *source_file_metadata);
void lista_arg(source_file_metadata_t *source_file_metadata);
void argumentos(source_file_metadata_t *source_file_metadata);
void mais_ident(source_file_metadata_t *source_file_metadata);
void mais_comandos(source_file_metadata_t *source_file_metadata);

void start_syntactic_analysis(source_file_metadata_t *const source_file_metadata)
{
    programa(source_file_metadata);
}

void programa(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == KEYWORD &&
        string_equals_literal(source_file_metadata->token->value, "program"))
    {
        source_file_metadata->token->is_consumed = true;
        get_next_token(source_file_metadata->file, source_file_metadata->token,
                       source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == IDENTIFIER)
        {
            source_file_metadata->token->is_consumed = true;
            corpo(source_file_metadata);
            get_next_token(source_file_metadata->file, source_file_metadata->token,
                           source_file_metadata->args->stop_on_error);
            if (source_file_metadata->token->class == SYMBOL &&
                string_equals_literal(source_file_metadata->token->value, "."))
            {
                source_file_metadata->token->is_consumed = true;
                return;
            }
            else
            {
                throw_syntactic_error("Missing . at the end of source file", ".",
                                      "Did you miss a ';' at the end of a command?", SYN_ERROR, source_file_metadata);
            }
        }
        else
        {
            throw_syntactic_error("Invalid identifier for program", "A valid identifier, e.g. \"square_root\"", NULL,
                                  SYN_ERROR, source_file_metadata);
        }
    }
    else
    {
        throw_syntactic_error("Invalid key word", "program", NULL, SYN_ERROR, source_file_metadata);
    }
}

void corpo(source_file_metadata_t *const source_file_metadata)
{
    dc(source_file_metadata);
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == KEYWORD &&
        string_equals_literal(source_file_metadata->token->value, "begin"))
    {
        source_file_metadata->token->is_consumed = true;
        comandos(source_file_metadata);
        get_next_token(source_file_metadata->file, source_file_metadata->token,
                       source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == KEYWORD &&
            string_equals_literal(source_file_metadata->token->value, "end"))
        {
            source_file_metadata->token->is_consumed = true;
            return;
        }
        else
        {
            throw_syntactic_error("Invalid key word", "end", NULL, SYN_ERROR, source_file_metadata);
        }
    }
    else
    {
        throw_syntactic_error("Invalid key word", "begin", NULL, SYN_ERROR, source_file_metadata);
    }
}

void dc(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    // lookahead
    if (source_file_metadata->token->class == KEYWORD &&
        string_equals_literal(source_file_metadata->token->value, "var"))
    {
        dc_v(source_file_metadata);

        mais_dc(source_file_metadata);

        return;
    }
    else if (source_file_metadata->token->class == KEYWORD &&
             string_equals_literal(source_file_metadata->token->value, "procedure"))
    {
        dc_p(source_file_metadata);
        mais_dc(source_file_metadata);
        return;
    }
    // Ɛ
}

void mais_dc(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, ";"))
    {
        source_file_metadata->token->is_consumed = true;
        dc(source_file_metadata);
    }
    // Ɛ
}

void dc_v(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == KEYWORD &&
        string_equals_literal(source_file_metadata->token->value, "var"))
    {
        source_file_metadata->token->is_consumed = true;
        variaveis(source_file_metadata);

        get_next_token(source_file_metadata->file, source_file_metadata->token,
                       source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == SYMBOL &&
            string_equals_literal(source_file_metadata->token->value, ":"))
        {
            source_file_metadata->token->is_consumed = true;
            tipo_var(source_file_metadata);
            return;
        }
        else
        {
            throw_syntactic_error("Invalid symbol", ":", NULL, SYN_ERROR, source_file_metadata);
            // FOLLOW
            do
            {
                source_file_metadata->token->is_consumed = true;
            } while (((source_file_metadata->token->class != SYMBOL &&
                       !string_equals_literal(source_file_metadata->token->value, ";")) &&
                      (source_file_metadata->token->class != KEYWORD &&
                       !string_equals_literal(source_file_metadata->token->value, "begin"))) &&
                     get_next_token(source_file_metadata->file, source_file_metadata->token,
                                    source_file_metadata->args->stop_on_error));
            return;
        }
    }
    else
    {
        throw_syntactic_error("Invalid key word", "var", NULL, SYN_ERROR, source_file_metadata);
        return;
    }
}

void variaveis(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == IDENTIFIER)
    {
        source_file_metadata->token->is_consumed = true;
        mais_var(source_file_metadata);
        return;
    }
    else
    {
        throw_syntactic_error("Invalid identifier for variable", "A valid identifier, e.g. \"square_root\"", NULL,
                              SYN_ERROR, source_file_metadata);
    }
}

void mais_var(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, ","))
    {
        source_file_metadata->token->is_consumed = true;
        variaveis(source_file_metadata);
    }
    // Ɛ
}

void tipo_var(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == KEYWORD &&
        string_equals_literal(source_file_metadata->token->value, "real"))
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    else if (source_file_metadata->token->class == KEYWORD &&
             string_equals_literal(source_file_metadata->token->value, "integer"))
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    else
    {
        throw_syntactic_error("Invalid type name for variable", "real | interger", NULL, SYN_ERROR,
                              source_file_metadata);
    }
}

void dc_p(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == KEYWORD &&
        string_equals_literal(source_file_metadata->token->value, "procedure"))
    {
        source_file_metadata->token->is_consumed = true;
        get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == IDENTIFIER)
        {
            source_file_metadata->token->is_consumed = true;
            parametros(source_file_metadata);
            corpo_p(source_file_metadata);
        }
        else
        {
            throw_syntactic_error("Invalid identifier for procedure", "A valid identifier, e.g. \"square_root\"", NULL,
                                  SYN_ERROR, source_file_metadata);
        }
    }
    else
    {
        throw_syntactic_error("Invalid keyword", "procedure", NULL, SYN_ERROR, source_file_metadata);
    }
}

void parametros(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, "("))
    {
        source_file_metadata->token->is_consumed = true;

        lista_par(source_file_metadata);

        get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == SYMBOL &&
            string_equals_literal(source_file_metadata->token->value, ")"))
        {
            source_file_metadata->token->is_consumed = true;
            return;
        }
        else
        {
            throw_syntactic_error("Invalid symbol in procedure declaration", ")", NULL, SYN_ERROR,
                                  source_file_metadata);
        }
    }
    // Ɛ
}

void mais_par(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, ";"))
    {
        source_file_metadata->token->is_consumed = true;

        lista_par(source_file_metadata);

        return;
    }
    // Ɛ
}

void lista_par(source_file_metadata_t *const source_file_metadata)
{
    variaveis(source_file_metadata);

    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, ":"))
    {
        source_file_metadata->token->is_consumed = true;

        tipo_var(source_file_metadata);
        mais_par(source_file_metadata);

        return;
    }
    else
    {
        throw_syntactic_error("Invalid symbol in procedure declaration parameters", ":", NULL, SYN_ERROR,
                              source_file_metadata);
    }
}

void corpo_p(source_file_metadata_t *const source_file_metadata)
{
    dc_loc(source_file_metadata);

    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == KEYWORD && string_equals_literal(source_file_metadata->token->value, "begin"))
    {
        source_file_metadata->token->is_consumed = true;

        comandos(source_file_metadata);

        get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == KEYWORD &&
            string_equals_literal(source_file_metadata->token->value, "end"))
        {
            source_file_metadata->token->is_consumed = true;
            return;
        }
        else
        {
            throw_syntactic_error("Invalid keyword in procedure definition", "end", NULL, SYN_ERROR,
                                  source_file_metadata);
        }
    }
    else
    {
        throw_syntactic_error("Invalid keyword in procedure definition", "begin", NULL, SYN_ERROR,
                              source_file_metadata);
    }
}

void dc_loc(source_file_metadata_t *const source_file_metadata)
{
    // lookahead
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == KEYWORD &&
        string_equals_literal(source_file_metadata->token->value, "var"))
    {
        dc_v(source_file_metadata);
        mais_dcloc(source_file_metadata);
    }
    // Ɛ
}

void mais_dcloc(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, ";"))
    {
        source_file_metadata->token->is_consumed = true;
        dc_loc(source_file_metadata);
    }
    // Ɛ
}

void comandos(source_file_metadata_t *const source_file_metadata)
{
    comando(source_file_metadata);
    mais_comandos(source_file_metadata);
}

void comando(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);

    if (source_file_metadata->token->class == KEYWORD &&
        string_equals_literal(source_file_metadata->token->value, "read"))
    {
        source_file_metadata->token->is_consumed = true;

        get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == SYMBOL &&
            string_equals_literal(source_file_metadata->token->value, "("))
        {
            source_file_metadata->token->is_consumed = true;

            variaveis(source_file_metadata);

            get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
            if (source_file_metadata->token->class == SYMBOL &&
                string_equals_literal(source_file_metadata->token->value, ")"))
            {
                source_file_metadata->token->is_consumed = true;
                return;
            }
            else
            {
                throw_syntactic_error("Invalid read command", ")", NULL, SYN_ERROR, source_file_metadata);
            }
        }
        else
        {
            throw_syntactic_error("Invalid read command", "(", NULL, SYN_ERROR, source_file_metadata);
        }
    }
    else if (source_file_metadata->token->class == KEYWORD &&
             string_equals_literal(source_file_metadata->token->value, "write"))
    {
        source_file_metadata->token->is_consumed = true;

        get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == SYMBOL &&
            string_equals_literal(source_file_metadata->token->value, "("))
        {
            source_file_metadata->token->is_consumed = true;

            variaveis(source_file_metadata);

            get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
            if (source_file_metadata->token->class == SYMBOL &&
                string_equals_literal(source_file_metadata->token->value, ")"))
            {
                source_file_metadata->token->is_consumed = true;
                return;
            }
            else
            {
                throw_syntactic_error("Invalid write command", ")", NULL, SYN_ERROR, source_file_metadata);
            }
        }
        else
        {
            throw_syntactic_error("Invalid write command", "(", NULL, SYN_ERROR, source_file_metadata);
        }
    }
    else if (source_file_metadata->token->class == KEYWORD &&
             string_equals_literal(source_file_metadata->token->value, "while"))
    {
        source_file_metadata->token->is_consumed = true;

        condicao(source_file_metadata);

        get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == KEYWORD &&
            string_equals_literal(source_file_metadata->token->value, "do"))
        {
            source_file_metadata->token->is_consumed = true;

            comandos(source_file_metadata);

            get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
            if (source_file_metadata->token->class == SYMBOL &&
                string_equals_literal(source_file_metadata->token->value, "$"))
            {
                source_file_metadata->token->is_consumed = true;

            }
            else
            {
                throw_syntactic_error("Invalid while command", "$", NULL, SYN_ERROR, source_file_metadata);
            }
        }
        else
        {
            throw_syntactic_error("Invalid while command", "do", NULL, SYN_ERROR, source_file_metadata);
        }
    }
    else if (source_file_metadata->token->class == KEYWORD &&
             string_equals_literal(source_file_metadata->token->value, "if"))
    {
        source_file_metadata->token->is_consumed = true;

        condicao(source_file_metadata);

        get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == KEYWORD &&
            string_equals_literal(source_file_metadata->token->value, "then"))
        {
            source_file_metadata->token->is_consumed = true;

            comandos(source_file_metadata);

            pfalsa(source_file_metadata);

            get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
            if (source_file_metadata->token->class == SYMBOL &&
                string_equals_literal(source_file_metadata->token->value, "$"))
            {
                source_file_metadata->token->is_consumed = true;

            }
            else
            {
                throw_syntactic_error("Invalid if command", "$", NULL, SYN_ERROR, source_file_metadata);
            }
        }
        else
        {
            throw_syntactic_error("Invalid if command", "then", NULL, SYN_ERROR, source_file_metadata);
        }
    }
    else if (source_file_metadata->token->class == IDENTIFIER)
    {
        source_file_metadata->token->is_consumed = true;

        restoIdent(source_file_metadata);
    }
    else
    {
        throw_syntactic_error("Invalid command", "read | write | while | if | A valid identifier, e.g. \"square_root\"",
                              NULL, SYN_ERROR, source_file_metadata);
    }
}

void condicao(source_file_metadata_t *const source_file_metadata)
{
    expressao(source_file_metadata);
    relacao(source_file_metadata);
    expressao(source_file_metadata);
}

void expressao(source_file_metadata_t *const source_file_metadata)
{
    termo(source_file_metadata);
    outros_termos(source_file_metadata);
}

void termo(source_file_metadata_t *const source_file_metadata)
{
    op_un(source_file_metadata);
    fator(source_file_metadata);
    mais_fatores(source_file_metadata);
}

void op_un(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);

    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, "+"))
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    else if (source_file_metadata->token->class == SYMBOL &&
             string_equals_literal(source_file_metadata->token->value, "-"))
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    // Ɛ
}

void fator(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);

    if (source_file_metadata->token->class == IDENTIFIER)
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    else if (source_file_metadata->token->class == INTEGER)
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    else if (source_file_metadata->token->class == REAL)
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    else if (source_file_metadata->token->class == SYMBOL &&
             string_equals_literal(source_file_metadata->token->value, "("))
    {
        source_file_metadata->token->is_consumed = true;

        expressao(source_file_metadata);

        get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == SYMBOL &&
            string_equals_literal(source_file_metadata->token->value, ")"))
        {
            source_file_metadata->token->is_consumed = true;
            return;
        }
        else
        {
            throw_syntactic_error("Invalid factor", ")", NULL, SYN_ERROR, source_file_metadata);
        }
    }
    else
    {
        throw_syntactic_error("Invalid factor",
                              "A valid identifier, e.g. \"square_root\" |\n\t"
                              "A valid integer, e.g. 123 |\n\t"
                              "A valid real, e.g. 3,14 |\n\t"
                              "A valid expression between parenthesis, e.g. (2+3)", NULL, SYN_ERROR,
                              source_file_metadata);
    }
}

void mais_fatores(source_file_metadata_t *const source_file_metadata)
{
    // lookahead
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL &&
        (string_equals_literal(source_file_metadata->token->value, "*") ||
         string_equals_literal(source_file_metadata->token->value, "/")
        ))
    {
        op_mul(source_file_metadata);
        fator(source_file_metadata);
        mais_fatores(source_file_metadata);
    }
    // Ɛ
}

void op_mul(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);

    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, "*"))
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    else if (source_file_metadata->token->class == SYMBOL &&
             string_equals_literal(source_file_metadata->token->value, "/"))
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    else
    {
        throw_syntactic_error("Invalid operator", "* | /", NULL, SYN_ERROR, source_file_metadata);
    }
}


void outros_termos(source_file_metadata_t *const source_file_metadata)
{
    // lookahead
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL &&
        (string_equals_literal(source_file_metadata->token->value, "+") ||
         string_equals_literal(source_file_metadata->token->value, "-")
        ))
    {
        op_ad(source_file_metadata);
        termo(source_file_metadata);
        outros_termos(source_file_metadata);
    }
    // Ɛ
}

void op_ad(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);

    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, "+"))
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    else if (source_file_metadata->token->class == SYMBOL &&
             string_equals_literal(source_file_metadata->token->value, "-"))
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    else
    {
        throw_syntactic_error("Invalid operator", "+ | -", NULL, SYN_ERROR, source_file_metadata);
    }
}

void relacao(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL &&
        (string_equals_literal(source_file_metadata->token->value, "=") ||
         string_equals_literal(source_file_metadata->token->value, "<>") ||
         string_equals_literal(source_file_metadata->token->value, ">=") ||
         string_equals_literal(source_file_metadata->token->value, "<=") ||
         string_equals_literal(source_file_metadata->token->value, ">") ||
         string_equals_literal(source_file_metadata->token->value, "<")))
    {
        source_file_metadata->token->is_consumed = true;
        return;
    }
    else
    {
        throw_syntactic_error("Invalid relational operator", "= | <> | >= | <= | > | <", NULL, SYN_ERROR,
                              source_file_metadata);
    }
}

void pfalsa(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);

    if (source_file_metadata->token->class == KEYWORD &&
        string_equals_literal(source_file_metadata->token->value, "else"))
    {
        source_file_metadata->token->is_consumed = true;

        comandos(source_file_metadata);
    }
    // Ɛ
}

void restoIdent(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);

    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, ":="))
    {
        source_file_metadata->token->is_consumed = true;

        expressao(source_file_metadata);
    }

    lista_arg(source_file_metadata);
}

void lista_arg(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, "("))
    {
        source_file_metadata->token->is_consumed = true;

        argumentos(source_file_metadata);

        get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == SYMBOL &&
            string_equals_literal(source_file_metadata->token->value, ")"))
        {
            source_file_metadata->token->is_consumed = true;
            return;
        }
        else
        {
            throw_syntactic_error("Invalid symbol in procedure call", ")", NULL, SYN_ERROR, source_file_metadata);
        }
    }
    // Ɛ
}

void argumentos(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == IDENTIFIER)
    {
        source_file_metadata->token->is_consumed = true;

        mais_ident(source_file_metadata);
    }
    else
    {
        throw_syntactic_error("Invalid arguments in procedure call", ")", NULL, SYN_ERROR, source_file_metadata);
    }
}

void mais_ident(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, ";"))
    {
        source_file_metadata->token->is_consumed = true;

        argumentos(source_file_metadata);
    }
    // Ɛ
}

void mais_comandos(source_file_metadata_t *const source_file_metadata)
{
    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, ";"))
    {
        source_file_metadata->token->is_consumed = true;

        comandos(source_file_metadata);
    }
    // Ɛ
}

void throw_syntactic_error(char const *const error_msg, char const *const expected_msg, char const *const tip,
                           exception_t exception, source_file_metadata_t  *const metadata)
{
    ++metadata->num_errors;
    printf("%zu:%zu~%zu:%zu: ", metadata->token->start_position.line + 1, metadata->token->start_position.col + 1,
           metadata->token->end_position.line + 1, metadata->token->end_position.col + 1);
    log_with_color(RED, "SYNTACTIC ERROR: ");
    printf("%s\n", error_msg);
    printf("Expected:\n");
    printf("\t%s\n", expected_msg);
    printf("Got:\n");
    printf("\t%s\n", metadata->token->value->buffer);

    if (tip != NULL)
    {
        log_with_color(CYN, "TIP: ");
        printf("%s\n", tip);
    }

    if (metadata->args->stop_on_error)
    {
        throw_exception(exception);
    }

    metadata->token->is_consumed = true;
}


