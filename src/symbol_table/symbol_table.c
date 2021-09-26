//
// Created by adrianoii on 22/08/2021.
//

#include "symbol_table.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "../s_mem_alloc/s_mem_alloc.h"
#include "../cache_printf/cache_printf.h"
#include "../exceptions/exceptions_handler.h"

#define VAR_ST_CELL_SIZE 20

#define HASH_TABLE_INIT_SIZE 1
#define HASH_TABLE_LOAD_FACTOR 0.5L
#define HASH_TABLE_EXPAND_FACTOR 2

#define FNV_OFFSET UINT64_C(14695981039346656037)
#define FNV_PRIME UINT64_C(1099511628211)

#define TYPE_TO_STRING(t) t == INTEGER_DATA_TYPE ? "integer" : t == REAL_DATA_TYPE ? "real" : "invalid"
// region inits

st_t *st_init()
{
    st_t *st = s_mem_alloc(1, sizeof(st_t));

    st->entries = st_entries_init(HASH_TABLE_INIT_SIZE);
    st->capacity = HASH_TABLE_INIT_SIZE;
    return st;
}

st_entry_t *st_entry_init()
{
    return s_mem_alloc(1, sizeof(st_entry_t));
}

st_entry_t *st_entries_init(size_t const table_size)
{
    return s_mem_alloc(table_size, sizeof(st_entry_t));
}

proc_st_elem_t *proc_st_elem_init(string_t *const id, size_t const scope)
{
    proc_st_elem_t *proc = s_mem_alloc(1, sizeof(proc_st_elem_t));

    proc->id = id;
    proc->scope = scope;

    return proc;
}

var_st_elem_t *var_st_elem_init(string_t *const id, size_t const scope, size_t const mem_pos)
{
    var_st_elem_t *var = s_mem_alloc(1, sizeof(var_st_elem_t));

    var->id = id;
    var->scope = scope;
    var->mem_pos =mem_pos;

    return var;
}

error_list_t *error_list_init()
{
    return s_mem_alloc(1, sizeof(error_list_t));
}
//endregion inits

//region hashtable

uint64_t hash_func(string_t const *const s)
{
    uint64_t hash = FNV_OFFSET;

    for (size_t i = 0; i < s->length; i++)
    {
        hash ^= (uint64_t) (unsigned char) s->buffer[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

void st_expand(st_t *st)
{
    st_entry_t *old_buffer = st->entries;
    size_t old_capacity = st->capacity;
    st->entries = st_entries_init(st->capacity * HASH_TABLE_EXPAND_FACTOR);
    st->count = 0;
    st->capacity *= HASH_TABLE_EXPAND_FACTOR;

    for (size_t i = 0; i < old_capacity; i++)
    {
        if (old_buffer[i].valid)
        {
            st_add_entry(st, old_buffer[i].type, old_buffer[i].id, old_buffer[i].scope, &old_buffer[i].hash,
                         old_buffer[i].elem);
        }
    }

    remove_elem_free_list(old_buffer, true);


}

size_t st_get_i(st_t const *const st, string_t const *const s)
{
    return st_get_i_from_hash(st, hash_func(s));
}

size_t st_get_i_from_hash(st_t const *const st, uint64_t const hash)
{
    return ((size_t) (hash % ((uint64_t) st->capacity)));
}

st_entry_t *
st_try_get(st_t const *const st, string_t const *id, size_t scope, lookup_target_t target, uint64_t const *hash)
{
    if (st->count == 0)
    {
        return false;
    }

    size_t possible_i = !hash ? st_get_i(st, id) : st_get_i_from_hash(st, (*hash));

    do
    {
        if (possible_i == st->capacity)
        {
            possible_i = 0;
        }

        if (st->entries[possible_i].valid && string_equals(st->entries[possible_i].id, id))
        {
            if (target == LOOKUP_VAR)
            {
                if (st->entries[possible_i].type == VAR_TYPE_ST_ENTRY && st->entries[possible_i].scope == scope)
                {
                    return &st->entries[possible_i];
                }
            }
            else if (target == LOOKUP_PROC)
            {
                if (st->entries[possible_i].type == PROC_TYPE_ST_ENTRY)
                {
                    return &st->entries[possible_i];
                }
                // LOOKUP_PROC || LOOKUP_ANY
            }
            else if (target == LOOKUP_ANY)
            {
                return &st->entries[possible_i];
            }
        }
    } while (st->entries[possible_i++].valid);

    return NULL;
}

st_entry_t *
st_try_get_var(st_t const *const st, string_t const *id, size_t scope, uint64_t const *hash)
{
    st_entry_t *possible_entry = st_try_get(st, id, scope, LOOKUP_VAR, hash);
    if (scope != GLOBAL_SCOPE && possible_entry == NULL)
    {
        possible_entry = st_try_get(st, id, GLOBAL_SCOPE, LOOKUP_VAR, hash);
    }

    return possible_entry;
}

bool st_lookup(st_t const *const st, string_t const *const id, size_t const scope, lookup_target_t target,
               uint64_t const *const hash)
{
    return st_try_get(st, id, scope, target, hash) != NULL;
}

void st_add_entry(st_t *const st, st_entry_type_t const type, string_t const *const id, size_t const scope,
                  uint64_t const *const hash, void const *const elem)
{
    uint64_t elem_hash = !hash ? hash_func(id) : (*hash);
    size_t possible_i = st_get_i_from_hash(st, elem_hash);

    while (st->entries[possible_i].valid)
    {
        ++possible_i;
        if (possible_i == st->capacity)
        {
            possible_i = 0;
        }
    }

    st->entries[possible_i].valid = true;
    st->entries[possible_i].id = (string_t *) id;
    st->entries[possible_i].type = type;
    st->entries[possible_i].scope = scope;
    st->entries[possible_i].elem = (void *) elem;
    st->entries[possible_i].hash = elem_hash;
    ++st->count;

    if (type == VAR_TYPE_ST_ENTRY)
    {
        ((var_st_elem_t *) elem)->hash_entry = &st->entries[possible_i];
    }
    else if (type == PROC_TYPE_ST_ENTRY)
    {
        ((proc_st_elem_t *) elem)->hash_entry = &st->entries[possible_i];
    }
    else
    {
        throw_exception(ST_INVALID_TYPE_ENTRY);
    }


    if (((long double) st->count / st->capacity) >= HASH_TABLE_LOAD_FACTOR)
    {
        st_expand(st);
    }
}

//endregion hashtable

//region analysis queue

analysis_queue_t *analysis_queue_init()
{
    analysis_queue_t *new = s_mem_alloc(1, sizeof(analysis_queue_t));
    return new;
}

void analysis_queue_append(analysis_queue_t **aq, string_t const *const id, size_t const scope)
{
    analysis_queue_t *new = analysis_queue_init();

    new->id = string_copy(id);
    new->scope = scope;

    if (!(*aq))
    {
        (*aq) = new;
        return;
    }
    analysis_queue_t *aux = (*aq);

    for (; aux->next; aux = aux->next)
    {}

    aux->next = new;
}

bool analysis_queue_fetch_vars_entries(st_t const *const st)
{

    bool all_vars_found = true;

    for (analysis_queue_t *aux = st->analysis_queue; aux; aux = aux->next)
    {
        if(aux->elem == NULL)
        {
            aux->elem = st_try_get_var(st, aux->id, aux->scope, NULL);
            if (aux->elem == NULL)
            {
                st_append_error((st_t *) st, "Variable \"%s\" does not exists on the current scope!\n",
                                aux->id->buffer);
                all_vars_found = false;
            }
            else
            {
                aux->elem = ((st_entry_t *) aux->elem)->elem;
            }
        }
    }

    return all_vars_found;
}

bool analysis_queue_assert_types(st_t *const st)
{
    if (st->analysis_queue == NULL)
    {
        return true;
    }

    bool compatible_types = true;
    data_type_t first_type = ((var_st_elem_t *) st->analysis_queue->elem)->data_type, aux_type = first_type;

    if (CHECK_FLAG(st, ST_DIV_OP))
    {
        if (first_type != INTEGER_DATA_TYPE)
        {
            st_append_error((st_t *) st, "Division result should be stored in an integers, not in the \"%s\"(%s)!\n",
                            st->analysis_queue->id->buffer, TYPE_TO_STRING(first_type));
            first_type = INTEGER_DATA_TYPE;
            compatible_types = false;
        }
    }

    for (analysis_queue_t const *aux = st->analysis_queue->next; aux; aux = aux->next)
    {
        aux_type = ((var_st_elem_t *) aux->elem)->data_type;
        if (first_type != aux_type)
        {
            compatible_types = false;
            if (CHECK_FLAG(st, ST_DIV_OP))
            {
                st_append_error((st_t *) st, "Division operands should be integers, not \"%s\"(%s)!\n", aux->id->buffer,
                                TYPE_TO_STRING(aux_type));
            }
            else
            {
                st_append_error((st_t *) st, "Incompatible types between %s(%s) and %s(%s)\n",
                                st->analysis_queue->id->buffer,
                                TYPE_TO_STRING(first_type),
                                aux->id->buffer, TYPE_TO_STRING(aux_type));
            }
        }
    }

    if (CHECK_FLAG(st, ST_DIV_OP))
    {
        st_toggle_flag(st, ST_DIV_OP);
    }

    return compatible_types;
}

exception_t analysis_queue_set_scope(st_t *st)
{
    if (st->actual_scope  != GLOBAL_SCOPE)
    {
        st_append_error(st, "Procedure nesting is not allowed!\n");
        analysis_queue_destroy(&st->analysis_queue);
        return SEM_PROCEDURE_NESTING;
    }

    st_entry_t *proc_entry = st_try_get(st, st->analysis_queue->id, 0, LOOKUP_PROC, NULL);
    if (proc_entry == NULL)
    {
        st_append_error(st, "Procedure \"%s\" has not been declared!\n", st->analysis_queue->id->buffer);
        analysis_queue_destroy(&st->analysis_queue);
        return ST_ID_NOT_FOUND;
    }

    proc_st_elem_t *proc = (proc_st_elem_t *) proc_entry->elem;
    st->actual_proc = proc;
    st->prev_scope = st->actual_scope;
    st->actual_scope = proc_entry->scope;

    analysis_queue_destroy(&st->analysis_queue);

    return EMPTY_EXCEPTION;
}

bool analysis_queue_assert_params(st_t *st)
{
    parameter_list_t *param = st->actual_proc->parameters;
    analysis_queue_t *arg_list = st->analysis_queue;
    st_entry_t *arg_entry = NULL;
    size_t num_arg = 0, num_param = 0;
    bool compatible_args = true;

    while (param && arg_list)
    {
        arg_entry = st_try_get_var(st, arg_list->id, arg_list->scope, NULL);

        if (!arg_entry)
        {
            st_append_error(st, "Variable \"%s\" does not exists on the current scope!\n", arg_list->id->buffer);
            compatible_args = false;
        }
        else
        {
            var_st_elem_t *arg_metadata = ((var_st_elem_t *) arg_entry->elem);

            if (arg_metadata->data_type != param->param->data_type)
            {
                st_append_error(st,
                                "Argument \"%s\" of %s type doesn't have a compatible type with the parameter \"%s\" of %s type!\n",
                                arg_entry->id->buffer, TYPE_TO_STRING(arg_metadata->data_type),
                                param->param->hash_entry->id->buffer,
                                TYPE_TO_STRING(param->param->data_type));
                compatible_args = false;
            }
        }

        param = param->next;
        arg_list = arg_list->next;
        ++num_arg;
        ++num_param;
    }

    while (arg_list)
    {
        ++num_arg;
        arg_list = arg_list->next;
    }

    while (param)
    {
        ++num_param;
        param = param->next;
    }

    if (num_arg != num_param)
    {
        st_append_error(st, "Incompatible numbers of arguments: Expected %zu got %zu!\n", num_param, num_arg);
        compatible_args = false;
    }

    return compatible_args;
}

void analysis_queue_pop(analysis_queue_t *aq)
{
    string_destroy(aq->id);
    remove_elem_free_list(aq, true);
}


void analysis_queue_destroy(analysis_queue_t **aq)
{
    analysis_queue_t *next = NULL;

    while ((*aq) != NULL)
    {
        next = (*aq)->next;
        analysis_queue_pop((*aq));
        (*aq) = next;
    }
}

//endregion analysis queue

//region semantic actions

exception_t assert_types(st_t *const st)
{
    exception_t error = EMPTY_EXCEPTION;

    if (!analysis_queue_fetch_vars_entries(st))
    {
        error = ST_ID_NOT_FOUND;
    }
    else if (!analysis_queue_assert_types(st))
    {
        error = SEM_INCOMPATIBLE_TYPES;
    }

    return error;
}

//endregion semantic actions


void st_return_global_scope(st_t *const st)
{
    st->actual_proc = st->global_proc;
    st->actual_scope = GLOBAL_SCOPE;
}

void st_update_scope(st_t *const st)
{
    st->actual_scope = st->next_scope++;
}

void st_append_error(st_t *const st, char const *const fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    error_list_t *error = error_list_init();

    error->error = vcache_printf(fmt, args);

    if (!st->error_list)
    {
        st->error_list = error;
        return;
    }

    error_list_t *aux = st->error_list;
    for (; aux->next; aux = aux->next)
    {}

    aux->next = error;
}

void st_toggle_flag(st_t *const st, st_flags_t const flag)
{
    st->flags ^= flag;
}


bool st_add_proc(st_t *st, string_t *id)
{
    uint64_t hash = hash_func(id);

    if (st_lookup(st, id, st->actual_scope, LOOKUP_PROC, &hash))
    {
        st_append_error(st, "Procedure \"%s\" has already been declared!\n", id->buffer);
        return false;
    }
    else if (st_lookup(st, id, st->actual_scope, LOOKUP_ANY, &hash))
    {
        st_append_error(st, "Identifier \"%s\" has already been declared!\n", id->buffer);
        return false;
    }

    string_t *id_copy = string_copy(id);
    st_update_scope(st);

    proc_st_elem_t *proc = proc_st_elem_init(id_copy, st->actual_scope);

    if (st->actual_scope == 0)
    {
        st->global_proc = proc;
    }

    st->actual_proc = proc;

    st_add_entry(st, PROC_TYPE_ST_ENTRY, id_copy, st->actual_scope, &hash, proc);

    return true;
}

bool st_add_var(st_t *st, analysis_queue_t *target)
{
    uint64_t hash = hash_func(target->id);

    // Vars can not have the same identifier of a procedure
    if (st_lookup(st, target->id, target->scope, LOOKUP_VAR, &hash))
    {
        st_append_error(st, "Variable \"%s\" has already been declared on this scope!\n", target->id->buffer);
        return false;
    }
    else if (st_lookup(st, target->id, target->scope, LOOKUP_PROC, &hash))
    {
        st_append_error(st, "Identifier \"%s\" has already been declared as a procedure!\n", target->id->buffer);
        return false;
    }

    string_t *id_copy = string_copy(target->id);

    var_st_elem_t *var = var_st_elem_init(id_copy, target->scope, st->actual_scope == GLOBAL_SCOPE ? st->actual_proc->num_vars++ : ((st->actual_proc->num_vars++) + 1));

    target->elem = var;

    st_add_entry(st, VAR_TYPE_ST_ENTRY, id_copy, target->scope, &hash, var);

    return true;
}

bool st_add_vars(st_t *st)
{
    bool no_vars_founded = true;

    for (analysis_queue_t *aux = st->analysis_queue; aux; aux = aux->next)
    {
        no_vars_founded &= st_add_var(st, aux);
    }

    return no_vars_founded;
}

bool st_add_vars_type(st_t *st, data_type_t data_type)
{
    bool no_erros_happened = true;

    for (analysis_queue_t *aux = st->analysis_queue; aux; aux = aux->next)
    {
        var_st_elem_t *var = ((var_st_elem_t *) aux->elem);
        if (!var || var->data_type != INVALID_DATA_TYPE)
        {
            continue;
        }

        var->data_type = data_type;
    }

    return no_erros_happened;
}

bool st_proc_add_params(st_t *st)
{
    bool no_erros_happened = true;

    for (analysis_queue_t *aux = st->analysis_queue; aux; aux = aux->next)
    {
        parameter_list_t *new = s_mem_alloc(1, sizeof(parameter_list_t));
        var_st_elem_t *var = ((var_st_elem_t *) aux->elem);
        new->param = var;
        //++st->actual_proc->num_vars;

        if (st->actual_proc->parameters == NULL)
        {
            st->actual_proc->parameters = new;
            continue;
        }

        parameter_list_t *aux2 = st->actual_proc->parameters;
        while (aux2->next != NULL)
        {
            aux2 = aux2->next;
        }

        aux2->next = new;
    }

    return no_erros_happened;
}

void st_set_proc_first_instruction(st_t *st, size_t i)
{
    st->actual_proc->first_instruction = i;
}

void st_update_proc_local_count(st_t *st)
{
    st->actual_proc->num_vars = st->actual_proc->num_vars;
}


void st_log(st_t *st)
{
    // TODO: Complete log with error msg and analisys queue
    printf("ST\n");
    printf("%*s|%*s|%*s|%*s|%*s\n", VAR_ST_CELL_SIZE, "actual_scope", VAR_ST_CELL_SIZE, "next_scope",
           VAR_ST_CELL_SIZE,
           "count", VAR_ST_CELL_SIZE, "capacity", VAR_ST_CELL_SIZE, "div_op");
    printf("%*zu|%*zu|%*zu|%*zu|%*s\n", VAR_ST_CELL_SIZE, st->actual_scope, VAR_ST_CELL_SIZE, st->next_scope,
           VAR_ST_CELL_SIZE, st->count, VAR_ST_CELL_SIZE, st->capacity,
           VAR_ST_CELL_SIZE, CHECK_FLAG(st, ST_DIV_OP) ? "true" : "false");
    printf("\n");
    st_var_log(st);
    printf("\n");
    st_proc_log(st);
}


void st_var_log(st_t *st)
{
    printf("VAR ST\n");
    printf("%*s|%*s|%*s|%*s|%*s\n", VAR_ST_CELL_SIZE, "identifier", VAR_ST_CELL_SIZE, "scope", VAR_ST_CELL_SIZE,
           "mem_pos", VAR_ST_CELL_SIZE, "data_type", VAR_ST_CELL_SIZE, "value");
    var_st_elem_t *aux = NULL;
    for (size_t i = 0; i < st->capacity; i++)
    {
        if (st->entries[i].valid && st->entries[i].type == VAR_TYPE_ST_ENTRY)
        {
            printf("%*s|%*zu", VAR_ST_CELL_SIZE, st->entries[i].id->buffer, VAR_ST_CELL_SIZE, st->entries[i].scope);
            aux = (var_st_elem_t *) st->entries[i].elem;
            printf("|%*zu", VAR_ST_CELL_SIZE, aux->mem_pos);

            if (aux->data_type == INTEGER_DATA_TYPE)
            {
                printf("|%*s|%*lld\n", VAR_ST_CELL_SIZE, "integer", VAR_ST_CELL_SIZE, aux->value.integer);
            }
            else if (aux->data_type == REAL_DATA_TYPE)
            {
                printf("|%*s|%*lf\n", VAR_ST_CELL_SIZE, "real", VAR_ST_CELL_SIZE, aux->value.real);
            }
            else
            {
                printf("|%*s|%*s\n", VAR_ST_CELL_SIZE, "invalid", VAR_ST_CELL_SIZE, "-");
            }
        }
    }
}

void st_proc_log(st_t *st)
{
    // TODO: LOG PARAMETERS
    printf("PROC ST\n");
    printf("%*s|%*s|%*s|%*s\n", VAR_ST_CELL_SIZE, "identifier", VAR_ST_CELL_SIZE, "scope", VAR_ST_CELL_SIZE, "num_vars", VAR_ST_CELL_SIZE, "parameters");
    for (size_t i = 0; i < st->capacity; i++)
    {
        if (st->entries[i].valid && st->entries[i].type == PROC_TYPE_ST_ENTRY)
        {
            proc_st_elem_t *aux = st->entries[i].elem;
            printf("%*.*s|%*zu|%*zu|", VAR_ST_CELL_SIZE, VAR_ST_CELL_SIZE, st->entries[i].id->buffer, VAR_ST_CELL_SIZE,
                   st->entries[i].scope,VAR_ST_CELL_SIZE, aux->num_vars);
            if (aux && aux->parameters)
            {
                parameter_list_t *aux2 = aux->parameters;
                while (aux2)
                {
                    printf("%s,", aux2->param->id->buffer);
                    aux2 = aux2->next;
                }
            }
            printf("\n");
        }
    }
}

//void analysis_queue_log(analysis_queue_t const * const aq)
//{
////    typedef struct analysis_queue_s {
////        size_t scope;
////        string_t *id;
////        //    uint64_t hash;
////        struct analysis_queue_s *next;
////        void *elem;
////    } analysis_queue_t;
//    for (analysis_queue_t const *aux = aq; aux; aux = aux->next)
//    {
//        printf("ANALYSIS QUEUE\n");
//        printf("%*s|%*s|%*s\n", VAR_ST_CELL_SIZE, "identifier", VAR_ST_CELL_SIZE, "scope", VAR_ST_CELL_SIZE, "parameters");
//    }
//}
