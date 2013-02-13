all: cblia

CFLAGS=-ggdb -O2 -std=c99 -Wall -Wextra -Werror -pedantic -pedantic-errors
CPPFLAGS=-DDEBUG
LDFLAGS=

cblia: main.o tokeniser.o parser_1.o parser_2.o
	$(CC) $(CFLAGS) -o cblia $+ $(LDFLAGS)

tokeniser.o: tokeniser.c tokeniser.h lill_token.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

parser_1.o: parser_1.c parser.h lill_token.h lill_stoken.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

parser_2.o: parser_2.c parser.h lill_stoken.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

clean:
	rm -f *.o cblia

.PHONY: clean
