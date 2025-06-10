CC=gcc
CFLAGS= -Ofast  -ggdb -march='native'
CFLAGS+= -Wall -Wextra -Wpedantic -Wconversion -Werror -Winline -Wshadow -Wfloat-equal 
CFLAGS+= -Wunreachable-code -Winit-self -Wuninitialized
#CPPFLAGS=
#LDFLAGS=
LDLIBS= `pkg-config --libs --cflags glfw3 glew`

all: a.out
	@echo 'compilation finished'
a.out:main.o render.o ball_api.o pse.o gen.o
	${CC} ${CFLAGS} ${LDLIBS} main.o render.o ball_api.o pse.o gen.o -o a.out
main.o:main.c main_globals.h render.h pse.h pse_const.h
	${CC} ${CFLAGS} ${LDLIBS} -c main.c -o main.o
render.o:render.c render.h main_globals.h pse_const.h
	${CC} ${CFLAGS} ${LDLIBS} -c render.c -o render.o
ball_api.o:ball_api.c ball_api.h pse_const.h
	${CC} ${CFLAGS} ${LDLIBS} -c ball_api.c -o ball_api.o
pse.o:pse.c pse.h pse_const.h ball_api.h gen.h
	${CC} ${CFLAGS} ${LDLIBS} -c pse.c -o pse.o
gen.o:gen.c gen.h pse.h pse_const.h ball_api.h
	${CC} ${CFLAGS} ${LDLIBS} -c gen.c -o gen.o
clean:
	rm -f *.out *.o
.PHONY: all clean
# add x.o target
# update all headers
# modify a.out target requirement
