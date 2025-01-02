#pragma once

void
model_setup(){
	//mouse ball setup
	BALL* a=malloc(sizeof(BALL));
	assert(a &&"malloc failed");
	ball_buff=realloc(ball_buff,sizeof(BALL*)*(++BALL_COUNT));
	assert(ball_buff && "realloc failed");
	a->pos[0]=0.0;		a->pos[1]=0.0;
	a->ppos[0]=0.0;		a->ppos[1]=0.0;
	a->acc[0]=0.0;		a->acc[1]=0.0;
	a->color[0]=1.0;	a->color[1]=1.0;	a->color[2]=0.0;	a->color[3]=1.0;
	a->rad=0.05;		a->flag=NO_FORCE|NO_CONSTRAIN|NO_MOVE|NO_COLLISION|NO_DISPLAY;
	ball_buff[BALL_COUNT-1]=a;
	mouse_ball=a;
	
	#ifdef gen_rand
		for (int i=0;i<gen_rand;++i)
			genclick(-1.0+((double)rand()*2.0/RAND_MAX) 
					,-1.0+((double)rand()*2.0/RAND_MAX)
					, 0.0 , 0.0, NULL );
	#endif
	
	#ifdef use_qt
		qt=qt_create((bod){1.0,-1.0,1.0,-1.0});
	#endif
}//fn

void
update_model(){
	//color varring ball
	float r_bias=1.0,
		  g_bias=1.0,
		  b_bias=1.0,
		  cycle_speed=0.1;
	ball_bp.color[0]=pow(cos(cycle_speed*r_bias*t),2);
	ball_bp.color[1]=pow(cos(cycle_speed*g_bias*t+2.0944),2);
	ball_bp.color[2]=pow(cos(cycle_speed*b_bias*t+4.1887),2);

	
	if (frameno%60==0){
		fprintf(stdout,"ball count:%i \nframerate:%lf\n\n",BALL_COUNT,(double)1/dt);
	}//if
	//sleep(1.4);//////////////////////////////////////////
	for (int i=0;i<substeps;++i){
		iter_phy()	;
		#ifdef use_qt
			coll_dect_qt();
		#else
			coll_dect() ;
		#endif
	}//for
#ifdef gen_stream
	if (frameno%(int)((C_RAD+0.001)*2.0/(fdt*0.5*substeps))==0){
		float sx=-1+C_RAD,
			  sy=1.0-C_RAD,
			  dy=2*C_RAD,
			  vx=0.5,
			  vy=-0.00;
		for (int i=0;i<gen_stream;++i)
			genclick(sx	,sy-dy*i, vx	, vy*(i+1)  , NULL);
	}
	#endif

}//fn

void
iter_phy(){
	int i;
	for (i=0;i<BALL_COUNT;++i){
		if (ckflg(ball_buff[i]->flag,NO_MOVE)) continue;
			
		float px=ball_buff[i]->ppos[0],py=ball_buff[i]->ppos[1];
		ball_buff[i]->ppos[0]=ball_buff[i]->pos[0];
		ball_buff[i]->ppos[1]=ball_buff[i]->pos[1];

		ball_buff[i]->pos[0]
			=ball_buff[i]->pos[0]*2.0
			-px
			+ball_buff[i]->acc[0]*fdt*fdt;
		ball_buff[i]->pos[1]
			=ball_buff[i]->pos[1]*2.0
			-py
			+ball_buff[i]->acc[1]*fdt*fdt;
	
		ball_buff[i]->acc[0]=0.0;
		ball_buff[i]->acc[1]=0.0;

		if (ckflg(ball_buff[i]->flag,NO_FORCE)) continue;
		for (int j=0;j<force_num;j++){
			(*force_buff[j])(ball_buff[i]);
			#ifdef use_qt
				qt_insert(ball_buff[i],qt);
			#endif
		}//for j
	}//for i
}//fn

void
genclick(float x, float y, float vx, float vy, BALL* b){
	if (b==NULL)
		b=&ball_bp;
	BALL* a=malloc(sizeof(BALL));
	assert(a && "malloc failed");
	*a=ball_bp;
	ball_buff=realloc(ball_buff,sizeof(BALL*)*(++BALL_COUNT));
	assert(ball_buff &&"null return by realloc!");
	ball_buff[BALL_COUNT-1]=a;
	ball_buff[BALL_COUNT-1]->pos[0]=x;
	ball_buff[BALL_COUNT-1]->pos[1]=y;
	ball_buff[BALL_COUNT-1]->ppos[0]=vtop(vx,x);
	ball_buff[BALL_COUNT-1]->ppos[1]=vtop(vy,y);
}//fn

void
free_model(){
	for (int i=0;i<BALL_COUNT;++i){
		free(ball_buff[i]);
	}//for i

	free(ball_buff);
	ball_buff=NULL;
	BALL_COUNT=0;
}//fn

void*
prep_buff(BALL** balls,int num,int* size,int* stride){
	struct a{
		vec4 color;
		mat4 trans;
		float radius;
	};
	*size=sizeof(struct a)*num;
	if (stride){*stride=sizeof(struct a);}
	if (!(*size)){return NULL;}
	void* ret=malloc(*size);
	assert(ret&&"malloc failed");
	for(int i=0;i<num;++i){
		if (ckflg(balls[i]->flag,NO_DISPLAY)) continue ;

		vec3 y={balls[i]->pos[0],balls[i]->pos[1],0.0};
		
		struct a x;
		x.color[0]=balls[i]->color[0];
		x.color[1]=balls[i]->color[1];
		x.color[2]=balls[i]->color[2];
		x.color[3]=balls[i]->color[3];
		x.radius=balls[i]->rad;
		glm_translate_make(x.trans,y);
		((struct a *)ret)[i]=x;
	}//for i
	return ret;
}//fn
