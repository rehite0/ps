CC=gcc
CFLAGS= -ggdb -Ofast -march='native' #-ffinite-math-only -freciprocal-math
CFLAGS+= -Wall -Wextra -Wpedantic -Wconversion -Werror -Winline -Wshadow -Wfloat-equal
CFLAGS+= -Wunreachable-code -Winit-self -Wuninitialized
#CPPFLAGS=
#LDFLAGS=
LDLIBS= `pkg-config --libs --cflags glfw3 glew` -lm -lmvec
OBJ=main.o render.o ball_api.o pse.o gen.o par.o motion.o collision.o

all: a.out
	@echo 'compilation finished'
a.out:${OBJ}
	${CC} ${CFLAGS} ${LDLIBS} ${OBJ} -o a.out
main.o:main.c main_globals.h render.h pse.h pse_const.h
	${CC} ${CFLAGS} ${LDLIBS} -c main.c -o main.o
render.o:render.c render.h main_globals.h pse_const.h
	${CC} ${CFLAGS} -c render.c -o render.o
ball_api.o:ball_api.c ball_api.h pse_const.h
	${CC} ${CFLAGS} -c ball_api.c -o ball_api.o
pse.o:pse.c pse.h pse_const.h main_globals.h ball_api.h gen.h par.h
	${CC} ${CFLAGS} -c pse.c -o pse.o
gen.o:gen.c gen.h pse.h pse_const.h ball_api.h
	${CC} ${CFLAGS} -c gen.c -o gen.o
par.o:par.c par.h pse.h pse_const.h motion.h
	${CC} ${CFLAGS} -c par.c -o par.o
motion.o:motion.c motion.h pse_const.h
	${CC} ${CFLAGS} -c motion.c -o motion.o
collision.o:collision.c collision.h pse_const.h
	${CC} ${CFLAGS} -c collision.c -o collision.o
clean:
	rm -f *.out *.o
.PHONY: all clean
