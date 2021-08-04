//
// Created by adrianoii on 04/08/2021.
//

#ifndef GCC_MAIN_H
#define GCC_MAIN_H

#include "file_handler/file_handler.h"
#include "cli/cli.h"

void playground();
void parse_source_file(file_t *source_file, cli_args_t *args);

#endif //GCC_MAIN_H
