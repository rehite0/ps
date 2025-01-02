#pragma once

//globel constants & flags
	#define use_qt
	#define substeps 4
//	#define gen_rand 10000
	#define gen_stream 9
	#define max_v 5
	#define min_t (1.0/60)/4
	#define C_RAD 0.008
	enum bflags{
		DEFAULT			=0b00000000 ,
		NO_COLLISION	=0b00000001 ,
		NO_FORCE		=0b00000010 ,
		NO_CONSTRAIN	=0b00000100 ,
		NO_MOVE			=0b00001000 ,
		NO_DISPLAY		=0b00010000 
	};

//macro functions
	#define abs2(o)		(o[0]*o[0] + o[1]*o[1])
	#define mod(x) 		((x>0)? x:-1*x)
	#define vtop(v,p)	((p)-(v)*fdt)
	#define ptov(pp,p)	(((p)-(pp))/fdt)
	#define ckflg(v,f)	(((v)&(f))!=0)

//types
	typedef struct _BALL{
		float pos[2];
		float ppos[2];
		float acc[2];
		float color[4];
		float rad;
		unsigned int flag;
	}BALL;

#ifdef use_qt
	#include "qt/quadtree.h"
#endif

//fn prototype
	void fgravity( BALL* a);
	void fcentergrav( BALL* a);
	
	void cinelastic_wall( BALL* a);
	void celastic_wall( BALL* a);
	void cair_resis(BALL* a);
	void ckeinetic_stablity(BALL* a);
	
	void coll_dect();
	#ifdef use_qt
		void coll_dect_qt();
	#endif

	void iter_phy();
	void update_model();
	
	void genclick(float x, float y, float vx, float vy, BALL* b);
	//void genblast(int x, int y, float velx, float vely, int num);
	
	void model_setup();
	void free_model();

	void* prep_buff(BALL** balls,int num,int* size,int* stride);

//globle var
	double t,dt,fdt=(1.0/(60.0*substeps)<min_t)?1.0/(60.0*substeps):min_t;
	int BALL_COUNT=0;
	BALL** ball_buff=NULL;
	
	BALL* mouse_ball=NULL;

	BALL ball_bp={
			{0.0, 0.0},
			{0.0, 0.0},
			{0.0, 0.0},
			{1.0, 0.5, 0.0, 1.0},
			C_RAD,
			DEFAULT
		};
	unsigned long long int frameno=0;
	#ifdef use_qt
		qtidx qt=0;
	#endif

	void (*force_buff[])(BALL*)={
			ckeinetic_stablity
			//cair_resis
			,fgravity
//			,fcentergrav
			,celastic_wall
//			,cinelastic_wall
	};
	#define force_num sizeof(force_buff)/sizeof(*force_buff)

#include "model.h"
#include "forces_and_constrains.h"
#include "collision.h"
