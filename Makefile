all: cblia

CFLAGS=-ggdb -O2 -Wall -Wextra -Werror -pedantic -pedantic-errors
CPPFLAGS=
LDFLAGS=

cblia: main.o tokeniser.o
	$(CC) $(CFLAGS) -o cblia $+ $(LDFLAGS)

tokeniser.o: tokeniser.c tokeniser.h lill_token.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

clean:
	rm -f *.o cblia

.PHONY: clean
