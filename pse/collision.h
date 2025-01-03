#pragma once

void
coll_dect(){
	int i,j;
	for (i=0;i<BALL_COUNT;i++){
		for (j=0;j<BALL_COUNT;j++){
			if (   ckflg(ball_buff[i]->flag,NO_COLLISION)
				|| ckflg(ball_buff[j]->flag,NO_COLLISION)
				|| i==j	) continue;

			BALL* a=ball_buff[i];
			BALL* b=ball_buff[j];

			coll_resolver(a,b);
		}//for j
	}//for i
}//fn
#ifdef use_qt
void
coll_dect_qt(){
	int i,j;
	for (i=0;i<BALL_COUNT;++i){
		int s=0;
		BALL** buff=qt_query_range_sq(qt,(bod){
				ball_buff[i]->pos[0]+2*ball_buff[i]->rad,
				ball_buff[i]->pos[0]-2*ball_buff[i]->rad,
				ball_buff[i]->pos[1]+2*ball_buff[i]->rad,
				ball_buff[i]->pos[1]-2*ball_buff[i]->rad
				},&s);
		for (j=0;j<s;++j){
			if (   ckflg(ball_buff[i]->flag,NO_COLLISION)
				|| ckflg(buff[j]->flag,NO_COLLISION)
				|| ball_buff[i]==buff[j]	) continue;

			BALL* a=ball_buff[i];
			BALL* b=buff[j];
			coll_resolver(a,b);
		}//for j
		free(buff);
	}//for i
	qt_free(qt);
	qt=qt_create((bod){1.0,-1.0,1.0,-1.0});
}//fn
#endif

#ifdef mthreads
void*
_coll_dect_mt_routine(void* ptr){
	int start=((int*)ptr)[0],stop=((int*)ptr)[1];
	int i,j;
	for (i=start;i<stop;++i){
		int s=0;
		BALL** buff=qt_query_range_sq(qt,(bod){
				ball_buff[i]->pos[0]+2*ball_buff[i]->rad,
				ball_buff[i]->pos[0]-2*ball_buff[i]->rad,
				ball_buff[i]->pos[1]+2*ball_buff[i]->rad,
				ball_buff[i]->pos[1]-2*ball_buff[i]->rad
				},&s);
		for (j=0;j<s;++j){
			if (   ckflg(ball_buff[i]->flag,NO_COLLISION)
				|| ckflg(buff[j]->flag,NO_COLLISION)
				|| ball_buff[i]==buff[j]	) continue;

			BALL* a=ball_buff[i];
			BALL* b=buff[j];
			coll_resolver(a,b);
		}
		free(buff);
	}
}
void
coll_dect_mt(){
	int work_idx[mthreads+1][2];
	int num_work=BALL_COUNT/(mthreads+1);
	for (int i=0;i<mthreads;++i){
		work_idx[i][0]=i*num_work;
		work_idx[i][1]=(i+1)*num_work;
		pthread_create(&(tid[i]),NULL,&_coll_dect_mt_routine,(void*)(work_idx[i]));
	}
	work_idx[mthreads][0]=mthreads*num_work;
	work_idx[mthreads][1]=BALL_COUNT;
	_coll_dect_mt_routine(work_idx+mthreads);
	for (int i=0;i<mthreads;++i)
		pthread_join(tid[i],NULL);
}
#endif

void coll_resolver(BALL* a,BALL* b){
	double axis[2]={
					a->pos[0]-b->pos[0],
					a->pos[1]-b->pos[1]};
	double dist,delta;
	dist=sqrt(abs2(axis));
	delta=(a->rad+b->rad)-dist;
	if (delta>0.0){
		if (!ckflg(a->flag,NO_MOVE)){
			a->pos[0]+=0.5f*delta*axis[0]/dist;
			a->pos[1]+=0.5f*delta*axis[1]/dist;
		}
		if (!ckflg(b->flag,NO_MOVE)){
			b->pos[0]-=0.5f*delta*axis[0]/dist;
			b->pos[1]-=0.5f*delta*axis[1]/dist;
		}
	#ifdef max_coll_rcv_v
		float avx=ptov(a->ppos[0],a->pos[0]),
			avy=ptov(a->ppos[1],a->pos[1]),
			bvx=ptov(b->ppos[0],b->pos[0]),
			bvy=ptov(b->ppos[1],b->pos[1]);
	if (mod(avx)>max_coll_rcv_v)
		a->ppos[0]=vtop(sig(avx)*max_coll_rcv_v,a->pos[0]);
	if (mod(avy)>max_coll_rcv_v)
		a->ppos[1]=vtop(sig(avy)*max_coll_rcv_v,a->pos[1]);
	if (mod(bvx)>max_coll_rcv_v)
		b->ppos[0]=vtop(sig(bvx)*max_coll_rcv_v,b->pos[0]);
	if (mod(bvy)>max_coll_rcv_v)
		b->ppos[1]=vtop(sig(bvy)*max_coll_rcv_v,b->pos[1]);

	#endif
	}
}
