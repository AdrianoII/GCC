//
// Created by adrianoii on 18/09/2021.
//

#include "expression_handler.h"
#include "../s_mem_alloc/s_mem_alloc.h"

instruction_t instruction_from_operand(char const op)
{
    if (op == '+')
    {
        return SOMA;
    } else if (op == '-')
    {
        return SUBT;
    } else if (op == '*')
    {
        return MULT;
    } else //if (op == '/')
    {
        return DIVI;
    }
}

operator_stack_elem_t *operator_stack_elem_t_init()
{
    operator_stack_elem_t *e = s_mem_alloc(1, sizeof(operator_stack_elem_t));
    return e;
}

operator_stack_t *operator_stack_init()
{
    operator_stack_t *os = s_mem_alloc(1, sizeof(operator_stack_t));
    return os;
}

void operator_stack_push(operator_stack_t *os, char op)
{
    operator_stack_elem_t *operator = operator_stack_elem_t_init();
    operator->operator = op;

    if (!os->top)
    {
        os->top = operator;
    }
    else
    {
        operator->next = os->top;
        os->top = operator;
    }
}

void expression_push_var(code_list_t *cl, st_t *st, to_fix_list_t **tf)
{
    if (analysis_queue_fetch_vars_entries(st))
    {
        analysis_queue_t *aux = st->analysis_queue;
        int_real_t e;
        for (; aux->next; aux = aux->next)
        {}

        e.integer = ((var_st_elem_t *) aux->elem)->mem_pos;
        gen_code(cl, CRVL, e, INTEGER_DATA_TYPE);
        if (aux->scope != GLOBAL_SCOPE)
        {
            to_fix_append(tf, cl->last);
        }
    }
}

void expression_push_int(code_list_t *cl, int_real_t n)
{
    gen_code(cl, CRCT, n, INTEGER_DATA_TYPE);
}

void expression_push_real(code_list_t *cl, int_real_t n)
{
    gen_code(cl, CRCT, n, REAL_DATA_TYPE);
}

void expression_push_op(code_list_t *cl, operator_stack_t *operator_stack, char op)
{
    operator_stack_elem_t *aux = operator_stack->top;
    int_real_t e;
    e.integer = 0;

    if (op == ')')
    {
        while (aux->operator != '(')
        {
            gen_code(cl, instruction_from_operand(aux->operator), e, INVALID_DATA_TYPE);
            operator_stack->top = operator_stack->top->next;
            remove_elem_free_list(aux, true);
            aux = operator_stack->top;
        }
        operator_stack->top = operator_stack->top->next;
        remove_elem_free_list(aux, true);
    }
    else
    {
        if(operator_stack->top && operator_stack->top->operator != '(' && op != '(')
        {
            bool new_is_md = op == '*' || op == '/';
            bool top_is_md = operator_stack->top->operator == '*' || operator_stack->top->operator == '/';

            // While the top of the stack has greater precedence
            while (top_is_md > new_is_md)
            {
                gen_code(cl, instruction_from_operand(aux->operator), e, INVALID_DATA_TYPE);
                operator_stack->top = operator_stack->top->next;
                remove_elem_free_list(aux, true);
                aux = operator_stack->top;
                top_is_md = aux->operator == '*' || aux->operator == '/';
            }
        }

        operator_stack_push(operator_stack, op);
    }
}

void expression_finish(code_list_t *cl, operator_stack_t *operator_stack)
{
    operator_stack_elem_t *aux = operator_stack->top;
    int_real_t e;
    e.integer = 0;

    while(aux)
    {
        gen_code(cl, instruction_from_operand(aux->operator), e, INVALID_DATA_TYPE);
        operator_stack->top = operator_stack->top->next;
        remove_elem_free_list(aux, true);
        aux = operator_stack->top;
    }
}
