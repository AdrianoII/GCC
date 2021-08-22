//
// Created by adrianoii on 06/08/2021.
//

#include "syntactic_analyzer.h"
#include <stdbool.h>
#include "../lexical/lexical_analyzer.h"
#include "../logs/logs.h"
#include "../exceptions/exceptions_handler.h"

void throw_syntactic_error(char const *error_msg, char const *expected_msg, exception_t exception,
                           source_file_metadata_t *metadata);
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
                source_file_metadata->tip = "Check the end of your file to see if you missed the '.'";
                throw_syntactic_error("Missing . at the end of source file", ".", SYN_ERROR, source_file_metadata);
            }
        }
        else
        {
            source_file_metadata->tip = "Identifiers should be a sequence of letters and digits,"
                                        "but starting with a letter";
            throw_syntactic_error("Invalid identifier for program", "A valid identifier, e.g. \"square_root\"",
                                  SYN_ERROR, source_file_metadata);
        }
    }
    else
    {
        source_file_metadata->tip = "Check the start of your source file to verify if you miss the program keyword";
        throw_syntactic_error("Invalid key word", "program", SYN_ERROR, source_file_metadata);
    }
}

void corpo(source_file_metadata_t *const source_file_metadata)
{
    bool error_happened = false;

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
            error_happened = true;
            source_file_metadata->tip = "Did you miss an \"end\" keyword on the main procedure body?";
            throw_syntactic_error("Invalid key word", "end", SYN_ERROR, source_file_metadata);
        }
    }
    else
    {
        error_happened = true;
        source_file_metadata->tip = "Did you miss a \"begin\" keyword on the main procedure body?";
        throw_syntactic_error("Invalid key word", "begin", SYN_ERROR, source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(corpo) = {.}
        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               !(source_file_metadata->token->class == SYMBOL &&
                string_equals_literal(source_file_metadata->token->value, ".")))
        {
            source_file_metadata->token->is_consumed = true;
        }
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
    bool error_happened = false;

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
            error_happened = true;
            source_file_metadata->tip = "Did you miss the \":\" symbol in a variable declaration?";
            throw_syntactic_error("Invalid symbol", ":", SYN_ERROR, source_file_metadata);
        }
    }
    else
    {
        error_happened = true;
        source_file_metadata->tip = "Did you miss the \"var\" keyword in a variable declaration?";
        throw_syntactic_error("Invalid key word", "var", SYN_ERROR, source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(DC_V) = {;, begin}
        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               (!(source_file_metadata->token->class == SYMBOL &&
                 string_equals_literal(source_file_metadata->token->value, ";")) &&
                !(source_file_metadata->token->class == KEYWORD &&
                 string_equals_literal(source_file_metadata->token->value, "begin"))))
        {
            source_file_metadata->token->is_consumed = true;
        }
    }
}

void variaveis(source_file_metadata_t *const source_file_metadata)
{
    bool error_happened = false;

    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == IDENTIFIER)
    {
        source_file_metadata->token->is_consumed = true;
        mais_var(source_file_metadata);
        return;
    }
    else
    {
        error_happened = true;
        source_file_metadata->tip = "A valid identifier is a sequence of letters,  uppercase or not, and digits, "
                                    "starting with a letter";
        throw_syntactic_error("Invalid identifier for variable", "A valid identifier, e.g. \"square_root\"",
                              SYN_ERROR, source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(variaveis) = {), :}

        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               (!(source_file_metadata->token->class == SYMBOL &&
                 string_equals_literal(source_file_metadata->token->value, ")")) &&
                !(source_file_metadata->token->class == SYMBOL &&
                 string_equals_literal(source_file_metadata->token->value, ":"))))
        {
            source_file_metadata->token->is_consumed = true;
        }
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
    bool error_happened = false;

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
        error_happened = true;
        source_file_metadata->tip = "Did you use a valid lalg type, like real or integer?";
        throw_syntactic_error("Invalid type for variable", "real | integer", SYN_ERROR,
                              source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(tipo_var) = {), ;, begin}

        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               (!(source_file_metadata->token->class == SYMBOL &&
                 string_equals_literal(source_file_metadata->token->value, ")")) &&
                !(source_file_metadata->token->class == SYMBOL &&
                 string_equals_literal(source_file_metadata->token->value, ";")) &&
                !(source_file_metadata->token->class == KEYWORD &&
                 string_equals_literal(source_file_metadata->token->value, "begin"))))
        {
            source_file_metadata->token->is_consumed = true;
        }
    }
}

void dc_p(source_file_metadata_t *const source_file_metadata)
{
    bool error_happened = false;

    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == KEYWORD &&
        string_equals_literal(source_file_metadata->token->value, "procedure"))
    {
        source_file_metadata->token->is_consumed = true;
        get_next_token(source_file_metadata->file, source_file_metadata->token,
                       source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == IDENTIFIER)
        {
            source_file_metadata->token->is_consumed = true;
            parametros(source_file_metadata);
            corpo_p(source_file_metadata);
            return;
        }
        else
        {
            error_happened = true;
            source_file_metadata->tip = "A valid identifier is a sequence of letters,  uppercase or not, and digits, "
                                        "starting with a letter";
            throw_syntactic_error("Invalid identifier for procedure", "A valid identifier, e.g. \"square_root\"",
                                  SYN_ERROR, source_file_metadata);
        }
    }
    else
    {
        error_happened = true;
        source_file_metadata->tip = "Did you miss the \"procedure\" keyword in the procedure declaration?";
        throw_syntactic_error("Invalid keyword", "procedure", SYN_ERROR, source_file_metadata);
    }


    if (error_happened)
    {
        // NEXT(dc_p) = {}

        throw_exception(SYN_ERROR);
    }
}

void parametros(source_file_metadata_t *const source_file_metadata)
{
    bool error_happened = false;

    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, "("))
    {
        source_file_metadata->token->is_consumed = true;

        lista_par(source_file_metadata);

        get_next_token(source_file_metadata->file, source_file_metadata->token,
                       source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == SYMBOL &&
            string_equals_literal(source_file_metadata->token->value, ")"))
        {
            source_file_metadata->token->is_consumed = true;
            return;
        }
        else
        {
            error_happened = true;
            source_file_metadata->tip = "Did you miss a \")\" in the procedure declaration?";
            throw_syntactic_error("Invalid symbol in procedure declaration", ")", SYN_ERROR,
                                  source_file_metadata);
        }
    }
    // Ɛ

    if (error_happened)
    {
        // NEXT(parametros) = {begin, var}

        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               (!(source_file_metadata->token->class == KEYWORD &&
                 string_equals_literal(source_file_metadata->token->value, "begin")) &&
                !(source_file_metadata->token->class == KEYWORD &&
                 string_equals_literal(source_file_metadata->token->value, "var"))))
        {
            source_file_metadata->token->is_consumed = true;
        }
    }
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
    bool error_happened = false;

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
        error_happened = true;
        source_file_metadata->tip = "Did you miss a \":\" in the parameter declaration?";
        throw_syntactic_error("Invalid symbol in procedure declaration parameters", ":", SYN_ERROR,
                              source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(lista_par) = {)}

        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               !(source_file_metadata->token->class == SYMBOL &&
                string_equals_literal(source_file_metadata->token->value, ")")))
        {
            source_file_metadata->token->is_consumed = true;
        }
    }
}

void corpo_p(source_file_metadata_t *const source_file_metadata)
{
    bool error_happened = false;

    dc_loc(source_file_metadata);

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
            error_happened = true;
            source_file_metadata->tip = "Did you miss an \"end\" in the procedure body?";
            throw_syntactic_error("Invalid keyword in procedure definition", "end", SYN_ERROR,
                                  source_file_metadata);
        }
    }
    else
    {
        error_happened = true;
        source_file_metadata->tip = "Did you miss an \"begin\" in the procedure body?";
        throw_syntactic_error("Invalid keyword in procedure definition", "begin", SYN_ERROR,
                              source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(corpo_p) = {}

        throw_exception(SYN_ERROR);
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
    bool error_happened = false;

    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);

    if (source_file_metadata->token->class == KEYWORD &&
        string_equals_literal(source_file_metadata->token->value, "read"))
    {
        source_file_metadata->token->is_consumed = true;

        get_next_token(source_file_metadata->file, source_file_metadata->token,
                       source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == SYMBOL &&
            string_equals_literal(source_file_metadata->token->value, "("))
        {
            source_file_metadata->token->is_consumed = true;

            variaveis(source_file_metadata);

            get_next_token(source_file_metadata->file, source_file_metadata->token,
                           source_file_metadata->args->stop_on_error);
            if (source_file_metadata->token->class == SYMBOL &&
                string_equals_literal(source_file_metadata->token->value, ")"))
            {
                source_file_metadata->token->is_consumed = true;
                return;
            }
            else
            {
                error_happened = true;
                source_file_metadata->tip = "Did you miss a \")\" in the read command?";
                throw_syntactic_error("Invalid read command", ")", SYN_ERROR, source_file_metadata);
            }
        }
        else
        {
            error_happened = true;
            source_file_metadata->tip = "Did you miss a \"(\" in the read command?";
            throw_syntactic_error("Invalid read command", "(", SYN_ERROR, source_file_metadata);
        }
    }
    else if (source_file_metadata->token->class == KEYWORD &&
             string_equals_literal(source_file_metadata->token->value, "write"))
    {
        source_file_metadata->token->is_consumed = true;

        get_next_token(source_file_metadata->file, source_file_metadata->token,
                       source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == SYMBOL &&
            string_equals_literal(source_file_metadata->token->value, "("))
        {
            source_file_metadata->token->is_consumed = true;

            variaveis(source_file_metadata);

            get_next_token(source_file_metadata->file, source_file_metadata->token,
                           source_file_metadata->args->stop_on_error);
            if (source_file_metadata->token->class == SYMBOL &&
                string_equals_literal(source_file_metadata->token->value, ")"))
            {
                source_file_metadata->token->is_consumed = true;
                return;
            }
            else
            {
                error_happened = true;
                source_file_metadata->tip = "Did you miss a \")\" in the write command?";
                throw_syntactic_error("Invalid write command", ")", SYN_ERROR, source_file_metadata);
            }
        }
        else
        {
            error_happened = true;
            source_file_metadata->tip = "Did you miss a \"(\" in the write command?";
            throw_syntactic_error("Invalid write command", "(", SYN_ERROR, source_file_metadata);
        }
    }
    else if (source_file_metadata->token->class == KEYWORD &&
             string_equals_literal(source_file_metadata->token->value, "while"))
    {
        source_file_metadata->token->is_consumed = true;

        condicao(source_file_metadata);

        get_next_token(source_file_metadata->file, source_file_metadata->token,
                       source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == KEYWORD &&
            string_equals_literal(source_file_metadata->token->value, "do"))
        {
            source_file_metadata->token->is_consumed = true;

            comandos(source_file_metadata);

            get_next_token(source_file_metadata->file, source_file_metadata->token,
                           source_file_metadata->args->stop_on_error);
            if (source_file_metadata->token->class == SYMBOL &&
                string_equals_literal(source_file_metadata->token->value, "$"))
            {
                source_file_metadata->token->is_consumed = true;
                return;
            }
            else
            {
                error_happened = true;
                source_file_metadata->tip = "Did you miss a \"$\" in the while command?";
                throw_syntactic_error("Invalid while command", "$", SYN_ERROR, source_file_metadata);
            }
        }
        else
        {
            error_happened = true;
            source_file_metadata->tip = "Did you miss a \"do\" in the while command?";
            throw_syntactic_error("Invalid while command", "do", SYN_ERROR, source_file_metadata);
        }
    }
    else if (source_file_metadata->token->class == KEYWORD &&
             string_equals_literal(source_file_metadata->token->value, "if"))
    {
        source_file_metadata->token->is_consumed = true;

        condicao(source_file_metadata);

        get_next_token(source_file_metadata->file, source_file_metadata->token,
                       source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == KEYWORD &&
            string_equals_literal(source_file_metadata->token->value, "then"))
        {
            source_file_metadata->token->is_consumed = true;

            comandos(source_file_metadata);

            pfalsa(source_file_metadata);

            get_next_token(source_file_metadata->file, source_file_metadata->token,
                           source_file_metadata->args->stop_on_error);
            if (source_file_metadata->token->class == SYMBOL &&
                string_equals_literal(source_file_metadata->token->value, "$"))
            {
                source_file_metadata->token->is_consumed = true;

            }
            else
            {
                error_happened = true;
                source_file_metadata->tip = "Did you miss a \"$\" in the if command?";
                throw_syntactic_error("Invalid if command", "$", SYN_ERROR, source_file_metadata);
            }
        }
        else
        {
            error_happened = true;
            source_file_metadata->tip = "Did you miss a \"then\" in the if command?";
            throw_syntactic_error("Invalid if command", "then", SYN_ERROR, source_file_metadata);
        }
    }
    else if (source_file_metadata->token->class == IDENTIFIER)
    {
        source_file_metadata->token->is_consumed = true;

        restoIdent(source_file_metadata);
    }
    else
    {
        error_happened = true;
        throw_syntactic_error("Invalid command", "read | write | while | if | A valid identifier, e.g. \"square_root\"",
                              SYN_ERROR, source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(comando) = {end}

        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               !(source_file_metadata->token->class == KEYWORD &&
                string_equals_literal(source_file_metadata->token->value, "end")))
        {
            source_file_metadata->token->is_consumed = true;
        }
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
    bool error_happened = false;

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

        get_next_token(source_file_metadata->file, source_file_metadata->token,
                       source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == SYMBOL &&
            string_equals_literal(source_file_metadata->token->value, ")"))
        {
            source_file_metadata->token->is_consumed = true;
            return;
        }
        else
        {
            error_happened = true;
            source_file_metadata->tip = "Did you miss a \")\" in the factor?";
            throw_syntactic_error("Invalid factor", ")", SYN_ERROR, source_file_metadata);
        }
    }
    else
    {
        error_happened = true;
        throw_syntactic_error("Invalid factor",
                              "A valid identifier, e.g. \"square_root\" |\n\t"
                              "A valid integer, e.g. 123 |\n\t"
                              "A valid real, e.g. 3,14 |\n\t"
                              "A valid expression between parenthesis, e.g. (2+3)", SYN_ERROR,
                              source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(fator) = {$, ;, <, <=, <>, =, >, >=, do, else, end, then}

        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               (!(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, "$")) &&
                !(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, ";")) &&
                !(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, "<")) &&
                !(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, "<=")) &&
                !(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, "<>")) &&
                !(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, "=")) &&
                !(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, ">")) &&
                !(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, ">=")) &&
                !(source_file_metadata->token->class == KEYWORD &&
                  string_equals_literal(source_file_metadata->token->value, "do")) &&
                !(source_file_metadata->token->class == KEYWORD &&
                  string_equals_literal(source_file_metadata->token->value, "else")) &&
                !(source_file_metadata->token->class == KEYWORD &&
                  string_equals_literal(source_file_metadata->token->value, "end")) &&
                !(source_file_metadata->token->class == KEYWORD &&
                  string_equals_literal(source_file_metadata->token->value, "then"))))
        {
            source_file_metadata->token->is_consumed = true;
        }
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
    bool error_happened = false;

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
        error_happened = true;
        throw_syntactic_error("Invalid operator", "* | /", SYN_ERROR, source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(op_mul) = {(, ident, numero_int, numero_real)}

        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               (!(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, "(")) &&
                source_file_metadata->token->class != IDENTIFIER &&
                source_file_metadata->token->class != INTEGER &&
                source_file_metadata->token->class != REAL))
        {
            source_file_metadata->token->is_consumed = true;
        }
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
    bool error_happened = false;

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
        error_happened = true;
        throw_syntactic_error("Invalid operator", "+ | -", SYN_ERROR, source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(op_ad) = {(, ident, numero_int, numero_real)}

        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               (!(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, "(")) &&
                source_file_metadata->token->class != IDENTIFIER &&
                source_file_metadata->token->class != INTEGER &&
                source_file_metadata->token->class != REAL))
        {
            source_file_metadata->token->is_consumed = true;
        }
    }
}

void relacao(source_file_metadata_t *const source_file_metadata)
{
    bool error_happened = false;

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
        error_happened = true;
        throw_syntactic_error("Invalid relational operator", "= | <> | >= | <= | > | <", SYN_ERROR,
                              source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(relacao) = {(, +, -, ident, numero_int, numero_real)}

        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               (!(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, "(")) &&
                !(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, "+")) &&
                !(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, "-")) &&
                source_file_metadata->token->class != IDENTIFIER &&
                source_file_metadata->token->class != INTEGER &&
                source_file_metadata->token->class != REAL))
        {
            source_file_metadata->token->is_consumed = true;
        }
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
    bool error_happened = false;

    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == SYMBOL && string_equals_literal(source_file_metadata->token->value, "("))
    {
        source_file_metadata->token->is_consumed = true;

        argumentos(source_file_metadata);

        get_next_token(source_file_metadata->file, source_file_metadata->token,
                       source_file_metadata->args->stop_on_error);
        if (source_file_metadata->token->class == SYMBOL &&
            string_equals_literal(source_file_metadata->token->value, ")"))
        {
            source_file_metadata->token->is_consumed = true;
            return;
        }
        else
        {
            error_happened = true;
            source_file_metadata->tip = "Did you miss a \")\" at a procedure call?";
            throw_syntactic_error("Invalid symbol in procedure call", ")", SYN_ERROR, source_file_metadata);
        }
    }
    // Ɛ

    if (error_happened)
    {
        // NEXT(lista_arg) = {$, else, end, then}

        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
               (!(source_file_metadata->token->class == SYMBOL &&
                  string_equals_literal(source_file_metadata->token->value, "$")) &&
                !(source_file_metadata->token->class == KEYWORD &&
                  string_equals_literal(source_file_metadata->token->value, "else")) &&
                !(source_file_metadata->token->class == KEYWORD &&
                  string_equals_literal(source_file_metadata->token->value, "end")) &&
                !(source_file_metadata->token->class == KEYWORD &&
                  string_equals_literal(source_file_metadata->token->value, "then"))))
        {
            source_file_metadata->token->is_consumed = true;
        }
    }
}

void argumentos(source_file_metadata_t *const source_file_metadata)
{
    bool error_happened = false;

    get_next_token(source_file_metadata->file, source_file_metadata->token, source_file_metadata->args->stop_on_error);
    if (source_file_metadata->token->class == IDENTIFIER)
    {
        source_file_metadata->token->is_consumed = true;

        mais_ident(source_file_metadata);
    }
    else
    {
        error_happened = true;
        source_file_metadata->tip = "Did you miss an argument at a procedure call?";
        throw_syntactic_error("Invalid arguments in procedure call", ")", SYN_ERROR, source_file_metadata);
    }

    if (error_happened)
    {
        // NEXT(argumentos) = {)}

        while (get_next_token(source_file_metadata->file, source_file_metadata->token,
                              source_file_metadata->args->stop_on_error) &&
                              (!(source_file_metadata->token->class == SYMBOL &&
                              string_equals_literal(source_file_metadata->token->value, ")"))))
        {
            source_file_metadata->token->is_consumed = true;
        }
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

void throw_syntactic_error(char const *const error_msg, char const *const expected_msg, exception_t exception,
                           source_file_metadata_t *const metadata)
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

    if (metadata->tip != NULL)
    {
        log_with_color(CYN, "TIP: ");
        printf("%s\n", metadata->tip);
    }

    if (metadata->args->stop_on_error)
    {
        throw_exception(exception);
    }

    metadata->tip = NULL;
}
