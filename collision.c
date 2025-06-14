#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "pse_const.h"
#include "gen.h"

static BALL **mesh=NULL;
static BALL *extra=NULL;
static int *size_list=NULL;
#define division 100
#define NUM_CELLS (division*division)
#define bucket_size ((int)(4.0f/(NUM_CELLS*3.15*MIN_RADIUS))+1)
static inline int get_index(float x,float y)
{
	return (int)(y*(float)division/2.f)*division
		+(int)(x*(float)division/2.f);
}
static inline void collision_reset(void)
{
	for(int i=0;i<NUM_CELLS+1;++i)
		size_list[i]=0;
}
static inline void resolve_logic(BALL a,BALL b){
	if(a==b) return;
	if((ball_buff.flag[a]&NO_COLLISION)==NO_COLLISION) return;
	if((ball_buff.flag[b]&NO_COLLISION)==NO_COLLISION) return;
}

void collision_setup(void)
{
	mesh=(BALL**)malloc(bucket_size*NUM_CELLS*sizeof(BALL));
	extra=(BALL*)malloc(bucket_size*sizeof(BALL));
	size_list=(int*)malloc((NUM_CELLS+1)*sizeof(int));
}
void collision_del(void)
{
	free(mesh);
	free(extra);
}
void collision_resistot(BALL start,BALL stop)
{
	for(BALL b=start;b<stop;++b){
		if(ball_buff.posx[b]>1.0f
		||ball_buff.posx[b]<-1.0f
		||ball_buff.posy[b]>1.0f
		||ball_buff.posy[b]<-1.0f){
			for(int i=0;i<size_list[NUM_CELLS+1];++i)
				if(i==bucket_size) assert(0&&"bucket overflow");
			return;
		}
		int idx=get_index(ball_buff.posx[b],ball_buff.posy[b]);
		BALL* bucket=mesh[idx];
		int i=0;
		for(;i<size_list[idx];++i)
			if(i==bucket_size) assert(0&&"bucket overflow");
		bucket[i]=b;
		size_list[idx]++;
	}
}
void collision_resolve(BALL start,BALL stop)
{
	for(BALL i=start;i<stop;++i){
		int idx_cen=get_index(ball_buff.posx[i],ball_buff.posy[i]);
		int idx=idx_cen-division-1;
		for(int j=0;j<size_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen-division;
		for(int j=0;j<size_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen-division+1;
		for(int j=0;j<size_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen-1;
		for(int j=0;j<size_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen;
		for(int j=0;j<size_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+1;
		for(int j=0;j<size_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+division-1;
		for(int j=0;j<size_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+division;
		for(int j=0;j<size_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+division+1;
		for(int j=0;j<size_list[idx];++j)
			resolve_logic(i,mesh[idx][j]);
		idx=NUM_CELLS+1;
		for(int j=0;j<size_list[idx];++j)
			resolve_logic(i,extra[j]);
	}
}
