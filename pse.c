#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "pse.h"
#include "pse_const.h"
#include "ball_api.h"
#include "gen.h"

unsigned long vtick=0;
unsigned long rtick=0;
unsigned long frameno=0;
double real_time=0;
struct ball_bufft ball_buff={0};
BALL MOUSE_BALL=(BALL)-1;

void pse_setup(void){
	ball_buff_create();
	MOUSE_BALL=ball_append(0., 0.
	     , 0., 0.
	     , 0.5
	     ,(DEFAULT|NO_DISPLAY|NO_COLLISION|NO_CONSTRAIN)
	     , 1.0, 1., 0., 1.);
	generate_random(MAX_SIZE-100);
}

void pse_exit(void){
	vtick=0;
	ball_buff_del();
}
void pse_update(void){
	++vtick;
	++rtick;
	++frameno;
	if(frameno%60==0){
		float frametime=(float)(glfwGetTime()-real_time);
		dprintf(2
			 ,"framerate:%f\n"
			 "frametime:%f\n"
			 "vtick:%lu\n"
			 "rtick:%lu\n"
			 "vtime:%f\n"
			 "rtime:%f\n"
			 "ball_num:%d\n"
			 "\n\n"
			 ,1.f/frametime,frametime,vtick,rtick,(float)vtick*vtick_time,real_time,ball_buff.len);
	}
	real_time=glfwGetTime();
	//usleep(1000000/95);
}

