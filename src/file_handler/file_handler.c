//
// Created by adrianoii on 24/07/2021.
//
#include "file_handler.h"
#include "../logs/logs.h"
#include "../exceptions/exceptions_handler.h"
#include "../s_mem_alloc/s_mem_alloc.h"

file_t *file_init(char const  *file_path, char const * const mode)
{
    file_t *new_file = s_mem_alloc(1, sizeof(file_t));

    new_file->path = file_path;
    new_file->line = 0;
    new_file->col = 0;
    new_file->is_fresh_line = false;

    new_file->p_file = s_fopen(new_file->path, mode);

    return new_file;
}

int file_get_next_char(file_t *file)
{
    file->actual_char = fgetc(file->p_file);

    if (!is_valid_char_value(file->actual_char))
    {
        log_with_color_nl(RED, "INVALID CHARACTER VALUE!");
        printf("FILE: %s\nCHARACTER VALUE: %d at %zu:%zu\n", file->path, file->actual_char, file->line,
               file->col + 1);
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
    }
    else
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
        size_t old_col = 0;
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
                // Edge case when the first byte of the file is \n
                if ((file->actual_char = fgetc(file->p_file)) != '\n')
                {
                    fseek(file->p_file, -1, SEEK_CUR);
                }

                ++old_col;
                break;
            }
        }

        while ((file->actual_char = fgetc(file->p_file)) != '\n')
        {}

        fseek(file->p_file, -1, SEEK_CUR);

        --file->line;
        file->col = old_col;
    }
    else
    {
        --file->col;
    }

}

size_t file_get_num_lines(file_t *file)
{
    size_t lines = 1;
    int c = 0;
    while(!feof(file->p_file))
    {
        c = fgetc(file->p_file);
        if(c == '\n')
        {
            lines++;
        }
    }

    rewind(file->p_file);

    return lines;
}

// FIXME: dasdasds
void file_log(file_t *file)
{
    printf("file{\n");
    printf("path %s\n", file->path);
    printf("line %zu\n", file->line);
    printf("col %zu\n", file->col);
    printf("is_fresh_line %d\n", file->is_fresh_line);
    printf("actual_char %c\n", file->actual_char);
}
