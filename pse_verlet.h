//0-x && 1-y
#define abs2(o) (o[0]*o[0] + o[1]*o[1])
#define mod(x)  ((x>0)? x:-1*x)

//types
typedef struct _BALL{
	float pos[2];
	float ppos[2];
	float acc[2];
	float color[4];
	float rad;

}BALL;

//fn prototype
void fgravity( BALL* a);
void fconstrain( BALL* a);
void coll_dect();
void iter_phy();
void update_model();
void genclick(float x, float y, float px, float py);
//void genblast(int x, int y, float velx, float vely, int num);
void free_model();

//fix shader uv
//wall constrain does not consider for rad
//pixel to ndc macro

//globle var
double t,dt;
unsigned long long int frameno=0;
int BALL_COUNT=0;
BALL** ball_buff=NULL;

#define force_num 2
void (*force_buff[force_num])(BALL*)={fgravity,fconstrain};

void
coll_dect(){
	int i,j;
	for (i=0;i<BALL_COUNT;i++){
		for (j=0;j<BALL_COUNT;j++){
			if (i==j) continue;
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
iter_phy(){
	int i;
	for (i=0;i<BALL_COUNT;++i){
		ball_buff[i]->acc[0]=0.0;
		ball_buff[i]->acc[1]=0.0;
		for (int j=0;j<force_num;j++)
			(*force_buff[j])(ball_buff[i]);

		float px=ball_buff[i]->ppos[0],py=ball_buff[i]->ppos[1];
		ball_buff[i]->ppos[0]=ball_buff[i]->pos[0];
		ball_buff[i]->ppos[1]=ball_buff[i]->pos[1];

		ball_buff[i]->pos[0]
			=ball_buff[i]->pos[0]*2.0
			-px
			+ball_buff[i]->acc[0]*dt*dt;
		ball_buff[i]->pos[1]
			=ball_buff[i]->pos[1]*2.0
			-py
			+ball_buff[i]->acc[1]*dt*dt;
	}//for i
}//fn

void
fgravity(BALL* a){
	a->acc[1]-=3.8;
}//fn

void
fconstrain(BALL* a){
	if (a->pos[0]+a->rad >1){
		a->pos[0]=0.99-a->rad;
	}
	if (a->pos[0]-a->rad <-1){
		a->pos[0]=-0.99+a->rad;
	}
	if (a->pos[1]+a->rad >1){
		a->pos[1]=0.99-a->rad;
	}
	if (a->pos[1]-a->rad <-1){
		a->pos[1]=-0.99+a->rad;
	}
}//fn
void
genclick(float x, float y, float px, float py){
	static BALL ball_bp={
		{0.0, 0.0},
		{0.0, 0.0},
		{0.0, 0.0},
		{1.0, 1.0, 0.0, 1.0},
		0.03 };
	BALL* a=malloc(sizeof(BALL));
	assert(a && "malloc failed");
	*a=ball_bp;
	ball_buff=realloc(ball_buff,sizeof(BALL*)*(++BALL_COUNT));
	assert(ball_buff &&"null return by realloc!");
	ball_buff[BALL_COUNT-1]=a;
	ball_buff[BALL_COUNT-1]->pos[0]=x;
	ball_buff[BALL_COUNT-1]->pos[1]=y;
	ball_buff[BALL_COUNT-1]->ppos[0]=px;
	ball_buff[BALL_COUNT-1]->ppos[1]=py;
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
	if (frameno%10==0) genclick(-0.95,0.95,-0.96,0.96);

	double rdt=dt;
	int substep=5;
	dt=dt/substep;
	for (int i=0;i<substep;++i){
		iter_phy()	;
		coll_dect() ;
	}
	dt=rdt;
}//fn
