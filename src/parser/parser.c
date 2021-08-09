//
// Created by adrianoii on 06/08/2021.
//

#include "parser.h"
#include <stdlib.h>
#include "../lexical/lexical_token.h"
#include "../lexical/lexical_analyzer.h"
#include "../syntactic/syntactic_analyzer.h"
#include "../logs/logs.h"

void parse_source_file(file_t  *const source_file, cli_args_t const *const args)
{
    token_t *token = token_init();

    if (args->stop_at == STOP_AT_LEX)
    {
        while (get_next_token(source_file, token, args->stop_on_error))
        {
            if (args->logs)
            {
                token_log(token);
            } else if (is_token_valid(token))
            {
                token_pretty_log(token);
            }
            token->is_consumed = true;
        }
    }
    else
    {   size_t num_errors = 0;
        start_syntactic_analysis(token, source_file, args, &num_errors);
        if (num_errors == 0)
        {
            log_with_color_nl(GRN, "Source file is syntactic correct!");
        }
        else
        {
            log_with_color(RED, "Invalid source file: ");
            printf("%zu", num_errors);
            log_with_color_nl(RED, " errors found.");
        }
    }

    token_destroy(token);
}
