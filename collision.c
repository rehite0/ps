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
#define DIVISIONS 50
#define NUM_CELLS (DIVISIONS*DIVISIONS)
#define COLL_COFFICIENT 0.8f

static inline int pos_index(float x)
{
	assert(!isnan(x));
	assert(!isinf(x));
	return (int)((x+1.0f)*(float)(DIVISIONS-1)/2.0f);
}
static inline int idx_of(int x,int y){
	return y*DIVISIONS+x;
}
static inline int get_index(BALL b)
{
	assert(!isnan(ball_buff.posx[b]));
	assert(!isnan(ball_buff.posy[b]));
	assert(!isinf(ball_buff.posx[b]));
	assert(!isinf(ball_buff.posy[b]));
	if(ball_buff.posx[b]>1.0f
	||ball_buff.posx[b]<-1.0f
	||ball_buff.posy[b]>1.0f
	||ball_buff.posy[b]<-1.0f)
		return NUM_CELLS;
	return idx_of(
		pos_index(ball_buff.posx[b])
	       ,pos_index(ball_buff.posy[b])
	);
}
static inline int resolve_logic(BALL a,BALL b)
{
	if(a==b) return 0;
	if((ball_buff.flag[a]&NO_COLLISION)==NO_COLLISION) return 0;
	if((ball_buff.flag[b]&NO_COLLISION)==NO_COLLISION) return 0;
	float axis[2]={ball_buff.posx[a]-ball_buff.posx[b]
		,ball_buff.posy[a]-ball_buff.posy[b]};
	float dist=axis[0]*axis[0]+axis[1]*axis[1];
	float rad_sum=ball_buff.rad[a]+ball_buff.rad[b];
	if(dist>rad_sum*rad_sum) return 0;
	float delta,norm_factor;
	dist=sqrtf(dist);
	delta=(rad_sum-dist);
	if((ball_buff.flag[a]&NO_CONSTRAIN)!=NO_CONSTRAIN);
	else if((ball_buff.flag[b]&NO_CONSTRAIN)!=NO_CONSTRAIN) ;
	else delta*=0.5f;
	norm_factor=1.0f/dist;
	assert(!isnan(norm_factor));
	assert(!isinf(norm_factor));
	float va_vb[2]={
		ball_buff.posx[a]-ball_buff.posx[b]
		-ball_buff.pposx[a]+ball_buff.pposx[b]
	,	ball_buff.posy[a]-ball_buff.posy[b]
		-ball_buff.pposy[a]+ball_buff.pposy[b]
	};
	float va_[2]={
		ball_buff.posx[a]-ball_buff.pposx[a]
	-(	(va_vb[0]*axis[0]+va_vb[1]*axis[1])
		*axis[0]*norm_factor*norm_factor
	)*(0.5f*(1.0f+COLL_COFFICIENT))
	,	ball_buff.posy[a]-ball_buff.pposy[a]
	-(	(va_vb[0]*axis[0]+va_vb[1]*axis[1])
		*axis[1]*norm_factor*norm_factor
	)*(0.5f*(1.0f+COLL_COFFICIENT))
	};

	if((ball_buff.flag[a]&NO_CONSTRAIN)!=NO_CONSTRAIN){
		ball_buff.posx[a]+=axis[0]*norm_factor*delta;
		ball_buff.posy[a]+=axis[1]*norm_factor*delta;

		ball_buff.pposx[a]=ball_buff.posx[a]-va_[0];
		ball_buff.pposy[a]=ball_buff.posy[a]-va_[1];
	}

	float vb_[2]={
		ball_buff.posx[b]-ball_buff.pposx[b]
	-(	(va_vb[0]*axis[0]+va_vb[1]*axis[1])
		*(-1.0f)*axis[0]*norm_factor*norm_factor
	)*(0.5f*(1.0f+COLL_COFFICIENT))
	,	ball_buff.posy[b]-ball_buff.pposy[b]
	-(	(va_vb[0]*axis[0]+va_vb[1]*axis[1])
		*(-1.0f)*axis[1]*norm_factor*norm_factor
	)*(0.5f*(1.0f+COLL_COFFICIENT))
	};
	if((ball_buff.flag[b]&NO_CONSTRAIN)!=NO_CONSTRAIN){
		ball_buff.posx[b]-=axis[0]*norm_factor*delta;
		ball_buff.posy[b]-=axis[1]*norm_factor*delta;

		ball_buff.pposx[b]=ball_buff.posx[b]-vb_[0];
		ball_buff.pposy[b]=ball_buff.posy[b]-vb_[1];
	}
	return 1;
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
		idx=get_index(b);
		if(len_list[idx]>=size_list[idx]){
			size_list[idx]*=2;
			mesh[idx]=(BALL*)realloc(mesh[idx],(size_t)size_list[idx]*sizeof(BALL));
		}
		mesh[idx][len_list[idx]]=b;
		len_list[idx]++;
	}
}
int collision_detect(BALL start,BALL stop)
{
	int coll_count=0;
	for(BALL i=start;i<stop;++i){
		int idx_cen=get_index(i);
		int idx=idx_cen-DIVISIONS-1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			coll_count+=resolve_logic(i,mesh[idx][j]);
		idx=idx_cen-DIVISIONS;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			coll_count+=resolve_logic(i,mesh[idx][j]);
		idx=idx_cen-DIVISIONS+1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			coll_count+=resolve_logic(i,mesh[idx][j]);
		idx=idx_cen-1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			coll_count+=resolve_logic(i,mesh[idx][j]);
		idx=idx_cen;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			coll_count+=resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			coll_count+=resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+DIVISIONS-1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			coll_count+=resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+DIVISIONS;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			coll_count+=resolve_logic(i,mesh[idx][j]);
		idx=idx_cen+DIVISIONS+1;
		for(int j=0;idx>=0&&idx<NUM_CELLS&&j<len_list[idx];++j)
			coll_count+=resolve_logic(i,mesh[idx][j]);
		idx=NUM_CELLS;
		for(int j=0;idx>0&&idx<=NUM_CELLS&&j<len_list[idx];++j)
			coll_count+=resolve_logic(i,mesh[idx][j]);
	}
	return coll_count;
}
