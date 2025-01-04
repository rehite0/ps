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
	a->rad=0.12;		a->flag=NO_FORCE|NO_CONSTRAIN|NO_MOVE|NO_COLLISION|NO_DISPLAY;
	ball_buff[BALL_COUNT-1]=a;
	mouse_ball=a;
	
	#ifdef gen_rand
		for (int i=0;i<gen_rand;++i){
			double t=((double)rand()*3.5/RAND_MAX);
			ball_bp.color[0]=pow(cos(t),2);
			ball_bp.color[1]=pow(cos(t+2.0944),2);
			ball_bp.color[2]=pow(cos(t+4.1887),2);

			genclick(-1.0+((double)rand()*2.0/RAND_MAX) 
					,-1.0+((double)rand()*2.0/RAND_MAX)
					, 0.0 , 0.0, NULL );
		}
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
		#ifdef mthreads
			iter_phy_mt();
		#else
			iter_phy();
		#endif

		#if defined(mthreads)
			coll_dect_mt();
		#elif defined(use_qt)
			coll_dect_qt();
		#else
			coll_dect();
		#endif
	}//for
#ifdef gen_stream
	if (frameno%(int)((C_RAD+0.003)*2.0/(fdt*0.5*substeps))==0){
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
phy_logic(int i){
	if (ckflg(ball_buff[i]->flag,NO_MOVE)) return;
			
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

	#if defined(use_qt) && !defined(mthreads)
		qt_insert(ball_buff[i],qt);
	#endif

	if (ckflg(ball_buff[i]->flag,NO_FORCE)) return;
	for (int j=0;j<force_num;j++)
		(*force_buff[j])(ball_buff[i]);

}
void
iter_phy(){
	for (int i=0;i<BALL_COUNT;++i){
		phy_logic(i);
	}//for i
}//fn

#ifdef mthreads
void*
_iter_phy_mt_routine(void* ptr){
	int start=((int*)ptr)[0],stop=((int*)ptr)[1];
	for (int i=start;i<stop;++i)
		phy_logic(i);	
}
void
iter_phy_mt(){
	int work_idx[mthreads+1][2];
	int num_work=BALL_COUNT/(mthreads+1);
	for (int i=0;i<mthreads;++i){
		work_idx[i][0]=i*num_work;
		work_idx[i][1]=(i+1)*num_work;
		pthread_create(&(tid[i]),NULL,&_iter_phy_mt_routine,(void*)(work_idx[i]));
	}
	work_idx[mthreads][0]=mthreads*num_work;
	work_idx[mthreads][1]=BALL_COUNT;
	_iter_phy_mt_routine(work_idx[mthreads]);
	for (int i=0;i<mthreads;++i)
		pthread_join(tid[i],NULL);

	#if defined(use_qt)///////////////////////////////////////////////////////////////////
	for (int i=0;i<BALL_COUNT;++i){
		qt_insert(ball_buff[i],qt);
	}//for i
	#endif
}
#endif

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
prep_buff(int* size,int* stride){
#ifdef mthread
	return prep_buff_mt(size,stride);
#else
	struct a{
		vec4 color;
		mat4 trans;
		float radius;
	};
	*size=sizeof(struct a)*BALL_COUNT;
	if (stride){*stride=sizeof(struct a);}
	if (!(*size)){return NULL;}
	void* ret=malloc(*size);
	assert(ret&&"malloc failed");
	for(int i=0;i<BALL_COUNT;++i){

		vec3 y={ball_buff[i]->pos[0],ball_buff[i]->pos[1],0.0};
		
		struct a x;
		x.color[0]=ball_buff[i]->color[0];
		x.color[1]=ball_buff[i]->color[1];
		x.color[2]=ball_buff[i]->color[2];
		x.color[3]=(ckflg(ball_buff[i]->flag,NO_DISPLAY))?0.0:ball_buff[i]->color[3];
		x.radius=ball_buff[i]->rad;
		glm_translate_make(x.trans,y);
		((struct a *)ret)[i]=x;
	}//for i
	return ret;
#endif
}//fn

#ifdef mthreads
void*
_prep_buff_mt_routine(void* ptr){
	struct a{
		vec4 color;
		mat4 trans;
		float radius;
	};

	struct arg{
		struct a* ret;
		int work_idx[2];
	};
	struct arg ag=*(struct arg*)ptr;
	int start=ag.work_idx[0],stop=ag.work_idx[1];
	for (int i=start;i<stop;++i){

		vec3 y={ball_buff[i]->pos[0],ball_buff[i]->pos[1],0.0};
		
		struct a x;
		x.color[0]=ball_buff[i]->color[0];
		x.color[1]=ball_buff[i]->color[1];
		x.color[2]=ball_buff[i]->color[2];
		if(ckflg(ball_buff[i]->flag,NO_DISPLAY))
			x.color[3]=0.0;
		else
			x.color[3]=ball_buff[i]->color[3];
		x.radius=ball_buff[i]->rad;
		glm_translate_make(x.trans,y);
		((struct a *)ag.ret)[i]=x;	
	}//for i
}//fn

void*
prep_buff_mt(int* size,int* stride){
	struct a{
		vec4 color;
		mat4 trans;
		float radius;
	};

	*size=sizeof(struct a)*BALL_COUNT;
	if (stride){*stride=sizeof(struct a);}
	if (!(*size)){return NULL;}
	void* ret=malloc(*size);
	assert(ret&&"malloc failed");

	int work_idx[mthreads+1][2];
	int num_work=BALL_COUNT/(mthreads+1);

	struct arg{
		struct a* ret;
		int work_idx[2];
	}args[mthreads+1];
	for (int i=0;i<mthreads;++i){
		args[i].ret=ret;
		args[i].work_idx[0]=i*num_work;
		args[i].work_idx[1]=(i+1)*num_work;
		pthread_create(&(tid[i]),NULL,&_prep_buff_mt_routine,(void*)(args+i));
	}
	args[mthreads].ret=ret;
	args[mthreads].work_idx[0]=mthreads*num_work;
	args[mthreads].work_idx[1]=BALL_COUNT;
	_prep_buff_mt_routine(args+mthreads);
	for (int i=0;i<mthreads;++i)
		pthread_join(tid[i],NULL);
	return ret;
}//fn
#endif
