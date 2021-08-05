//
// Created by adrianoii on 23/07/2021.
//

#include "logs.h"
#include <stdio.h>

void log_with_color(const char *color, const char *msg)
{
    printf("%s%s%s", color, msg, RESET);
}

void log_with_color_nl(const char *color, const char *msg)
{
    printf("%s%s%s\n", color, msg, RESET);
}
