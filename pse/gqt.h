void GQT_init();
void GQT_render(qtree qt);
void GQT_cleanup();

//global var
GLuint gqt_vao,gqt_vbo,gqt_pid;
GLuint u_gqt_hw;
void
GQT_init(){
	glGenVertexArrays(1,&gqt_vao);
	glGenBuffers(1,&gqt_vbo);

	glBindVertexArray(gqt_vao);
	glBlindBuffer(GL_ARRAY_BUFFER, gqt_vbo);

	glEnableVertexAttribArray(0);//pos
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,stride,0);


//load shader
		FILE *vsf, *fsf;
		char *bvs, *bfs;
		int vsl, fsl;

		if (	!(vsf=fopen("gqt.vert.glsl", "rb")) ||
				!(fsf=fopen("gqt.frag.glsl", "rb"))	)
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

		gqt_pid=glCreateProgram();
		glAttachShader(gqt_pid,vsid);
		glAttachShader(gqt_pid,fsid);
		
		glLinkProgram(gqt_pid);
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

		glUseProgram(gqt_pid);
		free(bvs);free(bfs);
//load shader
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//uniforms
	u_gqt_hw	=glGetUniformLocation(pid,"hw");
}


void
GQT_render(qtree qt){
		glBindVertexArray(gqt_vao);
		
		glUniform2f(u_hw,(float)hw.w,(float)hw.h);
	//data creation	
		void* buff=;
	//data initilisation
		if (size && buff){
			glBufferData(GL_ARRAY_BUFFER,size,buff,GL_DYNAMIC_DRAW);
			free(buff);
		}
		buff=NULL;

		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, BALL_COUNT);
}
