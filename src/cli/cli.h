//
// Created by adrianoii on 24/07/2021.
//

#ifndef GCC_CLI_H
#define GCC_CLI_H

#include <stdbool.h>
#include <stddef.h>

typedef struct
{
    const char *source_file_path;
    bool playground;
    bool logs;
    bool stop_on_error;
} cli_args_t;

cli_args_t *parse_args(int argc, char *argv[]);

void cli_args_log(cli_args_t *args);

void cli_destroy(cli_args_t *args);

#endif //GCC_CLI_H
