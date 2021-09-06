//
// Created by adrianoii on 29/08/2021.
//

#include "cache_printf.h"
#include <string.h>
#include <stdio.h>
#include "../s_mem_alloc/s_mem_alloc.h"

// FIXME: Convert this to use the string module

char const *cache_printf(char *fmt, ...)
{
    va_list args, args_copy;
    // FIXME: Fix buffer size precision
    size_t buffer_size = strlen(fmt);

    va_start(args, fmt);
    va_copy(args_copy, args);

    char *aux = fmt;

    while (*aux != '\0')
    {
        if (*aux == '%')
        {
            ++aux;
            if (*aux == 'z')
            {
                ++aux;
                if (*aux == 'u')
                {
                    va_arg(args, size_t);
                    buffer_size += 20;
                }
            }
            else if (*aux == 's')
            {
                buffer_size += strlen(va_arg(args, char*));
            }
        }
        ++aux;

    }

    char *buffer = s_mem_alloc(buffer_size, sizeof (char));

    vsnprintf(buffer, buffer_size, fmt, args_copy);

    return buffer;
}

char const *vcache_printf(char const *fmt, va_list args)
{
    va_list args_copy;
    // FIXME: Fix buffer size precision
    size_t buffer_size = strlen(fmt) + 1;

    va_copy(args_copy, args);

    char const *aux = fmt;

    while (*aux != '\0')
    {
        if (*aux == '%')
        {
            ++aux;
            if (*aux == 'z')
            {
                ++aux;
                if (*aux == 'u')
                {
                    va_arg(args, size_t);
                    buffer_size += 20;
                }
            }
            else if (*aux == 's')
            {
                buffer_size += strlen(va_arg(args, char*));
            }
        }
        ++aux;
    }

    char *buffer = s_mem_alloc(buffer_size, sizeof (char));

    vsnprintf(buffer, buffer_size, fmt, args_copy);

    return buffer;
}

char const *s_strcat(char const * const s1, char const * const s2, bool const break_line, bool free_s1, bool free_s2)
{
    size_t new_buffer_size = strlen(s1) + strlen(s2) +1 + (break_line ? 1 : 0);
    char *new_buffer = s_mem_alloc(new_buffer_size, sizeof(char));

    strcat(new_buffer, s1);

    if (break_line)
    {
        strcat(new_buffer, "\n");
    }

    strcat(new_buffer, s2);

    if (free_s1)
    {
        remove_elem_free_list((void*)s1, true);
    }

    if (free_s2)
    {
        remove_elem_free_list((void*)s2, true);
    }

    return new_buffer;
}
