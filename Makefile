all: cblia

CFLAGS=-ggdb -O2 -std=c99 -Wall -Wextra -Werror -pedantic -pedantic-errors
CPPFLAGS=-DDEBUG
LDFLAGS=

cblia: main.o tokeniser.o parser.o descent.o
	$(CC) $(CFLAGS) -o cblia $+ $(LDFLAGS)

main.o: main.c tokeniser.h parser.h errors.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

tokeniser.o: tokeniser.c tokeniser.h lill_token.h errors.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

parser.o: parser.c parser.h lill_token.h errors.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

descent.o: descent.c descent.h lill_token.h lill_tree_node.h errors.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

clean:
	rm -f *.o cblia

.PHONY: clean
