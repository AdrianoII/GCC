cmake_minimum_required(VERSION 3.19)
project(GCC C)

set(CMAKE_C_STANDARD 11)

add_executable(GCC src/main.c src/lexical/lexical_token.c src/lexical/lexical_token.h src/string/string.c src/string/string.h src/exceptions/exceptions_handler.c src/exceptions/exceptions_handler.h src/exceptions/exception.c src/exceptions/exception.h src/exceptions/logs.c src/exceptions/logs.h src/file_handler/file_handler.c src/file_handler/file_handler.h src/cli/cli.c src/cli/cli.h src/lexical/lexical_analyzer.c src/lexical/lexical_analyzer.h)