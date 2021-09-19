//
// Created by adrianoii on 23/08/2021.
//

#include "s_mem_alloc.h"
#include <stdlib.h>
#include <string.h>
#include "../exceptions/exceptions_handler.h"

typedef struct to_free_list_s {
    void *ptr;
    bool is_file;
    struct to_free_list_s *next;
} to_free_list_t;

void to_free_list_append(void *ptr, bool is_file);

static to_free_list_t *to_free_list = NULL;

void remove_elem_free_list(void *e, bool free_memory)
{
    to_free_list_t *aux = NULL, *aux_next = NULL;

    if (!to_free_list)
    {
        return;
    }

    if (to_free_list->ptr == e)
    {
        aux = to_free_list;

        if (to_free_list->next == NULL)
        {
            to_free_list = to_free_list->next;
        }

        if (free_memory)
        {
            free(aux->ptr);
        }

        free(aux);
        return;
    }

    if (to_free_list->next == NULL)
    {
        return;
    }

    for (aux = to_free_list, aux_next = to_free_list->next; aux_next != NULL; aux = aux_next, aux_next = aux_next->next)
    {
        if (aux_next->ptr == e)
        {
            aux->next = aux_next->next;

            if (free_memory)
            {
                if(aux_next->is_file)
                {
                    fclose(aux_next->ptr);
                }
                else
                {
                    free(aux_next->ptr);
                }
            }

            free(aux_next);

            return;
        }
    }
}


void to_free_list_append(void *ptr, bool is_file)
{
    to_free_list_t *new = calloc(1, sizeof(to_free_list_t));

    if (!new)
    {
        throw_exception(ALLOCATION_FAILED);
    }

    new->ptr = ptr;
    new->next = NULL;
    new->is_file = is_file;

    if (!to_free_list)
    {
        to_free_list = new;
        return;
    }

    for (to_free_list_t *aux = to_free_list; aux != NULL; aux = aux->next)
    {
        if (aux->next == NULL)
        {
            aux->next = new;
            return;
        }
    }
}


void *s_mem_alloc(size_t num_e, size_t size)
{
    void *mem = calloc(num_e, size);

    // mem == NULL
    if (!mem)
    {
        throw_exception(ALLOCATION_FAILED);
    }

    to_free_list_append(mem, false);

    return mem;
}

void *s_mem_realloc(void *ptr, size_t old_size, size_t new_size)
{
    void *new_buffer = realloc(ptr, new_size);

    if (new_buffer == NULL)
    {
        throw_exception(ALLOCATION_FAILED);
    }

    if (new_buffer != ptr)
    {
        // realloc already free the old buffer
        remove_elem_free_list(ptr, false);
        to_free_list_append(new_buffer, false);
    }

    if (new_size > old_size)
    {
        size_t diff_buffer_size = new_size - old_size;
        // Void point arithmetic is illegal
        void *p_diff_buffer_start = ((char *) new_buffer) + old_size;
        memset(p_diff_buffer_start, '\0', diff_buffer_size);
    }

    return new_buffer;
}

FILE *s_fopen(char const *path, char const *mode)
{
    FILE *file = fopen(path, mode);
    if (file == NULL)
    {
        throw_exception(INVALID_FILE);
    }

    to_free_list_append(file, true);

    return file;
}

void free_mem(void)
{
    for (to_free_list_t *aux = NULL; to_free_list != NULL; to_free_list = aux)
    {
        aux = to_free_list->next;
        if (to_free_list->is_file)
        {
            fclose(to_free_list->ptr);
        }else
        {
            free(to_free_list->ptr);
        }
        free(to_free_list);
    }
}


