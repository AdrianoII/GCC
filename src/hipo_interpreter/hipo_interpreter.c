//
// Created by adrianoii on 19/09/2021.
//

#include "hipo_interpreter.h"
#include <stdlib.h>
#include <stdio.h>

#include "../s_mem_alloc/s_mem_alloc.h"
#include "../exceptions/exceptions_handler.h"
#include "../logs/logs.h"

instruction_t instruction_from_string(char *instruction)
{
    if (literal_equals(instruction, "CRCT"))
    {
        return CRCT;
    }
    else if (literal_equals(instruction, "CRVL"))
    {
        return CRVL;
    }
    else if (literal_equals(instruction, "SOMA"))
    {
        return SOMA;
    }
    else if (literal_equals(instruction, "SUBT"))
    {
        return SUBT;
    }
    else if (literal_equals(instruction, "MULT"))
    {
        return MULT;
    }
    else if (literal_equals(instruction, "DIVI"))
    {
        return DIVI;
    }
    else if (literal_equals(instruction, "INVE"))
    {
        return INVE;
    }
    else if (literal_equals(instruction, "CONJ"))
    {
        return CONJ;
    }
    else if (literal_equals(instruction, "DISJ"))
    {
        return DISJ;
    }
    else if (literal_equals(instruction, "NEGA"))
    {
        return NEGA;
    }
    else if (literal_equals(instruction, "CPME"))
    {
        return CPME;
    }
    else if (literal_equals(instruction, "CPMA"))
    {
        return CPMA;
    }
    else if (literal_equals(instruction, "CPIG"))
    {
        return CPIG;
    }
    else if (literal_equals(instruction, "CDES"))
    {
        return CDES;
    }
    else if (literal_equals(instruction, "CPMI"))
    {
        return CPMI;
    }
    else if (literal_equals(instruction, "CMAI"))
    {
        return CMAI;
    }
    else if (literal_equals(instruction, "ARMZ"))
    {
        return ARMZ;
    }
    else if (literal_equals(instruction, "DSVI"))
    {
        return DSVI;
    }
    else if (literal_equals(instruction, "DSVF"))
    {
        return DSVF;
    }
    else if (literal_equals(instruction, "LEIT"))
    {
        return LEIT;
    }
    else if (literal_equals(instruction, "IMPR"))
    {
        return IMPR;
    }
    else if (literal_equals(instruction, "ALME"))
    {
        return ALME;
    }
    else if (literal_equals(instruction, "INPP"))
    {
        return INPP;
    }
    else if (literal_equals(instruction, "PARA"))
    {
        return PARA;
    }
    else if (literal_equals(instruction, "PARAM"))
    {
        return PARAM;
    }
    else if (literal_equals(instruction, "PUSHER"))
    {
        return PUSHER;
    }
    else if (literal_equals(instruction, "CHPR"))
    {
        return CHPR;
    }
    else if (literal_equals(instruction, "DESM"))
    {
        return DESM;
    }
    else if (literal_equals(instruction, "RTPR"))
    {
        return RTPR;
    }
    else
    {
        log_with_color(RED, "INVALID INSTRUCTION ");
        printf("%s", instruction);
        log_with_color(RED, "!\n");
        throw_exception(CG_INVALID_INSTRUCTION);
    }
    return INVALID_INST;
}

bool contains_char(char *s, char c)
{
    size_t i = 0;
    for (; s[i] != '\0'; i++)
    {
        if (s[i] == c)
        {
            return true;
        }
    }
    return false;
}

void code_from_file(file_t *source, code_array_t *codes)
{
    codes->count = file_get_num_lines(source);
    codes->actual = 0;
    codes->buffer = s_mem_alloc(codes->count, sizeof(code_t));

    char inst_name[13] = {'\0'}, const_buffer[65] = {'\0'};
    code_t *code;
    instruction_t inst;
    while (fscanf(source->p_file, "%s", inst_name) != EOF)
    {
        code = &codes->buffer[codes->actual];
        code->instruction = instruction_from_string(inst_name);
        inst = code->instruction;
        code->elem.integer = 0;

        // Track if is a real or integer constant
        if (inst == CRCT)
        {
            fscanf(source->p_file, "%s\n", const_buffer);
            code->type = contains_char(const_buffer, '.') ? REAL_DATA_TYPE : INTEGER_DATA_TYPE;
            if (code->type == REAL_DATA_TYPE)
            {
                sscanf(const_buffer, "%lf", &code->elem.real);
            }
            else
            {
                sscanf(const_buffer, "%zu", &code->elem.integer);
            }
        }
            // No operand
        else if ((inst == SOMA) || (inst == SUBT) || (inst == MULT) || (inst == DIVI) || (inst == INVE) ||
                 (inst == CONJ)
                 || (inst == DISJ) || (inst == NEGA) || (inst == CPME) || (inst == CPMA) || (inst == CPIG) ||
                 (inst == CDES)
                 || (inst == CPMI) || (inst == CMAI) || (inst == LEIT) || (inst == IMPR) || (inst == INPP) ||
                 (inst == PARA)
                 || (inst == RTPR))
        {
            // No need to do anything
        }
            // Integer Operand
        else if ((inst == CRVL) || (inst == ARMZ) || (inst == DSVI) || (inst == DSVF) || (inst == ALME)
                 || (inst == PARAM) || (inst == PUSHER) || (inst == CHPR) || (inst == DESM))
        {
            fscanf(source->p_file, "%zu", &code->elem.integer);
            code->type = INTEGER_DATA_TYPE;
        }

        ++codes->actual;
    }
}

void print_codes(code_array_t *codes)
{
    for (size_t i = 0; i < codes->count; i++)
    {
        printf("%s ", instruction_to_string(codes->buffer[i].instruction));
        if (codes->buffer[i].type == REAL_DATA_TYPE)
        {
            printf("REAL %lf", codes->buffer[i].elem.real);
        }
        else if (codes->buffer[i].type == INTEGER_DATA_TYPE)
        {
            printf("INTEGER %zu", codes->buffer[i].elem.integer);
        }

        printf("\n");
    }
}

void push(data_stack_t *s, data_stack_elem_t e)
{
    if (s->actual + 1 >= s->count)
    {
        s->buffer = (data_stack_elem_t *) s_mem_realloc(s->buffer, s->count * sizeof(data_stack_elem_t),
                                                        s->count * sizeof(data_stack_elem_t) * 2);
        s->count *= 2;
    }

    s->buffer[s->top++] = e;
}

void push_from_code(data_stack_t *s, code_t *code)
{
    data_stack_elem_t e;
    e.type = code->type;
    e.data = code->elem;

    push(s, e);
}

void pop(data_stack_t *s)
{
    --s->top;
}

void interpret(code_array_t *codes)
{
    if (codes->buffer[0].instruction != INPP)
    {
        log_with_color(RED, "MISSING ");
        printf("INPP");
        log_with_color(RED, " INSTRUCTION!\n");
        throw_exception(RT_MISSING_INPP);
    }

    size_t ip = 0;
    data_stack_t stack;
    char const_buffer[65] = {'\0'};
    data_stack_elem_t aux;
    bool read_success = true;

    while (true)
    {

        switch (codes->buffer[ip].instruction)
        {
            case INVALID_INST:
                log_with_color(RED, "INVALID INSTRUCTION ");
                printf("%s", instruction_to_string(codes->buffer[ip].instruction));
                log_with_color(RED, "!\n");
                throw_exception(CG_INVALID_INSTRUCTION);
            case CRCT:
                push_from_code(&stack, &codes->buffer[ip]);
                break;
            case CRVL:
                push(&stack, stack.buffer[codes->buffer[ip].elem.integer]);
                break;
            case SOMA:
                // TODO: ERROR
                aux.type = stack.buffer[stack.top - 1].type;
                if (aux.type == INTEGER_DATA_TYPE)
                {
                    aux.data.integer =
                            stack.buffer[stack.top - 1].data.integer + stack.buffer[stack.top - 2].data.integer;
                }
                else
                {
                    aux.data.real = stack.buffer[stack.top - 1].data.real + stack.buffer[stack.top - 2].data.real;
                }
                pop(&stack);
                pop(&stack);
                push(&stack, aux);
                break;
            case SUBT:
                aux.type = stack.buffer[stack.top - 1].type;
                if (aux.type == INTEGER_DATA_TYPE)
                {
                    aux.data.integer =
                            stack.buffer[stack.top - 1].data.integer - stack.buffer[stack.top - 2].data.integer;
                }
                else
                {
                    aux.data.real = stack.buffer[stack.top - 1].data.real - stack.buffer[stack.top - 2].data.real;
                }
                pop(&stack);
                pop(&stack);
                push(&stack, aux);
                break;
            case MULT:
                aux.type = stack.buffer[stack.top - 1].type;
                if (aux.type == INTEGER_DATA_TYPE)
                {
                    aux.data.integer =
                            stack.buffer[stack.top - 1].data.integer * stack.buffer[stack.top - 2].data.integer;
                }
                else
                {
                    aux.data.real = stack.buffer[stack.top - 1].data.real * stack.buffer[stack.top - 2].data.real;
                }
                pop(&stack);
                pop(&stack);
                push(&stack, aux);
                break;
            case DIVI:
                aux.type = stack.buffer[stack.top - 1].type;
                if (aux.type == INTEGER_DATA_TYPE)
                {
                    aux.data.integer =
                            stack.buffer[stack.top - 1].data.integer / stack.buffer[stack.top - 2].data.integer;
                }
                else
                {
                    aux.data.real = stack.buffer[stack.top - 1].data.real / stack.buffer[stack.top - 2].data.real;
                }
                pop(&stack);
                pop(&stack);
                push(&stack, aux);
                break;
            case INVE:
                break;
            case CONJ:
                break;
            case DISJ:
                break;
            case NEGA:
                break;
            case CPME:
                break;
            case CPMA:
                break;
            case CPIG:
                break;
            case CDES:
                break;
            case CPMI:
                break;
            case CMAI:
                break;
            case ARMZ:
                stack.buffer[codes->buffer[ip].elem.integer] = stack.buffer[stack.top - 1];
                pop(&stack);
                break;
            case DSVI:
                break;
            case DSVF:
                break;
            case LEIT:
                read_success = false;
                while (!read_success)
                {
                    scanf("%s", const_buffer);
                    if (contains_char(const_buffer, '.'))
                    {
                        aux.type = REAL_DATA_TYPE;
                        read_success = sscanf(const_buffer, "%lf", &aux.data.real);
                    }
                    else
                    {
                        aux.type = INTEGER_DATA_TYPE;
                        read_success = sscanf(const_buffer, "%zu", &aux.data.integer);
                    }
                }
                push(&stack, aux);
                break;
            case IMPR:
                if (stack.buffer[stack.top - 1].type == INTEGER_DATA_TYPE)
                {
                    printf("%zu\n", stack.buffer[stack.top - 1].data.integer);
                }
                else if (stack.buffer[stack.top - 1].type == REAL_DATA_TYPE)
                {
                    printf("%lf\n", stack.buffer[stack.top - 1].data.real);
                }
                pop(&stack);
                break;
            case ALME:
                ++stack.top;
                break;
            case INPP:
                stack.buffer = s_mem_alloc(64, sizeof(data_stack_elem_t));
                stack.actual = 0;
                stack.top = 0;
                stack.count = 64;
                break;
            case PARA:
                exit(0);
            case PARAM:
                break;
            case PUSHER:
                break;
            case CHPR:
                break;
            case DESM:
                break;
            case RTPR:
                break;
        }
        ++ip;
    }
}

void try_interpret(file_t *source)
{
    code_array_t codes;
    code_from_file(source, &codes);
    //print_codes(&codes);
    interpret(&codes);
}
