//
// Created by adrianoii on 06/08/2021.
//

#include "syntactic_analyzer.h"
#include <stdbool.h>
#include "../logs/logs.h"
#include "../exceptions/exceptions_handler.h"
#include "../lexical/lexical_analyzer.h"
#include "../s_mem_alloc/s_mem_alloc.h"
//#include "../semantic/semantic_actions.h"

void throw_syntactic_error(char const *error_msg, char const *expected_msg, exception_t exception,
                           source_file_metadata_t *metadata);
void throw_semantic_error(exception_t exception, source_file_metadata_t *metadata);

// Nonterminals
void programa(source_file_metadata_t *metadata);
void corpo(source_file_metadata_t *metadata);
void dc(source_file_metadata_t *metadata);
void mais_dc(source_file_metadata_t *metadata);
void dc_v(source_file_metadata_t *metadata);
void variaveis(source_file_metadata_t *metadata);
void mais_var(source_file_metadata_t *metadata);
void tipo_var(source_file_metadata_t *metadata);
void dc_p(source_file_metadata_t *metadata);
void parametros(source_file_metadata_t *metadata);
void lista_par(source_file_metadata_t *metadata);
void mais_par(source_file_metadata_t *metadata);
void corpo_p(source_file_metadata_t *metadata);
void dc_loc(source_file_metadata_t *metadata);
void mais_dcloc(source_file_metadata_t *metadata);
void comandos(source_file_metadata_t *metadata);
void comando(source_file_metadata_t *metadata);
void condicao(source_file_metadata_t *metadata);
void expressao(source_file_metadata_t *metadata);
void termo(source_file_metadata_t *metadata);
void op_un(source_file_metadata_t *metadata);
void fator(source_file_metadata_t *metadata);
void mais_fatores(source_file_metadata_t *metadata);
void op_mul(source_file_metadata_t *metadata);
void outros_termos(source_file_metadata_t *metadata);
void op_ad(source_file_metadata_t *metadata);
instruction_t relacao(source_file_metadata_t *metadata);
void pfalsa(source_file_metadata_t *metadata, code_t *template);
void restoIdent(source_file_metadata_t *metadata);
void lista_arg(source_file_metadata_t *metadata);
void argumentos(source_file_metadata_t *metadata);
void mais_ident(source_file_metadata_t *metadata);
void mais_comandos(source_file_metadata_t *metadata);

void start_syntactic_analysis(source_file_metadata_t *const metadata)
{
    programa(metadata);
}

void programa(source_file_metadata_t *const metadata)
{
    int_real_t e;
    e.integer = 0;
    gen_code(metadata->cl, INPP, e, INVALID_DATA_TYPE);
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == KEYWORD &&
        string_equals_literal(metadata->token->value, "program"))
    {
        metadata->token->is_consumed = true;
        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);
        if (metadata->token->class == IDENTIFIER)
        {
            metadata->token->is_consumed = true;

            if (!st_add_proc(metadata->st, metadata->token->value))
            {
                throw_semantic_error(ST_PROC_ALREADY_DECLARED, metadata);
            }

            corpo(metadata);
            get_next_token(metadata->file, metadata->token,
                           metadata->args->stop_on_error);
            if (metadata->token->class == SYMBOL &&
                string_equals_literal(metadata->token->value, "."))
            {
                metadata->token->is_consumed = true;

                gen_code(metadata->cl, PARA, e, INVALID_DATA_TYPE);

                return;
            }
            else
            {
                metadata->tip = "Check the end of your file to see if you missed the '.'";
                throw_syntactic_error("Missing . at the end of source file", ".", SYN_ERROR, metadata);
            }
        }
        else
        {
            metadata->tip = "Identifiers should be a sequence of letters and digits,"
                            "but starting with a letter";
            throw_syntactic_error("Invalid identifier for program", "A valid identifier, e.g. \"square_root\"",
                                  SYN_ERROR, metadata);
        }
    }
    else
    {
        metadata->tip = "Check the start of your source file to verify if you miss the program keyword";
        throw_syntactic_error("Invalid key word", "program", SYN_ERROR, metadata);
    }
}

void corpo(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    dc(metadata);

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == KEYWORD &&
        string_equals_literal(metadata->token->value, "begin"))
    {
        metadata->token->is_consumed = true;
        comandos(metadata);
        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);
        if (metadata->token->class == KEYWORD &&
            string_equals_literal(metadata->token->value, "end"))
        {
            metadata->token->is_consumed = true;
            return;
        }
        else
        {
            error_happened = true;
            metadata->tip = "Did you miss an \"end\" keyword on the main procedure body?";
            throw_syntactic_error("Invalid key word", "end", SYN_ERROR, metadata);
        }
    }
    else
    {
        error_happened = true;
        metadata->tip = "Did you miss a \"begin\" keyword on the main procedure body?";
        throw_syntactic_error("Invalid key word", "begin", SYN_ERROR, metadata);
    }

    if (error_happened)
    {
        // NEXT(corpo) = {.}
        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               !(metadata->token->class == SYMBOL &&
                 string_equals_literal(metadata->token->value, ".")))
        {
            metadata->token->is_consumed = true;
        }
    }
}

void dc(source_file_metadata_t *const metadata)
{
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);

    // lookahead
    if (metadata->token->class == KEYWORD &&
        string_equals_literal(metadata->token->value, "var"))
    {
        dc_v(metadata);

        mais_dc(metadata);

        return;
    }
    else if (metadata->token->class == KEYWORD &&
             string_equals_literal(metadata->token->value, "procedure"))
    {
        dc_p(metadata);

        st_return_global_scope(metadata->st);

        mais_dc(metadata);
        return;
    }
    // Ɛ
}

void mais_dc(source_file_metadata_t *const metadata)
{
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, ";"))
    {
        metadata->token->is_consumed = true;
        dc(metadata);
    }
    // Ɛ
}

void dc_v(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == KEYWORD &&
        string_equals_literal(metadata->token->value, "var"))
    {
        metadata->token->is_consumed = true;

        variaveis(metadata);

        if (!st_add_vars(metadata->st))
        {
            throw_semantic_error(ST_VAR_ALREADY_DECLARED, metadata);
        }

        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);
        if (metadata->token->class == SYMBOL &&
            string_equals_literal(metadata->token->value, ":"))
        {
            metadata->token->is_consumed = true;
            tipo_var(metadata);

            gen_var_alloc_code(metadata->cl, metadata->st);

            analysis_queue_destroy(&metadata->st->analysis_queue);

            return;
        }
        else
        {
            error_happened = true;
            metadata->tip = "Did you miss the \":\" symbol in a variable declaration?";
            throw_syntactic_error("Invalid symbol", ":", SYN_ERROR, metadata);
        }
    }
    else
    {
        error_happened = true;
        metadata->tip = "Did you miss the \"var\" keyword in a variable declaration?";
        throw_syntactic_error("Invalid key word", "var", SYN_ERROR, metadata);
    }

    if (error_happened)
    {
        // NEXT(DC_V) = {;, begin}
        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               (!(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, ";")) &&
                !(metadata->token->class == KEYWORD &&
                  string_equals_literal(metadata->token->value, "begin"))))
        {
            metadata->token->is_consumed = true;
        }
    }
}

void variaveis(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == IDENTIFIER)
    {
        metadata->token->is_consumed = true;

        analysis_queue_append(&metadata->st->analysis_queue, metadata->token->value, metadata->st->actual_scope);

        mais_var(metadata);
        return;
    }
    else
    {
        error_happened = true;
        metadata->tip = "A valid identifier is a sequence of letters,  uppercase or not, and digits, "
                        "starting with a letter";
        throw_syntactic_error("Invalid identifier for variable", "A valid identifier, e.g. \"square_root\"",
                              SYN_ERROR, metadata);
    }

    if (error_happened)
    {
        // NEXT(variaveis) = {), :}

        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               (!(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, ")")) &&
                !(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, ":"))))
        {
            metadata->token->is_consumed = true;
        }
    }
}

void mais_var(source_file_metadata_t *const metadata)
{
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, ","))
    {
        metadata->token->is_consumed = true;
        variaveis(metadata);
    }
    // Ɛ
}

void tipo_var(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == KEYWORD &&
        string_equals_literal(metadata->token->value, "real"))
    {
        metadata->token->is_consumed = true;

        st_add_vars_type(metadata->st, REAL_DATA_TYPE);

        return;
    }
    else if (metadata->token->class == KEYWORD &&
             string_equals_literal(metadata->token->value, "integer"))
    {
        metadata->token->is_consumed = true;

        st_add_vars_type(metadata->st, INTEGER_DATA_TYPE);

        return;
    }
    else
    {
        error_happened = true;
        metadata->tip = "Did you use a valid lalg type, like real or integer?";
        throw_syntactic_error("Invalid type for variable", "real | integer", SYN_ERROR,
                              metadata);
    }

    if (error_happened)
    {
        // NEXT(tipo_var) = {), ;, begin}

        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               (!(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, ")")) &&
                !(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, ";")) &&
                !(metadata->token->class == KEYWORD &&
                  string_equals_literal(metadata->token->value, "begin"))))
        {
            metadata->token->is_consumed = true;
        }
    }
}

void dc_p(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == KEYWORD &&
        string_equals_literal(metadata->token->value, "procedure"))
    {
        metadata->token->is_consumed = true;
        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);
        if (metadata->token->class == IDENTIFIER)
        {
            metadata->token->is_consumed = true;

            if (!st_add_proc(metadata->st, metadata->token->value))
            {
                throw_semantic_error(ST_PROC_ALREADY_DECLARED, metadata);
            }

            parametros(metadata);

            corpo_p(metadata);
            return;
        }
        else
        {
            error_happened = true;
            metadata->tip = "A valid identifier is a sequence of letters,  uppercase or not, and digits, "
                            "starting with a letter";
            throw_syntactic_error("Invalid identifier for procedure", "A valid identifier, e.g. \"square_root\"",
                                  SYN_ERROR, metadata);
        }
    }
    else
    {
        error_happened = true;
        metadata->tip = "Did you miss the \"procedure\" keyword in the procedure declaration?";
        throw_syntactic_error("Invalid keyword", "procedure", SYN_ERROR, metadata);
    }


    if (error_happened)
    {
        // NEXT(dc_p) = {}

        throw_exception(SYN_ERROR);
    }
}

void parametros(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, "("))
    {
        metadata->token->is_consumed = true;

        lista_par(metadata);

        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);
        if (metadata->token->class == SYMBOL &&
            string_equals_literal(metadata->token->value, ")"))
        {
            metadata->token->is_consumed = true;

            return;
        }
        else
        {
            error_happened = true;
            metadata->tip = "Did you miss a \")\" in the procedure declaration?";
            throw_syntactic_error("Invalid symbol in procedure declaration", ")", SYN_ERROR,
                                  metadata);
        }
    }
    // Ɛ

    if (error_happened)
    {
        // NEXT(parametros) = {begin, var}

        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               (!(metadata->token->class == KEYWORD &&
                  string_equals_literal(metadata->token->value, "begin")) &&
                !(metadata->token->class == KEYWORD &&
                  string_equals_literal(metadata->token->value, "var"))))
        {
            metadata->token->is_consumed = true;
        }
    }
}

void mais_par(source_file_metadata_t *const metadata)
{
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, ";"))
    {
        metadata->token->is_consumed = true;

        lista_par(metadata);

        return;
    }
    // Ɛ
}

void lista_par(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    variaveis(metadata);

    if (!st_add_vars(metadata->st))
    {
        throw_semantic_error(ST_VAR_ALREADY_DECLARED, metadata);
    }

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, ":"))
    {
        metadata->token->is_consumed = true;

        tipo_var(metadata);

        st_proc_add_params(metadata->st);

        analysis_queue_destroy(&metadata->st->analysis_queue);

        mais_par(metadata);

        return;
    }
    else
    {
        error_happened = true;
        metadata->tip = "Did you miss a \":\" in the parameter declaration?";
        throw_syntactic_error("Invalid symbol in procedure declaration parameters", ":", SYN_ERROR,
                              metadata);
    }

    if (error_happened)
    {
        // NEXT(lista_par) = {)}

        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               !(metadata->token->class == SYMBOL &&
                 string_equals_literal(metadata->token->value, ")")))
        {
            metadata->token->is_consumed = true;
        }
    }
}

void corpo_p(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    dc_loc(metadata);

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == KEYWORD &&
        string_equals_literal(metadata->token->value, "begin"))
    {
        metadata->token->is_consumed = true;

        comandos(metadata);

        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);
        if (metadata->token->class == KEYWORD &&
            string_equals_literal(metadata->token->value, "end"))
        {
            metadata->token->is_consumed = true;
            return;
        }
        else
        {
            error_happened = true;
            metadata->tip = "Did you miss an \"end\" in the procedure body?";
            throw_syntactic_error("Invalid keyword in procedure definition", "end", SYN_ERROR,
                                  metadata);
        }
    }
    else
    {
        error_happened = true;
        metadata->tip = "Did you miss an \"begin\" in the procedure body?";
        throw_syntactic_error("Invalid keyword in procedure definition", "begin", SYN_ERROR,
                              metadata);
    }

    if (error_happened)
    {
        // NEXT(corpo_p) = {}

        throw_exception(SYN_ERROR);
    }
}

void dc_loc(source_file_metadata_t *const metadata)
{
    // lookahead
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == KEYWORD &&
        string_equals_literal(metadata->token->value, "var"))
    {
        dc_v(metadata);
        mais_dcloc(metadata);
    }
    // Ɛ
}

void mais_dcloc(source_file_metadata_t *const metadata)
{
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, ";"))
    {
        metadata->token->is_consumed = true;
        dc_loc(metadata);
    }
    // Ɛ
}

void comandos(source_file_metadata_t *const metadata)
{
    comando(metadata);
    mais_comandos(metadata);
}

void comando(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);

    if (metadata->token->class == KEYWORD &&
        string_equals_literal(metadata->token->value, "read"))
    {
        metadata->token->is_consumed = true;

        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);

        if (metadata->token->class == SYMBOL &&
            string_equals_literal(metadata->token->value, "("))
        {
            metadata->token->is_consumed = true;

            variaveis(metadata);

            exception_t possible_error = assert_types(metadata->st);
            if (possible_error != EMPTY_EXCEPTION)
            {
                throw_semantic_error(possible_error, metadata);
            }
            else // if success
            {
                gen_read_code(metadata->cl, metadata->st);
            }


            analysis_queue_destroy(&metadata->st->analysis_queue);

            get_next_token(metadata->file, metadata->token,
                           metadata->args->stop_on_error);
            if (metadata->token->class == SYMBOL &&
                string_equals_literal(metadata->token->value, ")"))
            {
                metadata->token->is_consumed = true;

                return;
            }
            else
            {
                error_happened = true;
                metadata->tip = "Did you miss a \")\" in the read command?";
                throw_syntactic_error("Invalid read command", ")", SYN_ERROR, metadata);
            }
        }
        else
        {
            error_happened = true;
            metadata->tip = "Did you miss a \"(\" in the read command?";
            throw_syntactic_error("Invalid read command", "(", SYN_ERROR, metadata);
        }
    }
    else if (metadata->token->class == KEYWORD &&
             string_equals_literal(metadata->token->value, "write"))
    {
        metadata->token->is_consumed = true;

        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);
        if (metadata->token->class == SYMBOL &&
            string_equals_literal(metadata->token->value, "("))
        {
            metadata->token->is_consumed = true;

            variaveis(metadata);

            exception_t possible_error = assert_types(metadata->st);
            if (possible_error != EMPTY_EXCEPTION)
            {
                throw_semantic_error(possible_error, metadata);
            }
            else // if success
            {
                gen_write_code(metadata->cl, metadata->st);
            }

            analysis_queue_destroy(&metadata->st->analysis_queue);

            get_next_token(metadata->file, metadata->token,
                           metadata->args->stop_on_error);
            if (metadata->token->class == SYMBOL &&
                string_equals_literal(metadata->token->value, ")"))
            {
                metadata->token->is_consumed = true;

                return;
            }
            else
            {
                error_happened = true;
                metadata->tip = "Did you miss a \")\" in the write command?";
                throw_syntactic_error("Invalid write command", ")", SYN_ERROR, metadata);
            }
        }
        else
        {
            error_happened = true;
            metadata->tip = "Did you miss a \"(\" in the write command?";
            throw_syntactic_error("Invalid write command", "(", SYN_ERROR, metadata);
        }
    }
    else if (metadata->token->class == KEYWORD &&
             string_equals_literal(metadata->token->value, "while"))
    {
        metadata->token->is_consumed = true;

        size_t return_to_exp = metadata->cl->count;

        condicao(metadata);

        code_t *template = gen_template_cond_jump_code(metadata->cl);

        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);
        if (metadata->token->class == KEYWORD &&
            string_equals_literal(metadata->token->value, "do"))
        {
            metadata->token->is_consumed = true;

            comandos(metadata);

            gen_while_code(metadata->cl, template, return_to_exp);

            get_next_token(metadata->file, metadata->token,
                           metadata->args->stop_on_error);
            if (metadata->token->class == SYMBOL &&
                string_equals_literal(metadata->token->value, "$"))
            {
                metadata->token->is_consumed = true;
                return;
            }
            else
            {
                error_happened = true;
                metadata->tip = "Did you miss a \"$\" in the while command?";
                throw_syntactic_error("Invalid while command", "$", SYN_ERROR, metadata);
            }
        }
        else
        {
            error_happened = true;
            metadata->tip = "Did you miss a \"do\" in the while command?";
            throw_syntactic_error("Invalid while command", "do", SYN_ERROR, metadata);
        }
    }
    else if (metadata->token->class == KEYWORD &&
             string_equals_literal(metadata->token->value, "if"))
    {
        metadata->token->is_consumed = true;

        condicao(metadata);

        code_t *template = gen_template_cond_jump_code(metadata->cl);

        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);
        if (metadata->token->class == KEYWORD &&
            string_equals_literal(metadata->token->value, "then"))
        {
            metadata->token->is_consumed = true;

            comandos(metadata);

            pfalsa(metadata, template);

            get_next_token(metadata->file, metadata->token,
                           metadata->args->stop_on_error);
            if (metadata->token->class == SYMBOL &&
                string_equals_literal(metadata->token->value, "$"))
            {
                metadata->token->is_consumed = true;

            }
            else
            {
                error_happened = true;
                metadata->tip = "Did you miss a \"$\" in the if command?";
                throw_syntactic_error("Invalid if command", "$", SYN_ERROR, metadata);
            }
        }
        else
        {
            error_happened = true;
            metadata->tip = "Did you miss a \"then\" in the if command?";
            throw_syntactic_error("Invalid if command", "then", SYN_ERROR, metadata);
        }
    }
    else if (metadata->token->class == IDENTIFIER)
    {
        metadata->token->is_consumed = true;

        analysis_queue_append(&metadata->st->analysis_queue, metadata->token->value, metadata->st->actual_scope);

        restoIdent(metadata);
    }
    else
    {
        error_happened = true;
        throw_syntactic_error("Invalid command", "read | write | while | if | A valid identifier, e.g. \"square_root\"",
                              SYN_ERROR, metadata);
    }

    if (error_happened)
    {
        // NEXT(comando) = {end}

        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               !(metadata->token->class == KEYWORD &&
                 string_equals_literal(metadata->token->value, "end")))
        {
            metadata->token->is_consumed = true;
        }
    }
}

void condicao(source_file_metadata_t *const metadata)
{
    expressao(metadata);

    exception_t possible_error = assert_types(metadata->st);
    if (possible_error != EMPTY_EXCEPTION)
    {
        throw_semantic_error(possible_error, metadata);
    }
    else // if success
    {
        expression_finish(metadata->cl, metadata->op_stack);
    }

    analysis_queue_destroy(&metadata->st->analysis_queue);

    instruction_t rel_op = relacao(metadata);

    expressao(metadata);

    possible_error = assert_types(metadata->st);
    if (possible_error != EMPTY_EXCEPTION)
    {
        throw_semantic_error(possible_error, metadata);
    }
    else if(rel_op != INVALID_INST) // if success
    {
        expression_finish(metadata->cl, metadata->op_stack);
        gen_rel_code(metadata->cl, rel_op);
    }

    analysis_queue_destroy(&metadata->st->analysis_queue);
}

void expressao(source_file_metadata_t *const metadata)
{
    termo(metadata);
    outros_termos(metadata);
}

void termo(source_file_metadata_t *const metadata)
{
    op_un(metadata);
    fator(metadata);
    mais_fatores(metadata);
}

void op_un(source_file_metadata_t *const metadata)
{
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);

    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, "+"))
    {
        metadata->token->is_consumed = true;
        return;
    }
    else if (metadata->token->class == SYMBOL &&
             string_equals_literal(metadata->token->value, "-"))
    {
        metadata->token->is_consumed = true;

        st_toggle_flag(metadata->st, CG_MINUS);

        return;
    }
    // Ɛ
}

void fator(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    bool has_unitary_minus = CHECK_FLAG(metadata->st, CG_MINUS);
    // Reset it to the possible inside expression use it
    if (has_unitary_minus)
    {
        st_toggle_flag(metadata->st, CG_MINUS);
    }

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);

    if (metadata->token->class == IDENTIFIER)
    {
        metadata->token->is_consumed = true;

        analysis_queue_append(&metadata->st->analysis_queue, metadata->token->value, metadata->st->actual_scope);

        expression_push_var(metadata->cl, metadata->st);
    }
    else if (metadata->token->class == INTEGER)
    {
        metadata->token->is_consumed = true;

        int_real_t e;
        if (!sscanf(metadata->token->value->buffer, "%zu", &e.integer))
        {
            throw_exception(LEX_INVALID_INT);
        }

        expression_push_int(metadata->cl, e);
    }
    else if (metadata->token->class == REAL)
    {
        metadata->token->is_consumed = true;

        int_real_t e;
        if (!sscanf(metadata->token->value->buffer, "%lf", &e.real))
        {
            throw_exception(LEX_INVALID_INT);
        }

        expression_push_real(metadata->cl, e);
    }
    else if (metadata->token->class == SYMBOL &&
             string_equals_literal(metadata->token->value, "("))
    {
        metadata->token->is_consumed = true;

        expression_push_op(metadata->cl, metadata->op_stack, '(');

        expressao(metadata);

        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);
        if (metadata->token->class == SYMBOL &&
            string_equals_literal(metadata->token->value, ")"))
        {
            metadata->token->is_consumed = true;

            expression_push_op(metadata->cl, metadata->op_stack, ')');
        }
        else
        {
            error_happened = true;
            metadata->tip = "Did you miss a \")\" in the factor?";
            throw_syntactic_error("Invalid factor", ")", SYN_ERROR, metadata);
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
                              metadata);
    }

    if (has_unitary_minus)
    {
        int_real_t e;
        e.integer = 0;
        gen_code(metadata->cl, INVE, e, INVALID_DATA_TYPE);
    }

    if (error_happened)
    {
        // NEXT(fator) = {$, ;, <, <=, <>, =, >, >=, do, else, end, then}

        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               (!(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, "$")) &&
                !(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, ";")) &&
                !(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, "<")) &&
                !(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, "<=")) &&
                !(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, "<>")) &&
                !(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, "=")) &&
                !(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, ">")) &&
                !(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, ">=")) &&
                !(metadata->token->class == KEYWORD &&
                  string_equals_literal(metadata->token->value, "do")) &&
                !(metadata->token->class == KEYWORD &&
                  string_equals_literal(metadata->token->value, "else")) &&
                !(metadata->token->class == KEYWORD &&
                  string_equals_literal(metadata->token->value, "end")) &&
                !(metadata->token->class == KEYWORD &&
                  string_equals_literal(metadata->token->value, "then"))))
        {
            metadata->token->is_consumed = true;
        }
    }
}

void mais_fatores(source_file_metadata_t *const metadata)
{
    // lookahead
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL &&
        (string_equals_literal(metadata->token->value, "*") ||
         string_equals_literal(metadata->token->value, "/")
        ))
    {
        op_mul(metadata);
        fator(metadata);
        mais_fatores(metadata);
    }
    // Ɛ
}

void op_mul(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);

    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, "*"))
    {
        metadata->token->is_consumed = true;

        expression_push_op(metadata->cl, metadata->op_stack, '*');

        return;
    }
    else if (metadata->token->class == SYMBOL &&
             string_equals_literal(metadata->token->value, "/"))
    {
        metadata->token->is_consumed = true;

        st_toggle_flag(metadata->st, ST_DIV_OP);

        expression_push_op(metadata->cl, metadata->op_stack, '/');

        return;
    }
    else
    {
        error_happened = true;
        throw_syntactic_error("Invalid operator", "* | /", SYN_ERROR, metadata);
    }

    if (error_happened)
    {
        // NEXT(op_mul) = {(, ident, numero_int, numero_real)}

        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               (!(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, "(")) &&
                metadata->token->class != IDENTIFIER &&
                metadata->token->class != INTEGER &&
                metadata->token->class != REAL))
        {
            metadata->token->is_consumed = true;
        }
    }
}


void outros_termos(source_file_metadata_t *const metadata)
{
    // lookahead
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL &&
        (string_equals_literal(metadata->token->value, "+") ||
         string_equals_literal(metadata->token->value, "-")
        ))
    {
        op_ad(metadata);
        termo(metadata);
        outros_termos(metadata);
    }
    // Ɛ
}

void op_ad(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);

    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, "+"))
    {
        metadata->token->is_consumed = true;

        expression_push_op(metadata->cl, metadata->op_stack, '+');

        return;
    }
    else if (metadata->token->class == SYMBOL &&
             string_equals_literal(metadata->token->value, "-"))
    {
        metadata->token->is_consumed = true;

        expression_push_op(metadata->cl, metadata->op_stack, '-');

        return;
    }
    else
    {
        error_happened = true;
        throw_syntactic_error("Invalid operator", "+ | -", SYN_ERROR, metadata);
    }

    if (error_happened)
    {
        // NEXT(op_ad) = {(, ident, numero_int, numero_real)}

        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               (!(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, "(")) &&
                metadata->token->class != IDENTIFIER &&
                metadata->token->class != INTEGER &&
                metadata->token->class != REAL))
        {
            metadata->token->is_consumed = true;
        }
    }
}

instruction_t relacao(source_file_metadata_t *const metadata)
{
    bool error_happened = false;
    instruction_t rel_op = INVALID_INST;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL)
    {
        if (string_equals_literal(metadata->token->value, "="))
        {
            rel_op = CPIG;
        }
        else if (string_equals_literal(metadata->token->value, "<>"))
        {
            rel_op = CDES;
        }
        else if (string_equals_literal(metadata->token->value, ">="))
        {
            rel_op = CMAI;
        }
        else if (string_equals_literal(metadata->token->value, "<="))
        {
            rel_op = CPMI;
        }
        else if (string_equals_literal(metadata->token->value, ">"))
        {
            rel_op = CPMA;
        }
        else if (string_equals_literal(metadata->token->value, "<"))
        {
            rel_op = CPME;
        }
        else
        {
            error_happened = true;
            throw_syntactic_error("Invalid relational operator", "= | <> | >= | <= | > | <", SYN_ERROR,
                                  metadata);
        }

        metadata->token->is_consumed = true;
    }
    else
    {
        error_happened = true;
        throw_syntactic_error("Invalid relational operator", "= | <> | >= | <= | > | <", SYN_ERROR,
                              metadata);
    }

    if (error_happened)
    {
        // NEXT(relacao) = {(, +, -, ident, numero_int, numero_real)}

        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               (!(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, "(")) &&
                !(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, "+")) &&
                !(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, "-")) &&
                metadata->token->class != IDENTIFIER &&
                metadata->token->class != INTEGER &&
                metadata->token->class != REAL))
        {
            metadata->token->is_consumed = true;
        }
    }

    return rel_op;
}

void pfalsa(source_file_metadata_t *const metadata, code_t *template)
{
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);

    if (metadata->token->class == KEYWORD &&
        string_equals_literal(metadata->token->value, "else"))
    {
        metadata->token->is_consumed = true;

        code_t *else_jump = gen_template_uncond_jump_code(metadata->cl);
        gen_if_code(metadata->cl, template);

        comandos(metadata);

        gen_else_code(metadata->cl, else_jump);
    }
    else
    {
        // Ɛ
        gen_if_code(metadata->cl, template);

    }

}

void restoIdent(source_file_metadata_t *const metadata)
{
    token_t *old_token = token_copy(metadata->token), *aux_token = NULL;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);

    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, ":="))
    {
        metadata->token->is_consumed = true;

        expressao(metadata);

        exception_t possible_error = assert_types(metadata->st);
        if (possible_error != EMPTY_EXCEPTION)
        {
            aux_token = metadata->token;
            metadata->token = old_token;
            throw_semantic_error(ST_ID_NOT_FOUND, metadata);
            metadata->token = aux_token;
        }
        else // if success
        {
            expression_finish(metadata->cl, metadata->op_stack);
            gen_assignment_code(metadata->cl, metadata->st);
        }

        analysis_queue_destroy(&metadata->st->analysis_queue);

        return;
    }
    else
    {
        if (!analysis_queue_set_scope(metadata->st))
        {
            aux_token = metadata->token;
            metadata->token = old_token;
            throw_semantic_error(ST_ID_NOT_FOUND, metadata);
            metadata->token = aux_token;
        }

        token_destroy(old_token);

        lista_arg(metadata);

        st_return_global_scope(metadata->st);
    }
}

void lista_arg(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, "("))
    {
        metadata->token->is_consumed = true;

        argumentos(metadata);

        if (!analysis_queue_assert_params(metadata->st))
        {
            throw_semantic_error(SEM_INVALID_ARGUMENTS, metadata);
        }

        get_next_token(metadata->file, metadata->token,
                       metadata->args->stop_on_error);
        if (metadata->token->class == SYMBOL &&
            string_equals_literal(metadata->token->value, ")"))
        {
            metadata->token->is_consumed = true;

            analysis_queue_destroy(&metadata->st->analysis_queue);

            return;
        }
        else
        {
            error_happened = true;
            metadata->tip = "Did you miss a \")\" at a procedure call?";
            throw_syntactic_error("Invalid symbol in procedure call", ")", SYN_ERROR, metadata);
        }
    }
    // Ɛ

    if (error_happened)
    {
        // NEXT(lista_arg) = {$, else, end, then}

        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               (!(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, "$")) &&
                !(metadata->token->class == KEYWORD &&
                  string_equals_literal(metadata->token->value, "else")) &&
                !(metadata->token->class == KEYWORD &&
                  string_equals_literal(metadata->token->value, "end")) &&
                !(metadata->token->class == KEYWORD &&
                  string_equals_literal(metadata->token->value, "then"))))
        {
            metadata->token->is_consumed = true;
        }
    }
}

void argumentos(source_file_metadata_t *const metadata)
{
    bool error_happened = false;

    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == IDENTIFIER)
    {
        metadata->token->is_consumed = true;

        analysis_queue_append(&metadata->st->analysis_queue, metadata->token->value, metadata->st->actual_scope);

        mais_ident(metadata);
        return;
    }
    else
    {
        error_happened = true;
        metadata->tip = "Did you miss an argument at a procedure call?";
        throw_syntactic_error("Invalid arguments in procedure call", ")", SYN_ERROR, metadata);
    }

    if (error_happened)
    {
        // NEXT(argumentos) = {)}

        while (get_next_token(metadata->file, metadata->token,
                              metadata->args->stop_on_error) &&
               (!(metadata->token->class == SYMBOL &&
                  string_equals_literal(metadata->token->value, ")"))))
        {
            metadata->token->is_consumed = true;
        }
    }
}

void mais_ident(source_file_metadata_t *const metadata)
{
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, ";"))
    {
        metadata->token->is_consumed = true;

        argumentos(metadata);
    }
    // Ɛ
}

void mais_comandos(source_file_metadata_t *const metadata)
{
    get_next_token(metadata->file, metadata->token, metadata->args->stop_on_error);
    if (metadata->token->class == SYMBOL && string_equals_literal(metadata->token->value, ";"))
    {
        metadata->token->is_consumed = true;

        comandos(metadata);
    }
    // Ɛ
}

void throw_semantic_error(exception_t const exception, source_file_metadata_t *const metadata)
{
    // MAYBE: Cache the actual line before enter a semantic / st method to assure that the line on the error is the first
    for (error_list_t *aux = metadata->st->error_list, *next = NULL; aux; aux = next)
    {
        next = aux->next;

        ++metadata->num_errors;
        printf("%zu: ", metadata->token->start_position.line + 1);
        log_with_color(RED, "SEMANTIC ERROR: ");
        printf("%s", aux->error);

        remove_elem_free_list((void *) aux->error, true);
        remove_elem_free_list((void *) aux, true);

        if (metadata->args->stop_on_error)
        {
            throw_exception(exception);
        }

    }

    metadata->st->error_list = NULL;
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
