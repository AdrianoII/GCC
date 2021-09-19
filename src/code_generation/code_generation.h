//
// Created by adrianoii on 11/09/2021.
//

#ifndef GCC_CODE_GENERATION_H
#define GCC_CODE_GENERATION_H

#include <stddef.h>
#include <stdarg.h>
#include "../symbol_table/symbol_table.h"

typedef enum {
    CRCT, // load constant
    CRVL, // load value
    SOMA, // sum
    SUBT, // Subtraction
    MULT, // Multiplication
    DIVI, // integer division
    INVE, // invert signal
    CONJ, // AND
    DISJ, // OR
    NEGA, // CRVL
    CPME, // less than compare
    CPMA, // greater than compare
    CPIG, // equals compare
    CDES, // not equals compare
    CPMI, // less than or equal compare
    CMAI, // greater than or equal compare
    ARMZ, // Store
    DSVI, // Unconditional jump
    DSVF, // Conditional jump
    LEIT, // Read
    IMPR, // Print
    ALME, // Alloc memory
    INPP, // Init program
    PARA, // Stop program
    PARAM, // alloc memory to the parameter
    PUSHER, // Push next instruction in the stack
    CHPR, // Jump to the procedure
    DESM, // Free procedure memory
    RTPR, // Return from procedure

} instruction_t;

typedef struct code_s {
    instruction_t instruction;
    data_type_t type;
    int_real_t elem;
    struct code_s *next;
} code_t;

typedef struct code_list_s {
    code_t *head;
    size_t count;
} code_list_t;

code_list_t *cs_init();

void cs_add_code(code_list_t *cl, code_t *c);

void cs_write_codes(code_list_t const *cl, char const *path);

void gen_code(code_list_t *cl, instruction_t instruction, int_real_t e, data_type_t type);

// Add the alloc instructions for the analysis queue members
void gen_var_alloc_code(code_list_t *cl, st_t *st);

// Add the LEIT and ARMZ instructions for the analysis queue members
void gen_read_code(code_list_t *cl, st_t *st);

// Add the CRVL and IMPR instructions for the analysis queue members
void gen_write_code(code_list_t *cl, st_t *st);

// Add the ARMZ for the first element in the analysis queue
void gen_assignment_code(code_list_t *cl, st_t *st);

#endif //GCC_CODE_GENERATION_H
