//
// Created by adrianoii on 22/08/2021.
//

// TODO: pos MEMORIA
// TODO: fix visibility

#ifndef GCC_SYMBOL_TABLE_H
#define GCC_SYMBOL_TABLE_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "../string/string.h"
#include "../exceptions/exception.h"

#define GLOBAL_SCOPE 0

#define CHECK_FLAG(st, flag) (st->flags & flag) != 0

//region structs definition

typedef enum {
    ST_DIV_OP = 1,
    CG_MINUS = 2,
} st_flags_t;

typedef enum {
    LOOKUP_ANY,
    LOOKUP_VAR,
    LOOKUP_PROC,
} lookup_target_t;

typedef enum {
    INVALID_DATA_TYPE,
    INTEGER_DATA_TYPE,
    REAL_DATA_TYPE
} data_type_t;

typedef union int_real_u {
    size_t integer;
    double real;
} int_real_t;

typedef enum {
    INVALID_TYPE_ST_ENTRY,
    LITERAL_TYPE_ST_ENTRY,
    VAR_TYPE_ST_ENTRY,
    PROC_TYPE_ST_ENTRY,
} st_entry_type_t;

typedef struct parameter_list_s {
    struct var_st_elem_s *param;
    struct parameter_list_s *next;
} parameter_list_t;

typedef struct var_st_elem_s {
    string_t *id;
    size_t scope;
    size_t mem_pos; // Relative pos if scope != GLOBAL_SCOPE
    data_type_t data_type;
    int_real_t value;
    struct st_entry_s *hash_entry;
} var_st_elem_t;

typedef struct proc_st_elem_s {
    string_t *id;
    size_t scope;
    size_t num_vars;
    size_t first_instruction;
    parameter_list_t *parameters;
    struct st_entry_s *hash_entry;
} proc_st_elem_t;

typedef struct st_entry_s {
    st_entry_type_t type;
    bool valid;
    string_t *id;
    uint64_t hash;
    size_t scope;
    void *elem;
} st_entry_t;

typedef struct error_list_s {
    char const *error;
    struct error_list_s *next;
} error_list_t;

typedef struct analysis_queue_s {
    size_t scope;
    string_t *id;
    //    uint64_t hash;
    struct analysis_queue_s *next;
    void *elem;
} analysis_queue_t;

typedef struct st_s {
    size_t capacity;
    size_t count;
    st_entry_t *entries;
    analysis_queue_t *analysis_queue;
    size_t actual_scope;
    size_t prev_scope;
    size_t next_scope;
    proc_st_elem_t *actual_proc;
    proc_st_elem_t *global_proc;
    error_list_t *error_list;
    size_t flags;
} st_t;

//endregion structs definition

//region inits

st_t *st_init();

st_entry_t *st_entry_init();

st_entry_t *st_entries_init(size_t table_size);

proc_st_elem_t *proc_st_elem_init(string_t *id, size_t scope);

var_st_elem_t *var_st_elem_init(string_t *id, size_t scope, size_t mem_pos);

error_list_t *error_list_init();

//endregion inits

//region hashtable

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
uint64_t hash_func(string_t const *s);

void st_expand(st_t *st);

// Return a possible index to a string
size_t st_get_i(st_t const *st, string_t const *s);

// Return a possible index to a string, but using the given hash
size_t st_get_i_from_hash(st_t const *st, uint64_t hash);

// Try to return the respective st entry that matches the id,scope.
// If a hash is given, it will not recalculate the hash
st_entry_t *st_try_get(st_t const *st, string_t const *id, size_t scope, lookup_target_t target, uint64_t const *hash);

// Try to return the respective st entry that matches the id,scope (global or local one).
st_entry_t *st_try_get_var(st_t const *st, string_t const *id, size_t scope, uint64_t const *hash);

// Check if an entry existis on the hash table
bool st_lookup(st_t const *st, string_t const *id, size_t scope, lookup_target_t target, uint64_t const *hash);

// Add a new entry to the st
// It does not check if the element already exists
void
st_add_entry(st_t *st, st_entry_type_t type, string_t const *id, size_t scope, uint64_t const *hash, void const *elem);

//endregion hashtable

//region analysis queue

analysis_queue_t *analysis_queue_init();

// CAUTION: Used in diagrams
void analysis_queue_append(analysis_queue_t **aq, string_t const *id, size_t scope);

// MAYBE: do the serrch here
//void analysis_queue_append_from_ts(analysis_queue_t **aq, st_entry_t entry);

// CAUTION: Used in diagrams
bool analysis_queue_fetch_vars_entries(st_t const *st);

bool analysis_queue_assert_types(st_t *st);

// Set the actual scope to the first element of the analysis queue (destructive read)
exception_t analysis_queue_set_scope(st_t *st);

bool analysis_queue_assert_params(st_t *st);
// CAUTION: Used in diagrams
void analysis_queue_destroy(analysis_queue_t **aq);

void analysis_queue_pop(analysis_queue_t *aq);


//endregion analysis queue

//region semantic actions

exception_t assert_types(st_t *st);

//endregion semantic actions

// CAUTION: Used in diagrams
// Usado para voltar o escopo atual para o escopo global
// Set the actual scope of the symbol table
void st_return_global_scope(st_t *st);

// Update the actual scope to the next possible value
void st_update_scope(st_t *st);

void st_append_error(st_t *st, char const *fmt, ...);

void st_toggle_flag(st_t *st, st_flags_t flag);

// CAUTION: Used in diagrams
// Add a new procedure in the st
bool st_add_proc(st_t *st, string_t *id);

// Add a new variable in the st
bool st_add_var(st_t *st, analysis_queue_t *target);

// CAUTION: Used in diagrams
// Adiciona as variáveis que estiverem na fila de análise
bool st_add_vars(st_t *st);

bool st_add_vars_type(st_t *st, data_type_t data_type);

bool st_proc_add_params(st_t *st);

void st_set_proc_first_instruction(st_t *st, size_t i);

void st_update_proc_local_count(st_t *st);

void st_log(st_t *st);

//void analysis_queue_log(analysis_queue_t const *aq);

void st_var_log(st_t *st);

void st_proc_log(st_t *st);

#endif //GCC_SYMBOL_TABLE_H
