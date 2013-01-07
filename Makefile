all: cblia

CFLAGS=-ggdb -O2 -Wall -Wextra -Werror -pedantic -pedantic-errors
CPPFLAGS=
LDFLAGS=

cblia: main.o
	gcc $(CFLAGS) -o cblia main.o $(LDFLAGS)

clean:
	rm main.o cblia

.PHONY: clean
