#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>
//#define CGLM_DEFINE_PRINTS
#include <cglm/cglm.h>
//#include <cglm/io.h>
//#define d(a) fprintf(stdout,"this is bp (" #a ")\n");
#include "pse/main.h"
#include "mygl/main.h"


void err_callback(int error, const char* desc);
void event_log(const char* desc);
void err_log(int error, const char* desc);

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
			if (size && buff){
				glBufferData(GL_ARRAY_BUFFER,size,buff,GL_DYNAMIC_DRAW);
				free(buff);
			}
			glEnableVertexAttribArray(0);//color
			glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,stride,0);
			glEnableVertexAttribArray(1);//radius
			glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,stride,(void*)(sizeof(vec4)+sizeof(mat4)));
			glEnableVertexAttribArray(2);//pos_transform row1
			glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,stride,(void*)(1*sizeof(vec4)));
			glEnableVertexAttribArray(3);//pos_transform row2
			glVertexAttribPointer(3,4,GL_FLOAT,GL_FALSE,stride,(void*)(2*sizeof(vec4)));
			glEnableVertexAttribArray(4);//pos_transform row3
			glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,stride,(void*)(3*sizeof(vec4)));
			glEnableVertexAttribArray(5);//pos_transform row4
			glVertexAttribPointer(5,4,GL_FLOAT,GL_FALSE,stride,(void*)(4*sizeof(vec4)));

			glVertexAttribDivisor(0,1);
			glVertexAttribDivisor(1,1);
			glVertexAttribDivisor(2,1);
			glVertexAttribDivisor(3,1);
			glVertexAttribDivisor(4,1);
			glVertexAttribDivisor(5,1);
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
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
		free(bvs);free(bfs);
//load shader

		model_setup();

//input callback
	glfwSetKeyCallback			(win_main, key_cb);
	glfwSetCharCallback			(win_main, char_cb);	
	glfwSetScrollCallback		(win_main, scroll_cb);
	glfwSetCursorPosCallback	(win_main, cursor_position_cb);
	glfwSetCursorEnterCallback	(win_main, cursor_enter_cb);
	glfwSetMouseButtonCallback	(win_main, mouse_click_cb);
	glfwSetDropCallback			(win_main, drop_cb);
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
		glUniform2f(u_hw,(float)hw.w,(float)hw.h);
		
		//double mx,my;
	//	glfwGetCursorPos(win_main,&mx,&my);
	//	mouse_ball->pos[0]=(float)(mx/hw.w)*2.0-1.0;
	//	mouse_ball->pos[1]=(float)(my/hw.h)*-2.0+1.0;

		update_model();
		int size;
		void* buff=prep_buff(ball_buff,BALL_COUNT,&size,0);

		if (size && buff){
			glBufferData(GL_ARRAY_BUFFER,size,buff,GL_DYNAMIC_DRAW);
			free(buff);
		}
		buff=NULL;

		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, BALL_COUNT);
		glfwSwapBuffers(win_main);
		glfwPollEvents();
		++frameno;
	}//while

	//clean up
	glfwDestroyWindow(win_main);
	if (ball_buff) free_model(ball_buff);
	if (qt) qt_free(qt);
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

