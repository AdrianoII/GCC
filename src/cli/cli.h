//
// Created by adrianoii on 24/07/2021.
//

#ifndef GCC_CLI_H
#define GCC_CLI_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    NON_STOP,
    STOP_AT_LEX,
    STOP_AT_SYN,
    STOP_AT_SEM,
    STOP_AT_GEN,
} stop_parsing_at_t;

char const *stop_parsing_at_to_string(stop_parsing_at_t stop_at);

typedef struct {
    const char *source_file_path;
    const char *output_path;
    bool playground;
    bool logs;
    bool stop_on_error;
    bool interpreter_mode;
    stop_parsing_at_t stop_at;
} cli_args_t;


void cli_args_init(cli_args_t *args);

cli_args_t *parse_args(int argc, char *argv[]);

void cli_args_log(cli_args_t const *args);

void cli_args_destroy(cli_args_t *args);

#endif //GCC_CLI_H
