//
// Created by adrianoii on 04/08/2021.
//
#include "stdio.h"
#include "stdlib.h"
#include "../src/exceptions/exceptions_handler.h"
#include "../src/logs/logs.h"
#define GCC_PATH "out/GCC"
#define TESTS_PATH "tests/inputs/semantic/"
#define OUTPUTS_PATH "tests/outputs/semantic/"
#define TEMPLATES_PATH "tests/templates/semantic/"

void test_valid_symbols(void);

void test_semantic_rules(void);

void assert_valid_output_file(const char *file_name);

void assert_valid_output_file(const char *file_name)
{
    char cmd_buffer[1024] = {'\0'};
    snprintf(cmd_buffer, 1023, "./" GCC_PATH " " TESTS_PATH "%s.txt> " OUTPUTS_PATH "%s_out.txt",
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
        log_with_color_nl(RED,cmd_buffer);
        throw_exception(ASSERT_FAIL);
    }
}

void test_semantic_rules(void) {
    assert_valid_output_file("invalid_division");
    assert_valid_output_file("valid_division");
    assert_valid_output_file("invalid_read_write");
    assert_valid_output_file("valid_read_write");
    assert_valid_output_file("invalid_assignment");
    assert_valid_output_file("valid_assignment");
    assert_valid_output_file("invalid_param");
    assert_valid_output_file("valid_param");
    assert_valid_output_file("invalid_proc");
    assert_valid_output_file("valid_proc");
    assert_valid_output_file("invalid_declaration");
    assert_valid_output_file("valid_declaration");
    assert_valid_output_file("invalid_scope");
}

int main(void) {
    log_with_color_nl(YELB, "Starting semantic parser tests!");
    test_semantic_rules();
    log_with_color_nl(GRN, "SEMANTIC parser tests: OK");
}
