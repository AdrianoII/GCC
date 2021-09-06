# Made to work with clang

# clang flags
CFLAGS  = -std=c11
CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
CFLAGS += -g
#CFLAGS += -Weverything
ASANFLAGS  =

all: clean out/GCC

# ASan flags
debug: clean
debug: ASANFLAGS += -fsanitize=address
debug: ASANFLAGS += -fno-common
debug: ASANFLAGS += -fno-omit-frame-pointer
debug: ASANFLAGS += -fsanitize-address-use-after-scope
debug: export ASAN_OPTIONS=strict_string_checks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1

#debug: CFLAGS += -g
debug: out/GCC

out/GCC: out/parser.o
	clang $(CFLAGS) $(ASANFLAGS) src/main.c out/*.o -o out/GCC

out/parser.o: out/syntactic_analyzer.o
	clang $(CFLAGS) -c src/parser/parser.c -o out/parser.o

out/syntactic_analyzer.o: out/lexical_analyzer.o out/cli.o out/source_file.o #out/semantic_actions.o
	clang $(CFLAGS) -c src/syntactic/syntactic_analyzer.c -o out/syntactic_analyzer.o

out/source_file.o: out/st.o out/cli.o out/lexical_token.o
	clang $(CFLAGS) -c src/source_file/source_file.c -o out/source_file.o

out/st.o: out/sprint.o
	clang $(CFLAGS) -c src/symbol_table/symbol_table.c -o out/st.o

out/sprint.o: out/s_mem_alloc.o
	clang $(CFLAGS) -c src/cache_printf/cache_printf.c -o out/sprint.o

out/lexical_analyzer.o: out/lexical_token.o
	clang $(CFLAGS) -c src/lexical/lexical_analyzer.c -o out/lexical_analyzer.o

out/lexical_token.o: out/file_handler.o
	clang $(CFLAGS) -c src/lexical/lexical_token.c -o out/lexical_token.o

out/file_handler.o: out/string.o
	clang $(CFLAGS) -c src/file_handler/file_handler.c -o out/file_handler.o

out/cli.o: out/s_mem_alloc.o
	clang $(CFLAGS) -c src/cli/cli.c -o out/cli.o

out/string.o: out/s_mem_alloc.o
	clang $(CFLAGS) -c src/string/string.c -o out/string.o

out/s_mem_alloc.o : out/exceptions_handler.o
	clang $(CFLAGS) -c src/s_mem_alloc/s_mem_alloc.c -o out/s_mem_alloc.o

out/exceptions_handler.o: out/logs.o
	clang $(CFLAGS) -c src/exceptions/exceptions_handler.c -o out/exceptions_handler.o

out/logs.o:
	clang $(CFLAGS) -c src/logs/logs.c -o out/logs.o

.PHONY : tests
tests : debug
	clang ${CFLAGS} tests/lexical_tests.c out/logs.o out/exceptions_handler.o -o out/lexical_tests
	@./out/lexical_tests
	clang ${CFLAGS} tests/syntactic_tests.c out/logs.o out/exceptions_handler.o -o out/syntactic_tests
	@./out/syntactic_tests
	clang ${CFLAGS} tests/semantic_tests.c out/logs.o out/exceptions_handler.o -o out/semantic_tests
	@./out/semantic_tests

.PHONY : clean
clean:
	-rm out/GCC* out/*.o tests/outputs/*.txt out/lexical_tests
	-rm tests/outputs/*.txt