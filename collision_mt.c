
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <threads.h>
#include <unistd.h>
#include <pthread.h>
#include "collision.h"
#include "pse_const.h"

#define NUM_THREADS 16

static BALL **mesh=NULL;
static pthread_spinlock_t *mesh_lock=NULL;
static int *len_list=NULL;
static int *size_list=NULL;

static pthread_t thr[NUM_THREADS];
static pthread_barrier_t START_COLL_REGISTER,START_COLL_DETECT,STOP_COLL_DETECT;

static inline int resolve_logic(BALL a,BALL b);

static void * rut(void* ign);
static void rut_cleanup(void* a);

static void * rut(void* ign)
{
	(void)ign;
	pthread_cleanup_push(rut_cleanup,NULL);
	thread_local static int a;
	while(1){
		pthread_barrier_wait(&START_COLL_REGISTER);
		pthread_barrier_wait(&START_COLL_DETECT);
		pthread_barrier_wait(&STOP_COLL_DETECT);
	}
	pthread_cleanup_pop(1);
	return NULL;
}

static void rut_cleanup(void* a)
{
	(void)a;
}

void collision_setup(void)
{
	mesh=(BALL**)malloc((NUM_CELLS+1)*sizeof(BALL*));
	mesh_lock=(pthread_spinlock_t*)malloc((NUM_CELLS+1)*sizeof(pthread_spinlock_t));
	len_list=(int*)malloc((NUM_CELLS+1)*sizeof(int));
	size_list=(int*)malloc((NUM_CELLS+1)*sizeof(int));
	for(int i=0;i<NUM_CELLS+1;++i){
		len_list[i]=0;
		size_list[i]=8;
		pthread_spin_init(&mesh_lock[i],PTHREAD_PROCESS_PRIVATE);
		mesh[i]=(BALL*)malloc((size_t)size_list[i]*sizeof(BALL));
	}
	for(int i=0;i<NUM_THREADS;++i){
		pthread_create(&thr[i],0,rut,0);
	}
	pthread_barrier_init(&START_COLL_REGISTER,NULL,NUM_THREADS+1);
	pthread_barrier_init(&START_COLL_DETECT,NULL,NUM_THREADS+1);
	pthread_barrier_init(&STOP_COLL_DETECT,NULL,NUM_THREADS+1);
}
void collision_del(void)
{
	for(int i=0;i<NUM_THREADS;++i){
		pthread_cancel(thr[i]);
		thr[i]=0;
	}
	for(int i=0;i<NUM_CELLS+1;++i){
		free(mesh[i]);
		pthread_spin_destroy(&mesh_lock[i]);
	}
	free(mesh);
	free((void*)mesh_lock);
	free(len_list);
	free(size_list);
	pthread_barrier_destroy(&START_COLL_REGISTER);
	pthread_barrier_destroy(&START_COLL_DETECT);
	pthread_barrier_destroy(&STOP_COLL_DETECT);
}
void collision_reset(void)
{
	for(int i=0;i<NUM_CELLS+1;++i)
		len_list[i]=0;
}
void collision_register(BALL start,BALL stop);
int collision_detect(BALL start,BALL stop);

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

