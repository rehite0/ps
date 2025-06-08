#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "main_globals.h"
#include "pse_const.h"
#include "render.h"

void render_setup(void);
void render_exit(void);
void render_update(void);
static GLuint _compile_shader(
	char* file
	,char c	// f=frag v=vert  c=compute
);
static void
_buffer_init(GLuint* bid,int size);

static GLuint vao,pid
	,bposx
	,bposy
	,bpposx
	,bpposy
	,brad
	,bflag
	,bcolor;
void render_setup(void){
	assert((glewInit()==GLEW_OK)&&"glew init failed");
	//intialise buffer
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	_buffer_init(&bposx,sizeof(*(((struct ball_bufft*)0)->posx)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,1,GL_FLOAT,GL_FALSE,0,0);
	_buffer_init(&bposy,sizeof(*(((struct ball_bufft*)0)->posy)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,0,0);
	_buffer_init(&bpposx,sizeof(*(((struct ball_bufft*)0)->pposx)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,0,0);
	_buffer_init(&bpposy,sizeof(*(((struct ball_bufft*)0)->pposy)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,0,0);
	_buffer_init(&brad,sizeof(*(((struct ball_bufft*)0)->rad)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4,1,GL_FLOAT,GL_FALSE,0,0);
	_buffer_init(&bflag,sizeof(*(((struct ball_bufft*)0)->flag)));
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5,1,GL_UNSIGNED_INT,0,0);
	_buffer_init(&bcolor,sizeof(*(((struct ball_bufft*)0)->color)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6,4,GL_FLOAT,GL_FALSE,0,0);

	glVertexAttribDivisor(0,1);
	glVertexAttribDivisor(1,1);
	glVertexAttribDivisor(2,1);
	glVertexAttribDivisor(3,1);
	glVertexAttribDivisor(4,1);
	glVertexAttribDivisor(5,1);
	glVertexAttribDivisor(6,1);
	//load shader
	pid=glCreateProgram();
	glAttachShader(pid,_compile_shader("ps.vert.glsl",'v'));
	glAttachShader(pid,_compile_shader("ps.frag.glsl",'f'));
	glLinkProgram(pid);
	glUseProgram(pid);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

	// unsigned int u_tdt,u_hw;
	// u_tdt	=glGetUniformLocation(pid,"tdt");
	// u_hw	=glGetUniformLocation(pid,"hw");
}
void render_exit(void){
	glDeleteVertexArrays(1,&vao);
	glDeleteBuffers(1,&bposx);
	glDeleteBuffers(1,&bposy);
	glDeleteBuffers(1,&bpposx);
	glDeleteBuffers(1,&bpposy);
	glDeleteBuffers(1,&brad);
	glDeleteBuffers(1,&bflag);
	glDeleteBuffers(1,&bcolor);
	glDeleteProgram(pid); 
}
void render_update(void){
	glViewport(0,0,resolution_x,resolution_y);
	glClearColor(1.0,1.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vao);
	glUseProgram(pid);
	glNamedBufferData(bposx
		   ,ball_buff.len*sizeof(*(ball_buff.posx))
		   ,ball_buff.posx,GL_STREAM_DRAW);
	glNamedBufferData(bposy
		   ,ball_buff.len*sizeof(*(ball_buff.posy))
		   ,ball_buff.posy,GL_STREAM_DRAW);
	glNamedBufferData(bpposx
		   ,ball_buff.len*sizeof(*(ball_buff.pposx))
		   ,ball_buff.pposx,GL_STREAM_DRAW);
	glNamedBufferData(bpposy
		   ,ball_buff.len*sizeof(*(ball_buff.pposy))
		   ,ball_buff.pposy,GL_STREAM_DRAW);
	glNamedBufferData(brad
		   ,ball_buff.len*sizeof(*(ball_buff.rad))
		   ,ball_buff.rad,GL_STREAM_DRAW);
	glNamedBufferData(bflag
		   ,ball_buff.len*sizeof(*(ball_buff.flag))
		   ,ball_buff.flag,GL_STREAM_DRAW);
	glNamedBufferData(bcolor
		   ,ball_buff.len*sizeof(*(ball_buff.color))
		   ,ball_buff.color,GL_STREAM_DRAW);

	// dt=glfwGetTime()-t;t=glfwGetTime();
	// glUniform2f(u_tdt,t,dt);
	// glUniform2f(u_hw,(float)hw.w,(float)hw.h);

	//update buffer&draw
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6,(int) ball_buff.len);
}

static GLuint
_compile_shader(
	char* file
	,char c	// f=frag v=vert  c=compute(not implimented)
){
	FILE *fh;
	char *buff;
	unsigned long len;
	GLenum shader_type;
	switch(c){
		case 'f':
			shader_type=GL_FRAGMENT_SHADER;
			break;
		case 'v':
			shader_type=GL_VERTEX_SHADER;
			break;
		case 'c':(void)0;
		default:assert(0&&"wrong flag in compilation");
	}
	if (!(fh=fopen(file, "rb")))
		assert(0&&"shader opening failed");
	fseek(fh, 0, SEEK_END);
	len=(unsigned long)ftell(fh);
	buff=(char*)malloc(len+1);
	fseek(fh, 0, SEEK_SET);
	fread(buff, len, 1, fh);
	fclose(fh);
	buff[len]=0;
	GLuint sid=glCreateShader(shader_type);
	glShaderSource(sid, 1, (const GLchar**)&buff, 0);
	glCompileShader(sid);
	free(buff);

	int iscomp, maxlen;
	char *InfoLog;

	glGetShaderiv(sid, GL_COMPILE_STATUS, &iscomp);
	if(iscomp == 0)
	{
		glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &maxlen);
		InfoLog = (char *)malloc((unsigned long)maxlen);

		glGetShaderInfoLog(sid, (unsigned long)maxlen, &maxlen, InfoLog);
		printf("shader error:%s",InfoLog);
		free(InfoLog);
		exit(0);
	}
	return sid;
}
static void
_buffer_init(GLuint* bid,int size){
	glGenBuffers(1, bid);
	glBindBuffer( GL_ARRAY_BUFFER,*bid);
	glBufferData(GL_ARRAY_BUFFER,MAX_SIZE*size,NULL,GL_STREAM_DRAW);
}
