//
// Created by adrianoii on 11/09/2021.
//

#include "code_generation.h"
#include "../exceptions/exceptions_handler.h"
#include "../s_mem_alloc/s_mem_alloc.h"
#include "../file_handler/file_handler.h"

char* instruction_to_string(instruction_t const instruction)
{
    switch (instruction)
    {
        case CRCT:
            return "CRCT";
        case CRVL:
            return "CRVL";
        case SOMA:
            return "SOMA";
        case SUBT:
            return "SUBT";
        case MULT:
            return "MULT";
        case DIVI:
            return "DIVI";
        case INVE:
            return "INVE";
        case CONJ:
            return  "CONJ";
        case DISJ:
            return "DISJ";
        case NEGA:
            return "NEGA";
        case CPME:
            return "CPME";
        case CPMA:
            return "CPMA";
        case CPIG:
            return "CPIG";
        case CDES:
            return "CDES";
        case CPMI:
            return "CPMI";
        case CMAI:
            return "CMAI";
        case ARMZ:
            return "ARMZ";
        case DSVI:
            return "DSVI";
        case DSVF:
            return "DSVF";
        case LEIT:
            return "LEIT";
        case IMPR:
            return "IMPR";
        case ALME:
            return "ALME";
        case INPP:
            return "INPP";
        case PARA:
            return "PARA";
        case PARAM:
            return "PARAM";
        case PUSHER:
            return "PUSHER";
        case CHPR:
            return "CHPR";
        case DESM:
            return "DESM";
        case RTPR:
            return "RTPR";
        default:
            throw_exception(CG_INVALID_INSTRUCTION);
    }
}

code_list_t* cs_init()
{
    code_list_t *cs = s_mem_alloc(1, sizeof(code_list_t));
    return cs;
}

void cs_add_code(code_list_t * const cl, code_t * const c)
{
    if (cl->count == 0)
    {
        cl->head = c;
    }
    else
    {
        code_t *aux = cl->head;
        for(; aux->next; aux = aux->next);
        c->prev = aux;
        aux->next = c;
    }

    cl->last = c;
    ++cl->count;
}

void cs_write_codes(code_list_t const * const cl, char const * const path)
{
    file_t *source_file = file_init(path, "w");

    if (cl->count == 0)
    {
        return;
    }

    for (code_t *aux = cl->head; aux; aux = aux->next)
    {
        fprintf(source_file->p_file, "%s", instruction_to_string(aux->instruction));

        if (aux->type == INTEGER_DATA_TYPE)
        {
            fprintf(source_file->p_file, " %zu", aux->elem.integer);
        } else if (aux->type == REAL_DATA_TYPE)
        {
            fprintf(source_file->p_file, " %lf", aux->elem.real);
        }
        // else no data to print

        fprintf(source_file->p_file, "\n");
    }

    remove_elem_free_list(source_file->p_file, true);
    remove_elem_free_list(source_file, true);
}

void gen_code(code_list_t *cl, instruction_t instruction, int_real_t const e, data_type_t type)
{
    code_t *code = s_mem_alloc(1, sizeof(code_t));
    code->instruction = instruction;
    code->elem = e;
    code->type = type;

    cs_add_code(cl, code);
}

void gen_var_alloc_code(code_list_t *cl, st_t *st)
{
    int_real_t e;
    e.integer = 1;

    for(analysis_queue_t *aux = st->analysis_queue; aux; aux = aux->next)
    {
        gen_code(cl, ALME, e, INTEGER_DATA_TYPE);
    }
}

void gen_read_code(code_list_t *cl, st_t *st)
{
    analysis_queue_fetch_vars_entries(st);
    int_real_t e;
    for(analysis_queue_t *aux = st->analysis_queue; aux; aux = aux->next)
    {
        e.integer = 0;
        gen_code(cl, LEIT, e, INVALID_DATA_TYPE);
        e.integer = ((var_st_elem_t *)aux->elem)->mem_pos;
        gen_code(cl, ARMZ, e, INTEGER_DATA_TYPE);
    }
}


void gen_write_code(code_list_t *cl, st_t *st)
{
    analysis_queue_fetch_vars_entries(st);
    int_real_t e;

    for(analysis_queue_t *aux = st->analysis_queue; aux; aux = aux->next)
    {
        e.integer = ((var_st_elem_t *)aux->elem)->mem_pos;
        gen_code(cl, CRVL, e, INTEGER_DATA_TYPE);
        e.integer = 0;
        gen_code(cl, IMPR, e, INVALID_DATA_TYPE);
    }
}

void gen_assignment_code(code_list_t *cl, st_t *st)
{
    analysis_queue_fetch_vars_entries(st);
    int_real_t e;
    e.integer = ((var_st_elem_t *) st->analysis_queue->elem)->mem_pos;
    gen_code(cl, ARMZ, e, INTEGER_DATA_TYPE);
}

code_t* gen_template_cond_jump_code(code_list_t *cl)
{
    int_real_t e;
    e.integer = 0;
    gen_code(cl, DSVF, e, INTEGER_DATA_TYPE);

    return cl->last;
}

void gen_if_code(code_list_t *cl, code_t *template)
{
    int_real_t e;
    e.integer = cl->count;
    template->elem =e;
}

code_t* gen_template_uncond_jump_code(code_list_t *cl)
{
    int_real_t e;
    e.integer = 0;
    gen_code(cl, DSVI, e, INTEGER_DATA_TYPE);

    return cl->last;
}

void gen_else_code(code_list_t *cl, code_t *template)
{
    int_real_t e;
    e.integer = cl->count;
    template->elem =e;
}
