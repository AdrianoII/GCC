#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "s_mem_alloc/s_mem_alloc.h"
#include "exceptions/exceptions_handler.h"
#include "file_handler/file_handler.h"
#include "cli/cli.h"
#include "parser/parser.h"


void playground(void)
{
    printf("Entered at the playground!\n");
    string_t *stst = string_init();
    string_append_char(stst, '0');
    string_append_char(stst, '1');
    string_append_char(stst, '2');
    string_append_char(stst, '3');
    string_append_char(stst, '4');
    string_append_char(stst, '5');
    string_append_char(stst, '6');
    string_append_char(stst, '7');
    string_append_char(stst, '8');
    string_append_char(stst, '9');
    string_append_char(stst, '0');
    string_log(stst, 0);
    string_realloc_to_n(stst, 3);
    string_log(stst, 0);
    // Test things here
}

int main(int argc, char *argv[])
{

    if (sizeof(double) * CHAR_BIT != 64)
    {
        throw_exception(INVALID_FLOAT_ARCH);
    }

    if (sizeof(size_t) * CHAR_BIT < 64)
    {
        throw_exception(INVALID_SIZE_T_ARCH);
    }

    {

        if (atexit(free_mem) != 0)
        {
            printf("Fail to bind free_mem to atexit!");
            throw_exception(FAILED_AT_EXIT);
        }
    }

    cli_args_t *args = parse_args(argc, argv);

    if (args->logs)
    {
        cli_args_log(args);
    }

    if (args->playground)
    {
        playground();
        return 0;
    }

    file_t *source_file = file_init(args->source_file_path, "r");

    parse_source_file(source_file, args);

    return 0;
}
