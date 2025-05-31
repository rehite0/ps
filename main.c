#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include "main_globals.h"
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
setup_cbs(void){
    glfwSetErrorCallback(err_callback);
    glfwSetWindowCloseCallback(win_main, win_close_cb);
//    glfwSetKeyCallback(win_main, key_cb);
//    glfwSetCharCallback(win_main, char_cb);	
//    glfwSetScrollCallback(win_main, scroll_cb);
//    glfwSetCursorPosCallback(win_main, cursor_position_cb);
//    glfwSetCursorEnterCallback(win_main, cursor_enter_cb);
//    glfwSetMouseButtonCallback(win_main, mouse_click_cb);
//    glfwSetDropCallback(win_main, drop_cb);
}

