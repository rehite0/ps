#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <cglm/cglm.h>
#include "pse_verlet_like.h"


void err_callback(int error, const char* desc);
void event_log(const char* desc);
void err_log(int error, const char* desc);
void scroll_cb(GLFWwindow *win, double x_offset, double y_offset);
void mouse_click_cb(GLFWwindow* win, int button, int action, int mods);
void* prep_buff(BALL** balls,int num,int* size,int* stride);

GLFWwindow* win_main =NULL;
struct win_hw{
	int h;
	int w;
}hw={1080,1080};

int 
main(void)
{
	glfwSetErrorCallback(err_callback);
	if (glfwInit() != GLFW_TRUE) {err_log(1,"glfw failed to intialize.");}

//creating window
	win_main = glfwCreateWindow(hw.w , hw.h   , "ps", NULL   , NULL);
                                        //width, height, title, monitor, share
	if (!win_main) {err_log(1,"window creation failed");}
	
	glfwMakeContextCurrent(win_main);

	if (glewInit() !=GLEW_OK){err_log(1,"glewinit failed");}

	GLuint vao,vbo,pid;
	
//load data
{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		int size,stride;
		void* buff=prep_buff(ball_buff,BALL_COUNT,&size,&stride);

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER,size,buff,GL_DYNAMIC_DRAW);
			free(buff);
			glEnableVertexAttribArray(0);//pos_transform
			glVertexAttribPointer(0,16,GL_FLOAT,GL_FALSE,stride,0);
			glEnableVertexAttribArray(1);//color
			glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,stride,(void*)(16*sizeof(float)));
			glEnableVertexAttribArray(2);//radius
			glVertexAttribPointer(0,1,GL_FLOAT,GL_FALSE,stride,(void*)(20*sizeof(float)));
			glVertexAttribDivisor(0,1);
			glVertexAttribDivisor(1,1);
			glVertexAttribDivisor(2,1);
}
//load data

//load shader
		FILE *vsf, *fsf;
		char *bvs, *bfs;
		int vsl, fsl;

		if (	!(vsf=fopen("ps.vert.glsl", "rb")) ||
				!(fsf=fopen("ps.frag.glsl", "rb"))	)
			err_log(1,"shader opening failed");
		
		fseek(vsf, 0, SEEK_END);vsl=ftell(vsf);
		bvs=(char*)malloc(vsl+1);
		fseek(vsf, 0, SEEK_SET);
		fread(bvs, vsl, 1, vsf);fclose(vsf);
		bvs[vsl]=0;

		fseek(fsf, 0, SEEK_END);fsl=ftell(fsf);
		bfs=(char*)malloc(fsl+1);
		fseek(fsf, 0, SEEK_SET);
		fread(bfs, fsl, 1, fsf);fclose(fsf);
		bfs[fsl]=0;

		GLuint vsid=glCreateShader(GL_VERTEX_SHADER);
		GLuint fsid=glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vsid, 1, (const GLchar**)&bvs, 0);
		glShaderSource(fsid, 1, (const GLchar**)&bfs, 0);

		glCompileShader(vsid);
		glCompileShader(fsid);

		pid=glCreateProgram();
		glAttachShader(pid,vsid);
		glAttachShader(pid,fsid);
		
		glLinkProgram(pid);
	//catch error in shader code
		int iscomp,maxlen;
		char *InfoLog;

		glGetShaderiv(fsid, GL_COMPILE_STATUS, &iscomp);
		if(iscomp == 0)
		{
			glGetShaderiv(fsid, GL_INFO_LOG_LENGTH, &maxlen);
			InfoLog = (char *)malloc(maxlen);

			glGetShaderInfoLog(fsid, maxlen, &maxlen, InfoLog);
			printf("shader error:%s",InfoLog);
			free(InfoLog);free(bvs);free(bfs);
			exit(0);
		}

		glGetShaderiv(vsid, GL_COMPILE_STATUS, &iscomp);
		if(iscomp == 0)
		{
			glGetShaderiv(vsid, GL_INFO_LOG_LENGTH, &maxlen);
			InfoLog = (char *)malloc(maxlen);

			glGetShaderInfoLog(fsid, maxlen, &maxlen, InfoLog);
			printf("shader error:%s",InfoLog);
			free(InfoLog);free(bvs);free(bfs);
			exit(0);

		}

		glUseProgram(pid);

		free(bvs);free(bfs);
//load shader

//input callback
	//glfwSetScrollCallback(win_main, scroll_cb);
	//glfwSetCursorPosCallback(win_main, cursor_position_callback);
	//glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetMouseButtonCallback(win_main, mouse_click_cb);
//input

	unsigned int u_tdt,u_hw;
		u_tdt	=glGetUniformLocation(pid,"tdt");
		u_hw	=glGetUniformLocation(pid,"hw");


	//event loop
	while (!glfwWindowShouldClose(win_main)){
		glfwGetFramebufferSize(win_main,&hw.w,&hw.h);
		glViewport(0,0,hw.w,hw.h);

		glClearColor(1.0,1.0,1.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vao);
		
		dt=glfwGetTime()-t;t=glfwGetTime();
		glUniform2f(u_tdt,t,dt);
		glUniform2i(u_hw,hw.w,hw.h);

		update_model();
		int size;
		void* buff=prep_buff(ball_buff,BALL_COUNT,&size,0);
		glBufferData(GL_ARRAY_BUFFER,size,buff,GL_DYNAMIC_DRAW);
		free(buff);buff=NULL;

		glDrawArraysInstanced(GL_TRIANGLES, 6, 0, BALL_COUNT);
		glfwSwapBuffers(win_main);
		glfwPollEvents();
	}//while

	//clean up
	glfwDestroyWindow(win_main);
	err_log(0,"application terminated");
}//main
 


void 
err_log(int error, const char* desc){
	if (!error)
	{	fprintf(stdout,"exit message:%s\n",desc);}
	else
	{	fprintf(stderr,"error:%s\n",desc);}
	glfwTerminate();
	exit(error);
}

void 
err_callback(int error, const char* desc){
	fprintf(stderr,"error(%i):%s\n",error,desc);
}

void
event_log(const char* desc){
	fprintf(stdout,"event:%s\n",desc);
}

void*
prep_buff(BALL** balls,int num,int* size,int* stride){
	struct a{
		mat4 transform;
		vec4 color;
		float radius;	};
	*size=sizeof(struct a)*num;
	if (!stride){*stride=sizeof(struct a);}
	void* ret=malloc(*size);
	assert(ret&&"malloc failed");
	for(int i=0;i<num;++i){
		mat4 trans;
		vec3 y={balls[i]->pos[0],balls[i]->pos[1],0.0};
		glm_translate_make(trans,y);
		struct a x={
			{{	trans[0][0],	trans[0][1],	trans[0][2],	trans[0][3]	}
			,{	trans[1][0],	trans[1][1],	trans[1][2],	trans[1][3]	}
			,{	trans[2][0],	trans[2][1],	trans[2][2],	trans[2][3]	}
			,{	trans[3][0],	trans[3][1],	trans[3][2],	trans[3][3]	}
			}
			,{	balls[i]->color[0],
				balls[i]->color[1],
				balls[i]->color[2],
				balls[i]->color[3]
			}
			,balls[i]->rad};
		((struct a *)ret)[i]=x;
	}//for i
	return ret;
}//fn

void
scroll_cb(GLFWwindow *win, double x_offset, double y_offset){
	//not used
	(void*)0;
}
void
mouse_click_cb(GLFWwindow* win, int button, int action, int mods){
	if (button==GLFW_MOUSE_BUTTON_LEFT &&
		action==GLFW_RELEASE){
		double xpos,ypos;
		glfwGetCursorPos(win,&xpos,&ypos);
		genclick((float)xpos,(float)ypos);
	}//if
}//fn
