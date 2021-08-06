//
// Created by adrianoii on 24/07/2021.
//

#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../exceptions/exceptions_handler.h"
#include "../logs/logs.h"

bool str_equals(char const *s1, char const *s2);

// Handle non flag arguments
void handle_help();

void handle_stop_at(cli_args_t *args, char const *argument);
// Functions to check each arg
bool is_help(char const *s);

bool is_playground(char const *s);

bool is_logs(char const *s);

bool is_stop_on_error(char const *s);

bool is_stop_at(char const *s);

char const *stop_parsing_at_to_string(stop_parsing_at_t stop_at)
{
    const char *const stop_parsing_at_mapper[] = {
            "NON_STOP",
            "STOP_AT_LEX",
            "STOP_AT_SYN",
            "STOP_AT_SEM",
            "STOP_AT_GEN",
    };
    return stop_parsing_at_mapper[stop_at];
}

void cli_args_init(cli_args_t *const args)
{
    args->source_file_path = NULL;
    args->playground = false;
    args->logs = false;
    args->stop_on_error = false;
    args->stop_at = NON_STOP;
}

void handle_help()
{
    printf("Usage: gcc file [options] [...]\n");
    printf("Options:\n");
    printf("-h\t--help\tShow help information(this).\n");
    printf("-p\t--playground\tEnter playground function instead of the main flow.\n");
    printf("-l\t--logs\tEnable logs through the code.\n");
    printf("-se\t--stop-on-error\tEnable to stop the execution on first error.\n");
    printf("-sa p\t--stop-at p\tStop at the specific p part of the parsing process.\n");
    printf("\tp: l | lex | lexical | sy | syn | syntactic | se | sem | semantic | g | gen | code-generation\n");
    exit(0);
}

void handle_stop_at(cli_args_t *const args, char const *const argument)
{
    if (str_equals(argument, "l") || str_equals(argument, "lex") || str_equals(argument, "lexical"))
    {
        args->stop_at = STOP_AT_LEX;
    }
    else if (str_equals(argument, "sy") || str_equals(argument, "syn") || str_equals(argument, "syntactic"))
    {
        args->stop_at = STOP_AT_SYN;
    }
    else if (str_equals(argument, "se") || str_equals(argument, "sem") || str_equals(argument, "semantic"))
    {
        args->stop_at = STOP_AT_SEM;
    }
    else if (str_equals(argument, "g") || str_equals(argument, "gen")
             || str_equals(argument, "code-generation"))
    {
        args->stop_at = STOP_AT_GEN;
    }
    else
    {
        log_with_color_nl(RED, "Invalid p for stop-at argument!");
        throw_exception(INVALID_CLI);
    }
}

bool is_help(const char *const s)
{
    return str_equals(s, "-h") || str_equals(s, "--help");
}

bool is_playground(const char *const s)
{
    return str_equals(s, "-p") || str_equals(s, "--playground");
}

bool is_logs(const char *const s)
{
    return str_equals(s, "-l") || str_equals(s, "--logs");
}

bool is_stop_on_error(const char *s)
{
    return str_equals(s, "-se") || str_equals(s, "--stop-on-error");
}

bool is_stop_at(const char *const s)
{
    return str_equals(s, "-sa") || str_equals(s, "--stop-at");
}

cli_args_t *parse_args(int argc, char *argv[])
{
    // Check if the user provide something
    if (argc < 2)
    {
        log_with_color_nl(RED, "Please, provide, at least, a source file!");
        throw_exception(INVALID_CLI);
    }

    cli_args_t *new_cli = calloc(1, sizeof(cli_args_t));

    if (new_cli == NULL)
    {
        throw_exception(ALLOCATION_FAILED);
    }

    cli_args_init(new_cli);

    // First arg should be a path to a file or the help arg
    if (argv[1][0] != '-')
    {
        // Parse source file path
        new_cli->source_file_path = argv[1];
    }
    else if (is_help(argv[1]))
    {
        handle_help();
    }
    else
    {
        log_with_color_nl(RED, "Invalid arguments, please, use the help!");
        throw_exception(INVALID_CLI);
    }

    // Starting after the file path
    for (int i = 2; i < argc; i++)
    {
        if (is_help(argv[i]))
        {
            handle_help();
        }
        else if (is_playground(argv[i]))
        {
            new_cli->playground = true;
        }
        else if (is_logs(argv[i]))
        {
            new_cli->logs = true;
        }
        else if (is_stop_on_error(argv[i]))
        {
            new_cli->stop_on_error = true;
        }
        else if (is_stop_at(argv[i]))
        {
            // Assert that p exists
            if (i + 1 >= argc)
            {
                log_with_color_nl(RED, "The stop-at argument need an p like value!");
                throw_exception(INVALID_CLI);
            }

            // Look the next arg, that should be the p
            handle_stop_at(new_cli, argv[++i]);
        }
    }

    return new_cli;
}

void cli_args_log(cli_args_t const *const args)
{
    printf("ARGS:\n{\n");
    if (args->source_file_path == NULL)
    {
        printf("\tsource_file_path: null,\n");
    }
    else
    {
        printf("\tsource_file_path: %s,\n", args->source_file_path);
    }
    printf("\tplayground: %s,\n", args->playground ? "true" : "false");
    printf("\tlogs: %s,\n", args->logs ? "true" : "false");
    printf("\tstop_on_error: %s,\n", args->stop_on_error ? "true" : "false");
    printf("\tstop_at: %s,\n", args->stop_at ? "true" : "false");
    printf("\tstop_at: %s\n", stop_parsing_at_to_string(args->stop_at));
    printf("}\n");
}

bool str_equals(const char *const s1, const char *const s2)
{
    return strcmp(s1, s2) == 0;
}

void cli_args_destroy(cli_args_t *args)
{
    free(args);
}
