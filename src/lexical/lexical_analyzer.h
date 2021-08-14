//
// Created by adrianoii on 24/07/2021.
//

#ifndef GCC_LEXICAL_ANALYZER_H
#define GCC_LEXICAL_ANALYZER_H

#include "lexical_token.h"
#include "../file_handler/file_handler.h"

bool get_next_token(file_t *source_file, token_t *token, bool stop_on_error);

//void consume_token_until_equals(file_t *source_file, token_t *token, bool stop_on_error, )
#endif //GCC_LEXICAL_ANALYZER_H
