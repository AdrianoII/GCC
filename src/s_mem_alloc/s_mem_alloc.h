//
// Created by adrianoii on 23/08/2021.
//

#ifndef GCC_S_MEM_ALLOC_H
#define GCC_S_MEM_ALLOC_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

// FIXME: Find better name

void *s_mem_alloc(size_t num_e, size_t size);

void *s_mem_realloc(void *ptr, size_t old_size, size_t new_size);

FILE *s_fopen(char const *path, char const *mode);

void remove_elem_free_list(void *e, bool free_memory);

void free_mem(void);

#endif //GCC_S_MEM_ALLOC_H
