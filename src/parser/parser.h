//
// Created by adrianoii on 06/08/2021.
//

#ifndef GCC_PARSER_H
#define GCC_PARSER_H

#include "../cli/cli.h"
#include "stddef.h"
#include "../file_handler/file_handler.h"

void parse_source_file(file_t *source_file, cli_args_t const *args);

#endif //GCC_PARSER_H
