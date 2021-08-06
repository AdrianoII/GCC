//
// Created by adrianoii on 04/08/2021.
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

void test_lexemes(void);

void assert_valid_output_file(const char *file_name);

void assert_valid_output_file(const char *file_name)
{
    char cmd_buffer[1024] = {'\0'};
    int diff_return_code = 0;
    snprintf(cmd_buffer, 1023, "./" GCC_PATH " " TESTS_PATH "%s.txt > " OUTPUTS_PATH "%s_out.txt",
             file_name, file_name);
    printf("EXECUTE: %s\n", cmd_buffer);
    system(cmd_buffer);
    snprintf(cmd_buffer, 1023, "diff " OUTPUTS_PATH "%s_out.txt " TEMPLATES_PATH "%s_template.txt",
        file_name, file_name);
    printf("DIFF: %s\n", cmd_buffer);
    diff_return_code = system(cmd_buffer);
    if (diff_return_code != 0)
    {
        snprintf(cmd_buffer, 1023, "TEST FAILED: %s.", file_name);
        log_with_color_nl(RED,cmd_buffer);
        throw_exception(ASSERT_FAIL);
    }
}

void test_lexemes(void) {
    assert_valid_output_file("invalid_bracket_comment");
    assert_valid_output_file("invalid_multiline_bracket_comment");
    assert_valid_output_file("invalid_multiline_slash_comment");
    assert_valid_output_file("invalid_real");
    assert_valid_output_file("invalid_slash_comment");
    assert_valid_output_file("valid_bracket_comment");
    assert_valid_output_file("valid_id");
    assert_valid_output_file("valid_interger");
    assert_valid_output_file("valid_keywords");
    assert_valid_output_file("valid_multi_comments");
    assert_valid_output_file("valid_multiline_slash_comment");
    assert_valid_output_file("valid_multiline_bracket_comment");
    assert_valid_output_file("valid_real");
    assert_valid_output_file("valid_slash_comment");
    assert_valid_output_file("valid_symbols");
}

int main(void) {
    log_with_color_nl(YELB, "Starting lexical parser tests!");
    test_lexemes();
    log_with_color_nl(GRN, "Lexical parser tests: OK");
}
