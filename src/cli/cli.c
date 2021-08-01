//
// Created by adrianoii on 24/07/2021.
//

#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../exceptions/exceptions_handler.h"
#include "../logs/logs.h"

bool str_equals(const char *s1, const char *s2);

cli_args_t *parse_args(size_t argc, char *argv[])
{
    cli_args_t *new_cli = calloc(1, sizeof(cli_args_t));

    if (new_cli == NULL)
    {
        throw_exception(ALLOCATION_FAILED);
    }

    new_cli->source_file_path = NULL;
    new_cli->playground = false;
    new_cli->logs = false;
    new_cli->stop_on_error = false;

    if (argc < 2)
    {
        log_with_color_nl(RED, "Please, provide, at least, a source file!");
        throw_exception(INVALID_CLI);
    }

    if (argv[1][0] != '-')
    {
        // Parse source file path
        new_cli->source_file_path = argv[1];
    }

    for (size_t i = new_cli->source_file_path == NULL ? 1 : 2; i < argc; i++)
    {
        if (str_equals(argv[i], "-h") || str_equals(argv[i], "--help"))
        {
            printf("Usage: gcc file [options] [...]\n");
            printf("Options:\n");
            printf("-h\t--help\tShow help information(this).\n");
            printf("-p\t--playground\tEnter playground function instead of the main flow.\n");
            printf("-l\t--logs\tEnable logs through the code.\n");
            printf("-se\t--stop-on-error\tEnable to stop de execution on first error.\n");
            exit(0);
        }

        if (str_equals(argv[i], "-p") || str_equals(argv[i], "--playground"))
        {
            new_cli->playground = true;
        }

        if (str_equals(argv[i], "-l") || str_equals(argv[i], "--logs"))
        {
            new_cli->logs = true;
        }

        if (str_equals(argv[i], "-se") || str_equals(argv[i], "--stop-on-error"))
        {
            new_cli->stop_on_error = true;
        }
    }

    return new_cli;
}

void cli_args_log(cli_args_t *args)
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
    printf("}\n");
}

void cli_destroy(cli_args_t *args)
{
    free(args);
}

bool str_equals(const char *s1, const char *s2)
{
    return strcmp(s1, s2) == 0;
}