//0-x && 1-y

#define use_qt 1
#define abs2(o)		(o[0]*o[0] + o[1]*o[1])
#define mod(x) 		((x>0)? x:-1*x)
#define vtop(v,p)	(p-v*fdt)
#define ptov(pp,p)	((p-pp)/fdt)
#define ckflg(v,f)	(((v)&(f))!=0)

//types
enum bflags{
	DEFAULT			=0b00000000 ,
	NO_COLLISION	=0b00000001 ,
	NO_FORCE		=0b00000010 ,
	NO_CONSTRAIN	=0b00000100 ,
	NO_MOVE			=0b00001000 ,
	NO_DISPLAY		=0b00010000 
};
typedef struct _BALL{
	float pos[2];
	float ppos[2];
	float acc[2];
	float color[4];
	float rad;
	unsigned int flag;
}BALL;

#include "quadtree.h"

//fn prototype
void fgravity( BALL* a);
void fcentergrav( BALL* a);
void cinelastic_wall( BALL* a);
void celastic_wall( BALL* a);
void coll_dect();
void coll_dect_qt();
void iter_phy();
void update_model();
//void genclick(float x, float y, float vx, float vy);
void genblast(int x, int y, float velx, float vely, int num);
void free_model();

//globle var
double t,dt,fdt=1.0/60.0;
qtree* qt=NULL;
unsigned long long int frameno=0;
int BALL_COUNT=0;
BALL** ball_buff=NULL;

BALL* mouse_ball=NULL;

#define force_num 2
//void (*force_buff[force_num])(BALL*)={celastic_wall};
void (*force_buff[force_num])(BALL*)={fgravity,celastic_wall};
//void (*force_buff[force_num])(BALL*)={fcentergrav,celastic_wall};

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
			a->pos[0]+=0.5f*delta*axis[0]/dist;
			a->pos[1]+=0.5f*delta*axis[1]/dist;
			b->pos[0]-=0.5f*delta*axis[0]/dist;
			b->pos[1]-=0.5f*delta*axis[1]/dist;
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
			a->pos[0]+=0.5f*delta*axis[0]/dist;
			a->pos[1]+=0.5f*delta*axis[1]/dist;
			b->pos[0]-=0.5f*delta*axis[0]/dist;
			b->pos[1]-=0.5f*delta*axis[1]/dist;
		}
		}//for j
	}//for i
	qt_free(qt);
	qt=qt_create((bod){1.0,-1.0,1.0,-1.0});
}//fn

void
iter_phy(){
	int i;
	for (i=0;i<BALL_COUNT;++i){
		if (ckflg(ball_buff[i]->flag,NO_MOVE)) continue;
			
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

		if (ckflg(ball_buff[i]->flag,NO_FORCE)) continue;
		for (int j=0;j<force_num;j++){
			(*force_buff[j])(ball_buff[i]);
			if (use_qt)	qt_insert(ball_buff[i],qt);
		}//for j
	}//for i
}//fn

void
fgravity(BALL* a){
	a->acc[1]-=1.5;
}//fn

void
fcentergrav( BALL* a){
	float pv=sqrt(abs2(a->pos));
	a->acc[0]-=a->pos[0]*3.0/pv;
	a->acc[1]-=a->pos[1]*3.0/pv;
}//fn
void
celastic_wall(BALL* a){
	if (ckflg(a->flag,NO_CONSTRAIN)) return;

	float coll_coff=0.9,v=0.0;

	if (a->pos[0]+a->rad >1.0){
		v=ptov(a->ppos[0],a->pos[0]);
		a->pos[0]-=2.0*(a->pos[0]+a->rad-1.0);
		a->ppos[0]=vtop(-1.0*coll_coff*v,a->pos[0]);
	}
	if (a->pos[0]-a->rad <-1.0){
		v=ptov(a->ppos[0],a->pos[0]);
		a->pos[0]-=2.0*(a->pos[0]-a->rad+1.0);
		a->ppos[0]=vtop(-1.0*coll_coff*v,a->pos[0]);
	}
	if (a->pos[1]+a->rad >1.0){
		v=ptov(a->ppos[1],a->pos[1]);
		a->pos[1]-=2.0*(a->pos[1]+a->rad-1.0);
		a->ppos[1]=vtop(-1.0*coll_coff*v,a->pos[1]);
	}
	if (a->pos[1]-a->rad <-1.0){
		v=ptov(a->ppos[1],a->pos[1]);
		a->pos[1]-=2.0*(a->pos[1]-a->rad+1.0);
		a->ppos[1]=vtop(-1.0*coll_coff*v,a->pos[1]);
	}
}//fn

void cinelastic_wall( BALL* a){
	if (ckflg(a->flag,NO_CONSTRAIN)) return;

	if (a->pos[0]+a->rad >1.0){
		a->pos[0]=1.0-a->rad;
	}
	if (a->pos[0]-a->rad <-1.0){
		a->pos[0]=-1.0+a->rad;
	}
	if (a->pos[1]+a->rad >1.0){
		a->pos[1]=1.0-a->rad;
	}
	if (a->pos[1]-a->rad <-1.0){
		a->pos[1]=-1.0+a->rad;
	}
}//fn
void
genclick(float x, float y, float vx, float vy){
	static BALL ball_bp={
		{0.0, 0.0},
		{0.0, 0.0},
		{0.0, 0.0},
		{1.0, 0.5, 0.0, 1.0},
		0.01,
		DEFAULT
	};
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
}
void
update_model(){
	if (frameno%1==0){
		float sx=-0.98,
			  sy=1.0-0.01,
			  dy=2*0.01,
			  vx=0.2,//0.2
			  vy=-0.00;//0.05
		//for (int i=0;i<6;++i)
			//genclick(sx	,sy-dy*i, vx	, vy*(i+1)   );
		//fprintf(stdout,"ball count:%i \nframerate:%lf\n\n",BALL_COUNT,(double)1/dt);
	}
	//char x[10];scanf("%s",x);/////////////////////////////////////////////////////////////
	//sleep(0.7);/////////////////////////////////////////////////////////////////////////////
	double rdt=fdt;
	int substep=8;
	fdt=fdt/substep;
	for (int i=0;i<substep;++i){
		iter_phy()	;
		if (use_qt)
			coll_dect_qt();
		else
			coll_dect() ;
	}
	fdt=rdt;
}//fn
