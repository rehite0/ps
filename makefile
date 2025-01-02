
CFLAGS= -g -Wall -Wextra `pkg-config --libs --cflags glfw3 glew` -lm -ldl

all: a.out
	@echo '_______________compilation finished_________________'
a.out: ps.c ps.frag.glsl ps.vert.glsl pse/* mygl/* 
	cc $(CFLAGS) ps.c -o a.out
clean:
	rm *.out *.o
