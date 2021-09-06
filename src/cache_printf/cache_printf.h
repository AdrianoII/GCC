//
// Created by adrianoii on 29/08/2021.
//

#ifndef GCC_CACHE_PRINTF_H
#define GCC_CACHE_PRINTF_H

#include <stdarg.h>
#include <stdbool.h>

char const *cache_printf(char *fmt, ...);

char const *vcache_printf(char const *fmt, va_list args);

// MAYBE: use flags
char const *s_strcat(char const *s1, char const *s2, bool break_line, bool free_s1, bool free_s2);

#endif //GCC_CACHE_PRINTF_H
