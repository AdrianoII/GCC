//
// Created by adrianoii on 06/08/2021.
//

#ifndef GCC_SYNTACTIC_ANALYZER_H
#define GCC_SYNTACTIC_ANALYZER_H

#include <stdbool.h>
#include "../lexical/lexical_token.h"
#include "../cli/cli.h"

void start_syntactic_analysis(token_t *token, file_t *file, cli_args_t const *args, size_t *num_errors);


#endif //GCC_SYNTACTIC_ANALYZER_H
