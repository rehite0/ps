#include <GL/glew.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "pse.h"
#include "pse_const.h"
#include "ball_api.h"

unsigned long vtick;
struct ball_bufft ball_buff={0};
BALL MOUSE_BALL=(BALL)-1;

void pse_setup(void){
	vtick=0;
	ball_buff_create();
	MOUSE_BALL=ball_append(0., 0.
	     , 0., 0.
	     , 0.5
	     ,(DEFAULT|NO_DISPLAY|NO_COLLISION)
	     , 1.0, 1., 0., 1.);
}

void pse_exit(void){
	vtick=0;
	ball_buff_del();
}
void pse_update(void){
	++vtick;
}

