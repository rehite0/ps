#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include "collision.h"
#include "pse_const.h"

#define highlight_coll
#define THREAD_NUM 5
#define asd 1

static struct{
	BALL** mesh;
	int* len_l;
	int* size_l;
	pthread_spinlock_t* lock_l;
}grid={NULL,NULL,NULL,NULL};
static struct{
	int *idx_list;
	int idxl_len;
	int idxl_size;
}ovlap[THREAD_NUM]={0};
#define DIVISIONS 70
#define NUM_CELLS (DIVISIONS*DIVISIONS)
#define COLL_COFFICIENT 0.8f

static inline int resolve_logic(BALL a,BALL b);
static inline int pos_index(float x);
static inline int idx_of(int x,int y);
static inline void get_idxs(BALL b);
static inline int cst_collision_detect(BALL start,BALL stop);
static inline void cst_collision_register(BALL start,BALL stop);

void collision_reset(void)
{
	for(int i=0;i<NUM_CELLS+1;++i)
		grid.len_l[i]=0;
}
void collision_setup(void)
{
	grid.mesh=(BALL**)malloc((NUM_CELLS+1)*sizeof(BALL*));
	grid.len_l=(int*)malloc((NUM_CELLS+1)*sizeof(int));
	grid.size_l=(int*)malloc((NUM_CELLS+1)*sizeof(int));
	grid.lock_l=(pthread_spinlock_t*)malloc((NUM_CELLS+1)*sizeof(pthread_spinlock_t));
	for(int i=0;i<THREAD_NUM;++i){
		ovlap[i].idxl_len=0;
		ovlap[i].idxl_size=8;
		ovlap[i].idx_list=(int*)malloc((size_t)ovlap[i].idxl_size*sizeof(int));
	}
	for(int i=0;i<NUM_CELLS+1;++i){
		grid.len_l[i]=0;
		grid.size_l[i]=8;
		grid.mesh[i]=(BALL*)malloc((size_t)grid.size_l[i]*sizeof(BALL));
		pthread_spin_init(&grid.lock_l[i],PTHREAD_PROCESS_PRIVATE);
	}
}
void collision_del(void)
{
	for(int i=0;i<NUM_CELLS+1;++i){
		free(grid.mesh[i]);
		pthread_spin_destroy(&grid.lock_l[i]);
	}
	for(int i=0;i<THREAD_NUM;++i){
		free(ovlap[i].idx_list);
	}
	free(grid.mesh);
	free(grid.len_l);
	free(grid.size_l);
	free((void*)grid.lock_l);

}
void collision_register(void)
{
	cst_collision_register(0,ball_buff.len);
}
int collision_detect(void)
{
	BALL start=0,stop=NUM_CELLS;
	int coll_count=0;
	coll_count+=cst_collision_detect(start,stop);
	return coll_count;
}
struct{pthread_t tid;
}thread_data[THREAD_NUM];
void* cd_run(void* arg){cst_collision_detect(0,0)}
void* cd_spawn(void* arg){
	for(int i=0;i<THREAD_NUM;++i)
		pthread_create(thread_data[i]tid,NULL
		 ,cd_run,(void*)&thread_data[i])
}
/*
int collision_detect(void)
{
	BALL start=0,stop=ball_buff.len;
	int coll_count=0;
	for(BALL b=start;b<stop;++b){
		int idx;
		get_idxs(b);
		for(int i=0;i<ball_buff.len;++i){
			idx=ovlap[asd].idx_list[i];
			assert(idx>=0&&idx<=NUM_CELLS);
			for(int j=0;j<grid.len_l[idx];++j)
				coll_count+=resolve_logic(b,grid.mesh[idx][j]);

		}
		idx=NUM_CELLS;
		for(int j=0;j<grid.len_l[idx];++j)
			coll_count+=resolve_logic(b,grid.mesh[idx][j]);
	}
	return coll_count;
}*/

static inline int cst_collision_detect(BALL start,BALL stop){
	//int start=0,stop=NUM_CELLS;
	int coll_count=0;
	for(BALL idx=start;idx<stop;++idx){
		for(int i=0;i<grid.len_l[idx];++i)
			for(int j=i;j<grid.len_l[idx];++j)
				coll_count+=resolve_logic(grid.mesh[idx][i],grid.mesh[idx][j]);

	}
	return coll_count;
}

static inline void cst_collision_register(BALL start,BALL stop){
	int idx;
	for(BALL b=start;b<stop;++b){
		#ifdef highlight_coll
			ball_buff.color[b][2]=0.0f;
		#endif
		get_idxs(b);
		for(int i=0;i<ovlap[asd].idxl_len;++i){
			idx=ovlap[asd].idx_list[i];
			pthread_spin_lock(&grid.lock_l[idx]);

			if(grid.len_l[idx]>=grid.size_l[idx]){
				grid.size_l[idx]*=2;
				grid.mesh[idx]=(BALL*)realloc(grid.mesh[idx]
				    ,(size_t)grid.size_l[idx]*sizeof(BALL));
			}
			grid.mesh[idx][grid.len_l[idx]]=b;
			grid.len_l[idx]++;

			pthread_spin_unlock(&grid.lock_l[idx]);
		}
	}
}
static inline int pos_index(float x)
{
	assert(!isnan(x));
	assert(!isinf(x));
	if(x>1.0f||x<-1.0f){
		return -1;
	}
	return (int)((x+1.0f)*(float)(DIVISIONS-1)/2.0f);
}
static inline int idx_of(int x,int y)
{
	return y*DIVISIONS+x;
}
static inline void get_idxs(BALL b)
{
	assert(!isnan(ball_buff.posx[b]));
	assert(!isnan(ball_buff.posy[b]));
	assert(!isinf(ball_buff.posx[b]));
	assert(!isinf(ball_buff.posy[b]));
	ovlap[asd].idxl_len=0;
	int y1,y2,x1,x2;
	y1=pos_index(ball_buff.posy[b]-ball_buff.rad[b]);
	y2=pos_index(ball_buff.posy[b]+ball_buff.rad[b]);
	x1=pos_index(ball_buff.posx[b]-ball_buff.rad[b]);
	x2=pos_index(ball_buff.posx[b]+ball_buff.rad[b]);
	if(y1==-1||y2==-1||x1==-1||x2==-1){
		ovlap[asd].idx_list[ovlap[asd].idxl_len++]=NUM_CELLS;
		return;
	}
	for(int y=y1;y<=y2;++y){
		for(int x=x1;x<=x2;++x){
			if(ovlap[asd].idxl_len==ovlap[asd].idxl_size)
				ovlap[asd].idx_list=realloc(ovlap[asd].idx_list
					,(size_t)(ovlap[asd].idxl_size*=2)*sizeof(int));
			ovlap[asd].idx_list[ovlap[asd].idxl_len++]=idx_of(x,y);
		}
	}
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
	if(dist>=rad_sum*rad_sum) return 0;
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
	#ifdef highlight_coll
		ball_buff.color[a][2]=0.9f;
		ball_buff.color[b][2]=0.9f;
	#endif
	(void)(va_[0]+vb_[0]);
	return 1;
}
