all: cblia

CFLAGS=-ggdb -O2 -Wall -Wextra -Werror -pedantic -pedantic-errors
CPPFLAGS=
LDFLAGS=

cblia: *.o
	gcc $(CFLAGS) -o cblia $+ $(LDFLAGS)

clean:
	rm -f *.o cblia

.PHONY: clean
