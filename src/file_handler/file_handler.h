//
// Created by adrianoii on 24/07/2021.
//

#ifndef GCC_FILE_HANDLER_H
#define GCC_FILE_HANDLER_H

#include <stdio.h>
#include "../string/string.h"

typedef struct
{
    FILE *p_file;
    int actual_char;
    const char *path;
    size_t line;
    size_t col;
    bool is_fresh_line;
} file_t;

file_t *file_init(const char *file_path, char const *mode);

void file_destroy(file_t *file);

int file_get_next_char(file_t *file);

void file_rollback_byte(file_t *file);

size_t file_get_num_lines(file_t *file);

void file_log(file_t *file);

#endif //GCC_FILE_HANDLER_H
