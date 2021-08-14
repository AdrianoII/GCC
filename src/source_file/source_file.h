//
// Created by adrianoii on 14/08/2021.
//

#ifndef GCC_SOURCE_FILE_H
#define GCC_SOURCE_FILE_H

#include "../lexical/lexical_token.h"
#include "../file_handler/file_handler.h"
#include "../cli/cli.h"

// TODO: Maybe find a better name
typedef struct {
    token_t *const token; // actual token
    file_t *const file; // Source file to read the tokens
    cli_args_t const *const args; // cli args
    size_t num_errors; // Number of errors found
    char const *tip; // Tip message
} source_file_metadata_t;

source_file_metadata_t *source_file_metadata_init(token_t *token, file_t *file, cli_args_t const *args);

void source_file_metadata_destroy(source_file_metadata_t *metadata);

#endif //GCC_SOURCE_FILE_H
