//
// Created by adrianoii on 06/08/2021.
//
#include "stdio.h"
#include "stdlib.h"
#include "../src/exceptions/exceptions_handler.h"
#include "../src/logs/logs.h"

#define GCC_PATH "out/GCC"
#define TESTS_PATH "tests/inputs/"
#define OUTPUTS_PATH "tests/outputs/"
#define TEMPLATES_PATH "tests/templates/"

void test_valid_symbols(void);
void nts_program(void);
void test_nts(void);
void assert_valid_output_file(const char *file_name);

void assert_valid_output_file(const char *file_name)
{
    char cmd_buffer[1024] = {'\0'};
    snprintf(cmd_buffer, 1023, "./" GCC_PATH " " TESTS_PATH "%s.txt -sa sy> " OUTPUTS_PATH "%s_out.txt",
             file_name, file_name);
    printf("EXECUTE: %s\n", cmd_buffer);
    system(cmd_buffer);
    snprintf(cmd_buffer, 1023, "diff " OUTPUTS_PATH "%s_out.txt " TEMPLATES_PATH "%s_template.txt",
             file_name, file_name);
    printf("DIFF: %s\n", cmd_buffer);
    int diff_return_code = system(cmd_buffer);
    if (diff_return_code != 0)
    {
        snprintf(cmd_buffer, 1023, "TEST FAILED: %s.", file_name);
        log_with_color_nl(RED, cmd_buffer);
        throw_exception(ASSERT_FAIL);
    }
}

void nts_program(void)
{
    assert_valid_output_file("invalid_program_program");
    assert_valid_output_file("invalid_program_identifier");
    assert_valid_output_file("invalid_program_dot");
}

void test_nts(void)
{
    nts_program();
}

int main(void)
{
    log_with_color_nl(RED, "TESTS NOT IMPLEMENTED YET!");
    log_with_color_nl(YELB, "Starting syntactic parser tests!");
//    test_nts();
    log_with_color_nl(GRN, "Syntactic parser tests: OK");
}
