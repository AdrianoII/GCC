#include "main.h"
#include <stdio.h>
#include "file_handler/file_handler.h"
#include "cli/cli.h"
#include "parser/parser.h"


void playground(void)
{
    printf("Entered at the playground!\n");
   // Test things here
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
    cli_args_destroy(args);

    return 0;
}
