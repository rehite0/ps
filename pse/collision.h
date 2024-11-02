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
			}
		}//for j
	}//for i
}//fn

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
			}
		}//for j
	}//for i
	qt_free(qt);
	qt=qt_create((bod){1.0,-1.0,1.0,-1.0});
}//fn
