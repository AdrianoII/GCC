//
// Created by adrianoii on 18/09/2021.
//

#ifndef GCC_EXPRESSION_HANDLER_H
#define GCC_EXPRESSION_HANDLER_H

#include "../code_generation/code_generation.h"

// I am using the shunting-yard algorithm to convert infix expression to stack based code
// https://en.wikipedia.org/wiki/Shunting-yard_algorithm

// Operator stack used to convert an infix expression to postfix form
typedef struct operator_stack_elem_s {
    char operator;
    struct operator_stack_elem_s *next;
} operator_stack_elem_t;

typedef struct operator_stack_s {
    operator_stack_elem_t *top;
} operator_stack_t;

operator_stack_t* operator_stack_init();

void expression_push_var(code_list_t *cl, st_t *st);

void expression_push_int(code_list_t *cl, int_real_t n);

void expression_push_real(code_list_t *cl, int_real_t n);

void expression_push_op(code_list_t *cl, operator_stack_t *operator_stack, char op);

void expression_finish(code_list_t *cl, operator_stack_t *operator_stack);

#endif //GCC_EXPRESSION_HANDLER_H
