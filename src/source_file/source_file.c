//
// Created by adrianoii on 14/08/2021.
//

#include "source_file.h"
#include <stdlib.h>
#include "../s_mem_alloc/s_mem_alloc.h"

source_file_metadata_t * source_file_metadata_init(token_t *token, file_t *file, cli_args_t *args)
{
    // TODO: FIX CONST HERE
    source_file_metadata_t * metadata = s_mem_alloc(1, sizeof(source_file_metadata_t));

    metadata->token = token;
    metadata->file = file;
    metadata->args = args;
    metadata->num_errors = 0;
    metadata->args = args;
    metadata->tip = NULL;
    metadata->st = st_init();
    metadata->cl = cs_init();
    metadata->op_stack = operator_stack_init();

    return metadata;
}
