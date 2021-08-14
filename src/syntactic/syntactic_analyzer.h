//
// Created by adrianoii on 06/08/2021.
//

#ifndef GCC_SYNTACTIC_ANALYZER_H
#define GCC_SYNTACTIC_ANALYZER_H

#include <stdbool.h>
#include "../lexical/lexical_token.h"
#include "../cli/cli.h"
#include "../source_file/source_file.h"

void start_syntactic_analysis(source_file_metadata_t * source_file_metadata);


#endif //GCC_SYNTACTIC_ANALYZER_H
