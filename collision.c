#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include "collision.h"
#include "pse_const.h"

static BALL **mesh=NULL;
static int *len_list=NULL;
static int *size_list=NULL;
#define DIVISIONS 100
#define NUM_CELLS (DIVISIONS*DIVISIONS)
static inline int get_index(float x,float y)
{
	return (int)((y+1.0f)*(float)DIVISIONS/2.f)*DIVISIONS
		+(int)((x+1.0f)*(float)DIVISIONS/2.f);
}
static inline void resolve_logic(BALL a,BALL b)
{
	if(a==b) return;
	if((ball_buff.flag[a]&NO_COLLISION)==NO_COLLISION) return;
	if((ball_buff.flag[b]&NO_COLLISION)==NO_COLLISION) return;
	float axis[2]={ball_buff.posx[a]-ball_buff.posx[b]
		,ball_buff.posy[a]-ball_buff.posy[b]};
	float dist=sqrtf(axis[0]*axis[0]+axis[1]*axis[1]);
	float delta=(ball_buff.rad[a]+ball_buff.rad[b]-dist)*0.5f;
	if(delta<=0.f) return;
	float norm_factor=1.0f/dist;
	if((ball_buff.flag[a]&NO_CONSTRAIN)!=NO_CONSTRAIN){
		float vel_dist_vec[2]={ball_buff.posx[a]-ball_buff.posx[b]
			-ball_buff.pposx[a]+ball_buff.pposx[b]
			,ball_buff.posy[a]-ball_buff.posy[b]
			-ball_buff.pposy[a]+ball_buff.pposy[b]};
		vel_dist_vec[0]=ball_buff.posx[a]-ball_buff.pposx[a]
			-(vel_dist_vec[0]*axis[0]+vel_dist_vec[1]*axis[1])
			*axis[0]*norm_factor*norm_factor;
		vel_dist_vec[1]=ball_buff.posy[a]-ball_buff.pposy[a]
			-(vel_dist_vec[0]*axis[0]+vel_dist_vec[1]*axis[1])
			*axis[1]*norm_factor*norm_factor;
		ball_buff.posx[a]+=axis[0]*norm_factor*delta;
		ball_buff.posy[a]+=axis[1]*norm_factor*delta;
		ball_buff.pposx[a]=ball_buff.posx[a]-vel_dist_vec[0];
		ball_buff.pposy[a]=ball_buff.posy[a]-vel_dist_vec[1];
	}
	if((ball_buff.flag[b]&NO_CONSTRAIN)!=NO_CONSTRAIN){
		float vel_dist_vec[2]={ball_buff.posx[b]-ball_buff.posx[a]
			-ball_buff.pposx[b]+ball_buff.pposx[a]
			,ball_buff.posy[b]-ball_buff.posy[a]
			-ball_buff.pposy[b]+ball_buff.pposy[a]};
		vel_dist_vec[0]=ball_buff.posx[b]-ball_buff.pposx[b]
			-(vel_dist_vec[0]*axis[0]+vel_dist_vec[1]*axis[1])
			*axis[0]*norm_factor*norm_factor;
		vel_dist_vec[1]=ball_buff.posy[b]-ball_buff.pposy[b]
			-(vel_dist_vec[0]*axis[0]+vel_dist_vec[1]*axis[1])
			*axis[1]*norm_factor*norm_factor;

		ball_buff.posx[b]-=axis[0]*norm_factor*delta;
		ball_buff.posy[b]-=axis[1]*norm_factor*delta;
		ball_buff.pposx[b]=ball_buff.posx[b]-vel_dist_vec[0];
		ball_buff.pposy[b]=ball_buff.posy[b]-vel_dist_vec[1];
	}
}
void collision_reset(void)
{
	for(int i=0;i<NUM_CELLS+1;++i)
		len_list[i]=0;
}
void collision_setup(void)
{
	mesh=(BALL**)malloc((NUM_CELLS+1)*sizeof(BALL*));
	len_list=(int*)malloc((NUM_CELLS+1)*sizeof(int));
	size_list=(int*)malloc((NUM_CELLS+1)*sizeof(int));
	for(int i=0;i<NUM_CELLS+1;++i){
		len_list[i]=0;
		size_list[i]=8;
		mesh[i]=(BALL*)malloc((size_t)size_list[i]*sizeof(BALL));
	}
}
void collision_del(void)
{
	for(int i=0;i<NUM_CELLS+1;++i)
		free(mesh[i]);
	free(mesh);
	free(len_list);
	free(size_list);
}
void collision_register(BALL start,BALL stop)
{
	int idx;
	for(BALL b=start;b<stop;++b){
		if(ball_buff.posx[b]>1.0f
		||ball_buff.posx[b]<-1.0f
		||ball_buff.posy[b]>1.0f
		||ball_buff.posy[b]<-1.0f)
			idx=NUM_CELLS;
		else idx=get_index(ball_buff.posx[b],ball_buff.posy[b]);
		if(len_list[idx]>=size_list[idx]){
			size_list[idx]*=2;
			mesh[idx]=(BALL*)realloc(mesh[idx],(size_t)size_list[idx]*sizeof(BALL));
		}
		mesh[idx][len_list[idx]]=b;
		len_list[idx]++;
	}
}
void collision_detect(BALL start,BALL stop)
{
	for(BALL i=start;i<stop;++i){
		int idx_cen=get_index(ball_buff.posx[i],ball_buff.posy[i]);
		int idx=idx_cen-DIVISIONS-1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen-DIVISIONS;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen-DIVISIONS+1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen-1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+DIVISIONS-1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+DIVISIONS;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+DIVISIONS+1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=NUM_CELLS;
		for(int j=0;idx>0&&idx<=NUM_CELLS&&j<len_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
	}
}
