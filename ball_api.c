#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "pse_const.h"
#include "ball_api.h"

void ball_buff_create(void){
	struct ball_bufft* bb=&ball_buff;
	assert(bb->posx==NULL);
	assert(bb->posy==NULL);
	assert(bb->pposx==NULL);
	assert(bb->pposy==NULL);
	assert(bb->rad==NULL);
	assert(bb->flag==NULL);
	assert(bb->color==NULL);
	assert(bb->len==0);
	assert(bb->size==0);
	assert(src_buff.coll_no==NULL);

	bb->size	=8;
	bb->posx	=(float*) malloc(bb->size*sizeof(float));
	bb->posy	=(float*) malloc(bb->size*sizeof(float));
	bb->pposx	=(float*) malloc(bb->size*sizeof(float));
	bb->pposy	=(float*) malloc(bb->size*sizeof(float));
	bb->rad		=(float*) malloc(bb->size*sizeof(float));
	bb->flag	=(unsigned int*) malloc(bb->size*sizeof(unsigned int));
	bb->color	=(float(*)[4]) malloc(bb->size*sizeof(float [4]));
	src_buff.coll_no=(unsigned int*) malloc(bb->size*sizeof(unsigned int));

	assert(bb->posx!=NULL);
	assert(bb->posy!=NULL);
	assert(bb->pposx!=NULL);
	assert(bb->pposy!=NULL);
	assert(bb->rad!=NULL);
	assert(bb->flag!=NULL);
	assert(bb->color!=NULL);
	assert(bb->size!=0);
	assert(src_buff.coll_no!=NULL);
}
void ball_buff_del(void){
	struct ball_bufft* bb=&ball_buff;
	free( bb->posx );
	free( bb->posy );
	free( bb->pposx);
	free( bb->pposy);
	free( bb->rad  );
	free( bb->flag );
	free( bb->color);
	free( src_buff.coll_no);
	bb->size	=0;
	bb->len		=0;
	bb->posx	= NULL;
	bb->posy	= NULL;
	bb->pposx	= NULL;
	bb->pposy	= NULL;
	bb->rad		= NULL;
	bb->flag	= NULL;
	bb->color	= NULL;
	src_buff.coll_no= NULL;
}
unsigned int ball_append(
	float posx,
	float posy,
	float pposx,
	float pposy,
	float rad,
	unsigned int flag,
	float colorr,
	float colorg,
	float colorb,
	float colora
){
	struct ball_bufft* bb=&ball_buff;
	if(bb->len==MAX_SIZE) assert(0&&"buffer full");
	if(bb->len==bb->size){
		bb->size*=2;
		bb->posx	=(float*) realloc(bb->posx	,bb->size*sizeof(float));
		bb->posy	=(float*) realloc(bb->posy	,bb->size*sizeof(float));
		bb->pposx	=(float*) realloc(bb->pposx	,bb->size*sizeof(float));
		bb->pposy	=(float*) realloc(bb->pposy	,bb->size*sizeof(float));
		bb->rad		=(float*) realloc(bb->rad	,bb->size*sizeof(float));
		bb->flag	=(unsigned int*) realloc(bb->flag,bb->size*sizeof(unsigned int));
		bb->color	=(float(*)[4]) realloc(bb->color,bb->size*sizeof(float [4]));

		src_buff.coll_no=(unsigned int*) realloc(src_buff.coll_no,bb->size*sizeof(unsigned int));

		assert(bb->posx!=NULL);
		assert(bb->posy!=NULL);
		assert(bb->pposx!=NULL);
		assert(bb->pposy!=NULL);
		assert(bb->rad!=NULL);
		assert(bb->flag!=NULL);
		assert(bb->color!=NULL);
		assert(bb->size!=0);
		assert(src_buff.coll_no!=NULL);
	}
	unsigned int i=bb->len++;
	bb->posx[i]	= posx;
	bb->posy[i]	= posy;
	bb->pposx[i]	= pposx;
	bb->pposy[i]	= pposy;
	bb->rad[i]	= rad;
	bb->flag[i]	= flag;
	bb->color[i][0]	= colorr;
	bb->color[i][1]	= colorg;
	bb->color[i][2]	= colorb;
	bb->color[i][3]	= colora;
	src_buff.coll_no[i]= 0;

	return bb->len-1;
}
