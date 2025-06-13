#include <math.h>
#include "motion.h"
#include "pse_const.h"

static inline void get_aceration(BALL b,float acc[2]){
	if ((ball_buff.flag[b]&NO_FORCE)==NO_FORCE) return;
	const float gav=0.1f;
	#if 0
	//gravity
	acc[1]-=gav;
	#else 
	//center gravity
	float v[2]={
		0.f-ball_buff.posx[b],
		0.f-ball_buff.posy[b]};
	float norm_factor=1.0f/sqrtf(v[0]*v[0]+v[1]*v[1]);
	acc[0]+=gav*v[0]*norm_factor;
	acc[1]+=gav*v[1]*norm_factor;
	#endif
	return;
}
void apply_motion(BALL start,BALL stop,const float dt){
	for (BALL i=start;i<stop;++i){
		if ((ball_buff.flag[i]&NO_MOVE)==NO_MOVE) continue;
		float acc[2]={0.0f,0.0f};
		get_aceration(i, acc);
		ball_buff.pposx[i]=2.0f*ball_buff.posx[i]-ball_buff.pposx[i]+acc[0]*dt*dt;
		ball_buff.pposy[i]=2.0f*ball_buff.posy[i]-ball_buff.pposy[i]+acc[1]*dt*dt;
		float* tmp=0;
		tmp=ball_buff.posx;
		ball_buff.posx=ball_buff.pposx;
		ball_buff.pposx=tmp;
		tmp=ball_buff.posy;
		ball_buff.posy=ball_buff.pposy;
		ball_buff.pposy=tmp;
	}
}
void apply_constrains(BALL start,BALL stop,const float dt){
	(void)dt;
	for (BALL i=start;i<stop;++i){
		if ((ball_buff.flag[i]&NO_CONSTRAIN)==NO_CONSTRAIN) continue;
		float wall=1.0f-ball_buff.rad[i];
		const float coll_coff=0.8f;
		if (ball_buff.posx[i]>=wall){
			ball_buff.posx[i]=2.0f*wall-ball_buff.posx[i];
			ball_buff.pposx[i]=2.0f*wall-ball_buff.pposx[i];
			ball_buff.pposx[i]=ball_buff.posx[i]*(1.0f-coll_coff)+ball_buff.pposx[i]*coll_coff;
		}
		if (ball_buff.posy[i]>=wall){
			ball_buff.posy[i]=2.0f*wall-ball_buff.posy[i];
			ball_buff.pposy[i]=2.0f*wall-ball_buff.pposy[i];
			ball_buff.pposy[i]=ball_buff.posy[i]*(1.0f-coll_coff)+ball_buff.pposy[i]*coll_coff;
		}
		wall*=-1.0f;
		if (ball_buff.posx[i]<=wall){
			ball_buff.posx[i]=2.0f*wall-ball_buff.posx[i];
			ball_buff.pposx[i]=2.0f*wall-ball_buff.pposx[i];
			ball_buff.pposx[i]=ball_buff.posx[i]*(1.0f-coll_coff)+ball_buff.pposx[i]*coll_coff;
		}
		if (ball_buff.posy[i]<=wall){
			ball_buff.posy[i]=2.0f*wall-ball_buff.posy[i];
			ball_buff.pposy[i]=2.0f*wall-ball_buff.pposy[i];
			ball_buff.pposy[i]=ball_buff.posy[i]*(1.0f-coll_coff)+ball_buff.pposy[i]*coll_coff;
		}
	}
}
