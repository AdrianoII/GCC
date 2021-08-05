//
// Created by adrianoii on 24/07/2021.
//
#include "file_handler.h"
#include <stdlib.h>
#include "../exceptions/exceptions_handler.h"
#include "../logs/logs.h"

file_t *file_init(const char *file_path)
{
    file_t *new_file = calloc(1, sizeof(file_t));

    if (new_file == NULL)
    {
        throw_exception(ALLOCATION_FAILED);
    }

    new_file->path = file_path;
    new_file->line = 0;
    new_file->col = 0;
    new_file->is_fresh_line = false;

    new_file->p_file = fopen(new_file->path, "r");
    if (new_file->p_file == NULL)
    {
        throw_exception(INVALID_FILE);
    }

    return new_file;
}

void file_destroy(file_t *file)
{
    fclose(file->p_file);
    free(file);
}

int file_get_next_char(file_t *file)
{
    file->actual_char = fgetc(file->p_file);

    if (!is_valid_char_value(file->actual_char))
    {
        log_with_color_nl(RED, "INVALID CHARACTER VALUE!");
        printf("FILE: %s\nCHARACTER VALUE: %d\n", file->path, file->actual_char);
        throw_exception(INVALID_CHAR_FROM_FILE);
    }

    if (file->actual_char == '\n')
    {
        ++file->line;
        file->col = 0;
//        file->is_fresh_line = true;
    }
    else if (file->is_fresh_line)
    {
        file->is_fresh_line = false;
    } else
    {
        ++file->col;
    }

    return file->actual_char;
}

void file_rollback_byte(file_t *file)
{
    if (file->line == 0 && file->col == 0)
    {
        throw_exception(INVALID_FILE_ROLLBACK_CHAR);
    }
    fseek(file->p_file, -1, SEEK_CUR); // "abc\n"
    if (file->actual_char == '\n')
    {
        long int old_col = 0;
        fseek(file->p_file, -1, SEEK_CUR);

        while ((file->actual_char = fgetc(file->p_file)) != '\n')
        {
            ++old_col;
            // try to roll back the cursor 2 bytes
            for (size_t i = 0; (ftell(file->p_file) != 0) && i < 2; i++)
            {
                fseek(file->p_file, -1, SEEK_CUR);
            }

            if (ftell(file->p_file) == 0)
            {
                ++old_col;
                break;
            }
        }

        while ((file->actual_char = fgetc(file->p_file)) != '\n');

        fseek(file->p_file, -1, SEEK_CUR);

        --file->line;
        file->col = old_col;
    } else
    {
        --file->col;
    }

}
