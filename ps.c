#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include "pse_verlet_like.h"


void err_callback(int error, const char* desc);
void event_log(const char* desc);
void err_log(int error, const char* desc);
void scroll_cb(GLFWwindow *win, double x_offset, double y_offset);
void* prep_buff(BALL* balls,int num,int* size,int* stride);

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
			free(buff)
			glEnableVertexAttribArray(0);//pos_transform
			glVertexAttribPointer(0,16,GL_FLOAT,GL_FALSE,stride,0);
			glEnableVertexAttribArray(1);//color
			glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,stride,16*sizeof(float));
			glEnableVertexAttribArray(2);//radius
			glVertexAttribPointer(0,1,GL_FLOAT,GL_FALSE,stride,20*sizeof(float));
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
//load shaderm

////////input
	glfwSetScrollCallback(win_main,scroll_cb);
//////input

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

		///////update phy
		int size;
		void* buff=prep_buff(ball_buff,BALL_COUNT,&size,0);
		glBufferData(GL_ARRAY_BUFFER,size,buff,GL_DYNAMIC_DRAW);
		free(buff);buff=NULL;

		glDrawInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, BALL_COUNT);
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
	if (!stride){*stride=sizeof(struct a)}
	void* ret=malloc(*size);
	assert(ret&&"malloc failed");
	for(int i=0;i<num;++i){
		mat4 trans;
		vec3 y={ball[i]->pos[0],ball[i]->pos[1],0.0};
		glm_translate_make(trans,y);
		struct a x={
			 trans
			,balls[i]->color
			,ball[i]->radius};
		((*struct a)ret)[i]=a;
	}//for
	return ret;
}//fn

void////////////////////
scroll_cb(GLFWwindow *win, double x_offset, double y_offset){
	double zoom_factor=(zoom*y_offset);
	if (zoom_factor<0) zoom_factor=2+zoom_factor;
	double diff_x=(cp.x-cp.x_)*zoom_factor/2.0;
	double diff_y=(cp.y-cp.y_)*zoom_factor/2.0;
	double cx=540.0,cy=540.0;
	glfwGetCursorPos(win_main,&cx,&cy);
	glfwSetCursorPos(win_main,hw.w/2,hw.h/2);
	double cix,ciy;
	cix=cp.x_+(((double)hw.w-cx)/hw.w)*(cp.x - cp.x_);
	ciy=cp.y_+(cy/hw.h)*(cp.y - cp.y_);

	cp.x =(float)(cix+diff_x);
	cp.x_=(float)(cix-diff_x);
	cp.y =(float)(ciy+diff_y);
	cp.y_=(float)(ciy-diff_y);
	if(max_iter>10||y_offset>0)
		max_iter+=4*(int)y_offset;
}
