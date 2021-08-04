#include "main.h"
#include <stdio.h>
#include "file_handler/file_handler.h"
#include "cli/cli.h"
#include "lexical/lexical_token.h"
#include "lexical/lexical_analyzer.h"


void playground()
{
    printf("Entered at the playground!\n");
   // Test things here
}


void parse_source_file(file_t *source_file, cli_args_t *args)
{
    token_t *token = token_init();

    while (get_next_token(source_file, token, args->stop_on_error))
    {
        if (args->logs)
        {
            token_log(token);
        } else if (token->token_class != INVALID_TOKEN_CLASS)
        {
            token_pretty_log(token);
        }
        token_reset(token);
    }

    token_destroy(token);
}

int main(int argc, char *argv[])
{
    cli_args_t *args = parse_args(argc, argv);
    if (args->logs)
    {
        cli_args_log(args);
    }

    if (args->playground)
    {
        playground();
    }

    file_t *source_file = file_init(args->source_file_path);

    parse_source_file(source_file, args);

    file_destroy(source_file);
    cli_destroy(args);

    return 0;
}
