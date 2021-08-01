# Made to work with GCC

# GCC flags
CFLAGS  = -std=c11
CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Werror
CFLAGS += -Wmissing-declarations

# ASan flgas
ASANFLAGS  = -fsanitize=address
ASANFLAGS += -fno-common
ASANFLAGS += -fno-omit-frame-pointer

all: out/GCC

out/GCC: out/logs.o out/exceptions_handler.o out/string.o out/cli.o out/file_handler.o out/lexical_token.o \
 out/lexical_analyzer.o
	gcc $(CFLAGS) src/main.c out/*.o -o out/GCC

out/lexical_analyzer.o: out/logs.o out/exceptions_handler.o out/file_handler.o
	gcc $(CFLAGS) -c src/lexical/lexical_analyzer.c -o out/lexical_analyzer.o

out/lexical_token.o: out/exceptions_handler.o out/string.o
	gcc $(CFLAGS) -c src/lexical/lexical_token.c -o out/lexical_token.o

out/file_handler.o: out/exceptions_handler.o
	gcc $(CFLAGS) -c src/file_handler/file_handler.c -o out/file_handler.o

out/cli.o: out/exceptions_handler.o out/logs.o
	gcc $(CFLAGS) -c src/cli/cli.c -o out/cli.o

out/string.o: out/exceptions_handler.o
	gcc $(CFLAGS) -c src/string/string.c -o out/string.o

out/exceptions_handler.o: out/logs.o
	gcc $(CFLAGS) -c src/exceptions/exceptions_handler.c -o out/exceptions_handler.o

out/logs.o:
	gcc $(CFLAGS) -c src/logs/logs.c -o out/logs.o

.PHONY : debug
debug: out/logs.o out/exceptions_handler.o out/string.o out/cli.o out/file_handler.o out/lexical_token.o \
        out/lexical_analyzer.o
	gcc $(CFLAGS) -g src/main.c out/*.o -o out/GCC-debug

.PHONY : memcheck
memcheck: out/logs.o out/exceptions_handler.o out/string.o out/cli.o out/file_handler.o out/lexical_token.o \
        out/lexical_analyzer.o
	gcc ${ASANFLAGS} $(CFLAGS) -g src/main.c out/*.o -o out/GCC-memcheck
	./out/GCC-memcheck
	@echo "Memory check passed"

.PHONY : clean
clean:
	rm out/GCC out/*.o