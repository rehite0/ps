
CFLAGS=-Wall -Wextra `pkg-config --libs --cflags glfw3 glew` -lm -ldl

all: a.out
	@echo '_______________compilation finished_________________'
a.out: ps.c ps.frag.glsl ps.vert.glsl pse_verlet.h 
	cc $(CFLAGS) ps.c -o a.out 2>cmpl.log || ! less cmpl.log
clean:
	rm *.out *.o
refresh_tags:
	rm tags
	ctags *.c /usr/include/{GLFW/glfw3.h,GL/glew.h,stdio.h,stdio.h,cglm/*}
