//
// Created by adrianoii on 14/08/2021.
//

#ifndef GCC_SOURCE_FILE_H
#define GCC_SOURCE_FILE_H

#include "../lexical/lexical_token.h"
#include "../file_handler/file_handler.h"
#include "../cli/cli.h"
#include "../symbol_table/symbol_table.h"

// TODO: Maybe find a better name
typedef struct {
    st_t *st;
    token_t * token; // actual token
    file_t * file; // Source file to read the tokens
    cli_args_t  * args; // cli args
    size_t num_errors; // Number of errors found
    char  *tip; // Tip message
} source_file_metadata_t;

source_file_metadata_t *source_file_metadata_init(token_t *token, file_t *file, cli_args_t *args);

#endif //GCC_SOURCE_FILE_H
