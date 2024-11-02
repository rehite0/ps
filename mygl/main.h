#pragma once

//globles var
GLFWwindow* win_main =NULL;
struct win_hw{
	int h;
	int w;
}hw={1080,1080};

//fn prototypes
void key_cb				(GLFWwindow* win, int key, int scancode, int action, int mods);
void char_cb			(GLFWwindow* win, unsigned int codepoint);
void scroll_cb			(GLFWwindow *win, double x_offset, double y_offset);
void cursor_position_cb	(GLFWwindow* win, double xpos, double ypos);
void cursor_enter_cb	(GLFWwindow* win, int entered);
void mouse_click_cb		(GLFWwindow* win, int button, int action, int mods);
void drop_cb			(GLFWwindow* win, int path_count, const char* paths[]);
#include "callbacks.h"
