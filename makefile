CC=gcc
CFLAGS=-O3 -ggdb -Wall -Wextra -Wpedantic -Wconversion -Werror
#CPPFLAGS=
#LDFLAGS=
LDLIBS= `pkg-config --libs --cflags glfw3 glew`

all: a.out
	@echo 'compilation finished'
a.out:main.o render.o
	${CC} ${CFLAGS} ${LDLIBS} main.o render.o -o a.out
main.o:main.c main_globals.h render.h
	${CC} ${CFLAGS} ${LDLIBS} -c main.c -o main.o
render.o:render.c render.h
	${CC} ${CFLAGS} ${LDLIBS} -c render.c -o render.o
clean:
	rm -f *.out *.o
.PHONY: all clean
