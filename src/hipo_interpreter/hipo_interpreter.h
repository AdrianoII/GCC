//
// Created by adrianoii on 19/09/2021.
//

#ifndef GCC_HIPO_INTREPRETER_H
#define GCC_HIPO_INTREPRETER_H

#include <stdint.h>
#include "../file_handler/file_handler.h"
#include "../code_generation/code_generation.h"

typedef struct code_array_s {
    code_t *buffer;
    size_t count;
    size_t actual;
} code_array_t;

typedef struct data_stack_elem_s
{
    int_real_t data;
    data_type_t type;
} data_stack_elem_t;

typedef struct data_stack_s {
    data_stack_elem_t *buffer;
    size_t count;
    size_t actual;
    size_t top;
} data_stack_t;

typedef struct offset_queue_elem_s
{
    size_t pos;
    struct offset_queue_elem_s *next;
} offset_queue_elem_t;

typedef struct offset_queue_s
{
    offset_queue_elem_t *global;
    offset_queue_elem_t *actual;
    size_t count;
} offset_queue_t;

void try_interpret(file_t *source);

#endif //GCC_HIPO_INTREPRETER_H
