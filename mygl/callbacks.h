#pragma once

void
key_cb(GLFWwindow* win, int key, int scancode, int action, int mods)
{(void*)0;}//fn

void
char_cb(GLFWwindow* win, unsigned int codepoint)
{(void*)0;}//fn

void
scroll_cb(GLFWwindow *win, double x_offset, double y_offset)
{(void*)0;}//fn

void
cursor_position_cb	(GLFWwindow* win, double xpos, double ypos)
{	if (mouse_ball){
		mouse_ball->pos[0]=(float)(xpos/hw.w)*2.0-1.0;
		mouse_ball->pos[1]=(float)(ypos/hw.h)*-2.0+1.0;
	}
}

void
cursor_enter_cb	(GLFWwindow* win, int entered)
{(void*)0;}

void
mouse_click_cb(GLFWwindow* win, int button, int action, int mods){
	if (button==GLFW_MOUSE_BUTTON_LEFT &&
		action==GLFW_RELEASE){
		double xpos,ypos;
		glfwGetCursorPos(win,&xpos,&ypos);
		genclick((float) ( (xpos-(double)hw.w/2.0)*2.0 /(double)hw.w)
				,(float) ( (ypos-(double)hw.h/2.0)*2.0 /(double)hw.h)*-1.0
				,0.0,0.05,NULL	);
	}//if
	if (button==GLFW_MOUSE_BUTTON_RIGHT){
		assert(mouse_ball && "mouse ball is not present");
		if (action==GLFW_RELEASE)
			mouse_ball->flag=mouse_ball->flag|NO_DISPLAY|NO_COLLISION;
		if (action==GLFW_PRESS)
			mouse_ball->flag=mouse_ball->flag&(~NO_DISPLAY)&(~NO_COLLISION);
	}//if
}//fn

void
drop_cb(GLFWwindow* window, int path_count, const char* paths[])
{(void*)0;}
