#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "render.h"

GLuint vao,vbo,pid;
void render_setup(void){
    assert((glewInit()==GLEW_OK)&&"glew init failed");
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // int size,stride;
    // void* buff=prep_buff(ball_buff,BALL_COUNT,&size,&stride);

    // glBindVertexArray(vao);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // if (size && buff){
	    // glBufferData(GL_ARRAY_BUFFER,size,buff,GL_DYNAMIC_DRAW);
	    // free(buff);
    // }
    // glEnableVertexAttribArray(0);//color
    // glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,stride,0);
    // glEnableVertexAttribArray(1);//radius
    // glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,stride,(void*)(sizeof(vec4)+sizeof(mat4)));
    // glEnableVertexAttribArray(2);//pos_transform row1
    // glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,stride,(void*)(1*sizeof(vec4)));
    // glEnableVertexAttribArray(3);//pos_transform row2
    // glVertexAttribPointer(3,4,GL_FLOAT,GL_FALSE,stride,(void*)(2*sizeof(vec4)));
    // glEnableVertexAttribArray(4);//pos_transform row3
    // glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,stride,(void*)(3*sizeof(vec4)));
    // glEnableVertexAttribArray(5);//pos_transform row4
    // glVertexAttribPointer(5,4,GL_FLOAT,GL_FALSE,stride,(void*)(4*sizeof(vec4)));

    // glVertexAttribDivisor(0,1);
    // glVertexAttribDivisor(1,1);
    // glVertexAttribDivisor(2,1);
    // glVertexAttribDivisor(3,1);
    // glVertexAttribDivisor(4,1);
    // glVertexAttribDivisor(5,1);
//load shader
    FILE *vsf, *fsf;
    char *bvs, *bfs;
    unsigned long vsl, fsl;

    if (!(vsf=fopen("ps.vert.glsl", "rb")) ||
	!(fsf=fopen("ps.frag.glsl", "rb")) )
	assert(0&&"shader opening failed");
    
    fseek(vsf, 0, SEEK_END);vsl=(unsigned long)ftell(vsf);
    bvs=(char*)malloc(vsl+1);
    fseek(vsf, 0, SEEK_SET);
    fread(bvs, vsl, 1, vsf);fclose(vsf);
    bvs[vsl]=0;

    fseek(fsf, 0, SEEK_END);fsl=(unsigned long)ftell(fsf);
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
    int iscomp, maxlen;
    char *InfoLog;

    glGetShaderiv(fsid, GL_COMPILE_STATUS, &iscomp);
    if(iscomp == 0)
    {
	    glGetShaderiv(fsid, GL_INFO_LOG_LENGTH, &maxlen);
	    InfoLog = (char *)malloc((unsigned long)maxlen);

	    glGetShaderInfoLog(fsid, (unsigned long)maxlen, &maxlen, InfoLog);
	    printf("shader error:%s",InfoLog);
	    free(InfoLog);free(bvs);free(bfs);
	    exit(0);
    }

    glGetShaderiv(vsid, GL_COMPILE_STATUS, &iscomp);
    if(iscomp == 0)
    {
	    glGetShaderiv(vsid, GL_INFO_LOG_LENGTH, &maxlen);
	    InfoLog = (char *)malloc((unsigned long)maxlen);

	    glGetShaderInfoLog(fsid, maxlen, &maxlen, InfoLog);
	    printf("shader error:%s",InfoLog);
	    free(InfoLog);free(bvs);free(bfs);
	    exit(0);

    }

    glUseProgram(pid);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    free(bvs);free(bfs);

    // unsigned int u_tdt,u_hw;
	    // u_tdt	=glGetUniformLocation(pid,"tdt");
	    // u_hw	=glGetUniformLocation(pid,"hw");
}
void render_exit(void){
   (void)0; 
}
void render_update(void){
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //glBindVertexArray(vao);
    //bind pid
    
    // dt=glfwGetTime()-t;t=glfwGetTime();
    // glUniform2f(u_tdt,t,dt);
    // glUniform2f(u_hw,(float)hw.w,(float)hw.h);

    //update buffer&draw
    //glBufferData(GL_ARRAY_BUFFER,size,buff,GL_DYNAMIC_DRAW);
    //glDrawArraysInstanced(GL_TRIANGLES, 0, 6, BALL_COUNT);
}
