//0-x && 1-y
#define abs2(o) (o[0]*o[0] + o[1]*o[1])
#define mod(x)  ((x>0)? x:-1*x)

//types
typedef struct _BALL{
	float pos[2];
	float vel[2];
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
void genclick(float x, float y);
//void genstream(int x, int y, float velx, float vely, float inc_rate);
//void genblast(int x, int y, float velx, float vely, int num);
void free_model();

//fix shader uv
//colldect use a.,b.
//wall constrain does not consider for rad

//globle var
double t,dt;
int BALL_COUNT=0;
BALL** ball_buff=NULL;
#define force_num 2
void (*force_buff[force_num])(BALL*)={fgravity,fconstrain};

/*collision physics
 * <   n'+
 *  *  __+
 *   *|  +
 *    *  +
 *  m'( * *
 * ++++* a +/+
 *      * +/+ *
 *       +/+ b *++++
 *          * * )m'
 *           + *
 *           +  *
 *           +__|*
 *           + n' >
 *
 *1 aofcol(axis of collision) is vec b->a
 *  m,n is angle made by aofcol and -ve x_axis,+ve y_axis
 *2 cos m is |b/h| and cos n is |p/h|
 *  velocity after collision
 */
void
coll_dect(){
	int i,j;
	for (i=0;i<BALL_COUNT;i++){
		for (j=0;j<BALL_COUNT;j++){
			BALL* a=ball_buff[i];
			BALL* b=ball_buff[j];

			double aofcol[2]={a->pos[0]-b->pos[0], a->pos[1]-b->pos[1]};	//*1

			if ( abs2(aofcol)<(a->rad+b->rad)*(a->rad+b->rad) && i!=j ){
				double p=aofcol[0],ba=aofcol[1],h=sqrt(abs2(aofcol));
				a->vel[0]=a->vel[0]-2*(sqrt(abs2(a->vel))*mod(p/h));		//*2
				b->vel[0]=b->vel[0]-2*(sqrt(abs2(b->vel))*mod(ba/h));
				a->vel[1]=a->vel[1]-2*(sqrt(abs2(a->vel))*mod(ba/h));
				b->vel[1]=b->vel[1]-2*(sqrt(abs2(b->vel))*mod(p/h));
			}//if
		}//for j
	}//for i
}//fn

void
iter_phy(){
	int i;
	for (i=0;i<BALL_COUNT;i++){
		ball_buff[i]->acc[0]=0.0;
		ball_buff[i]->acc[1]=0.0;
		for (int j=0;j<force_num;j++)
			(*force_buff[j])(ball_buff[i]);
		ball_buff[i]->vel[0]+=ball_buff[i]->acc[0]*dt*0.5;
		ball_buff[i]->vel[1]+=ball_buff[i]->acc[1]*dt*0.5;
		ball_buff[i]->pos[0]+=ball_buff[i]->vel[0]*dt;
		ball_buff[i]->pos[1]+=ball_buff[i]->vel[1]*dt;
		
	}//for i
}//fn

void
fgravity(BALL* a){
	a->acc[1]-=9.8;
}//fn

void
fconstrain(BALL* a){
	if (a->pos[0] >1){
		a->pos[0]=1;
		a->vel[0]=0;
	}
	if (a->pos[0] <-1){
		a->pos[0]=-1;
		a->vel[0]=0;
	}
	if (a->pos[1] >1){
		a->pos[1]=1;
		a->vel[1]=0;
	}
	if (a->pos[1] <-1){
		a->pos[1]=-1;
		a->vel[1]=0;
	}
}//fn
void
genclick(float x, float y){
	static BALL ball_bp={
		{0.0, 0.0},
		{0.0, 0.0},
		{0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0},
		0.05 };
	BALL *a=malloc(sizeof(BALL));
	assert(a && "null return by malloc!");
	*a=ball_bp;
	a->pos[0]=x;
	a->pos[1]=y;
	ball_buff=realloc(ball_buff,sizeof(BALL*)*(++BALL_COUNT));
	assert(ball_buff &&"null return by realloc!");
	ball_buff[BALL_COUNT]=a;
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
	iter_phy()	;
	coll_dect() ;
}//fn
