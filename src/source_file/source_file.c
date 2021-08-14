//
// Created by adrianoii on 14/08/2021.
//

#include "source_file.h"
#include <stdlib.h>
#include <string.h>
#include "../exceptions/exceptions_handler.h"

source_file_metadata_t * source_file_metadata_init(token_t *token, file_t *file, cli_args_t const *args)
{
    source_file_metadata_t temp_metada = {token, file, args, 0, NULL};
    source_file_metadata_t * const metadata = calloc(1, sizeof(source_file_metadata_t*const));

    if (metadata == NULL)
    {
        throw_exception(ALLOCATION_FAILED);
    }

    // Hacky way to init const members
    memcpy(metadata, &temp_metada, sizeof(source_file_metadata_t));
    return metadata;
}

void source_file_metadata_destroy(source_file_metadata_t * const metadata)
{
    free(metadata);
}
