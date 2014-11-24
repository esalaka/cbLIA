all: cblia

CC=gcc
CFLAGS=-ggdb -O2 -std=c99 -Wall -Wextra -Werror -pedantic -pedantic-errors
CPPFLAGS=-DDEBUG
LDFLAGS=

cblia: main.o tokeniser.o parser.o descent.o util.o generate.o function.o
	$(CC) $(CFLAGS) -o cblia $+ $(LDFLAGS)

main.o: main.c tokeniser.h parser.h errors.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

tokeniser.o: tokeniser.c tokeniser.h lill_token.h errors.h util.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

parser.o: parser.c parser.h lill_token.h errors.h util.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

descent.o: descent.c descent.h lill_token.h lill_tree_node.h errors.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

generate.o: generate.c generate.h function.h lill_tree_node.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

function.o: function.c function.h lill_tree_node.h generate.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

util.o: util.c util.h

clean:
	rm -f *.o cblia

.PHONY: clean
