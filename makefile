CC=gcc
CFLAGS=-O3 -ggdb -Wall -Wextra -Wpedantic -Wconversion -Werror
#CPPFLAGS=
#LDFLAGS=
LDLIBS= `pkg-config --libs --cflags glfw3 glew`

all: a.out
	@echo 'compilation finished'
a.out:main.c main_globals.h
	${CC} ${CFLAGS} ${LDLIBS} main.c -o a.out
clean:
	rm -f *.out *.o
.PHONY: all clean
