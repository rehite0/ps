#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include "main_globals.h"
#include "pse_const.h"
#include "render.h"
#include "pse.h"

static GLFWwindow* win_main=NULL;
int resolution_x=900; 
int resolution_y=900; 
unsigned long rtick; 
static int running=1;

void setup_cbs(void);

int
main(){
	assert((glfwInit() == GLFW_TRUE)&&"glfw intialise failed");
	win_main = glfwCreateWindow(resolution_x , resolution_y , "ps", NULL , NULL);
	assert(win_main&&"window creation failed");
	glfwMakeContextCurrent(win_main);
	setup_cbs();
	render_setup();
	pse_setup();

	while(running){
		glfwGetFramebufferSize(win_main,&resolution_x,&resolution_y);
		render_update();
		glfwSwapBuffers(win_main);
		glfwPollEvents();
	}

	glfwDestroyWindow(win_main);
	pse_exit();
	render_exit();
}
void 
err_callback(int error, const char* desc){
	fprintf(stderr,"error(%i):%s\n",error,desc);
}
void
win_close_cb(GLFWwindow* winid){
	(void)winid;
	running=0;
}
void
cursor_position_cb(GLFWwindow* winid, double xpos, double ypos){
	(void)winid;
	if(MOUSE_BALL!=(BALL)-1){
		ball_buff.posx[MOUSE_BALL]=(float)((xpos/resolution_x)*2.0-1.0);
		ball_buff.posy[MOUSE_BALL]=(float)((ypos/resolution_y)*-2.0+1.0);
	}
}
void
mouse_click_cb(GLFWwindow* winid, int button, int action, int mods){
	(void)winid;
	(void)mods;
	/*
	if (button==GLFW_MOUSE_BUTTON_LEFT &&
		action==GLFW_RELEASE){
		double xpos,ypos;
		glfwGetCursorPos(win,&xpos,&ypos);
		genclick((float) ( (xpos-(double)hw.w/2.0)*2.0 /(double)hw.w)
				,(float) ( (ypos-(double)hw.h/2.0)*2.0 /(double)hw.h)*-1.0
				,0.0,0.05,NULL	);
	}*/
	if (button==GLFW_MOUSE_BUTTON_RIGHT && MOUSE_BALL!=(BALL)-1){
		if (action==GLFW_RELEASE)
			ball_buff.flag[MOUSE_BALL]|=NO_DISPLAY|NO_COLLISION;
		if (action==GLFW_PRESS)
			ball_buff.flag[MOUSE_BALL]&=~(unsigned int)(NO_DISPLAY|NO_COLLISION);
	}
}

void
setup_cbs(void){
	glfwSetErrorCallback(err_callback);
	glfwSetWindowCloseCallback(win_main, win_close_cb);
	glfwSetMouseButtonCallback(win_main, mouse_click_cb);
	glfwSetCursorPosCallback(win_main, cursor_position_cb);
	//    glfwSetKeyCallback(win_main, key_cb);
	//    glfwSetCharCallback(win_main, char_cb);	
	//    glfwSetScrollCallback(win_main, scroll_cb);
	//    glfwSetCursorEnterCallback(win_main, cursor_enter_cb);
	//    glfwSetDropCallback(win_main, drop_cb);
}

