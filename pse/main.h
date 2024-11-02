#pragma once

//globel constants
	#define use_qt 1
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
	#define vtop(v,p)	(p-v*fdt)
	#define ptov(pp,p)	((p-pp)/fdt)
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
	
	void genclick(float x, float y, float vx, float vy);
	//void genblast(int x, int y, float velx, float vely, int num);
	
	void model_setup();
	void free_model();

	void* prep_buff(BALL** balls,int num,int* size,int* stride);

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
#include "model.h"
#include "forces_and_constrains.h"
#include "collision.h"
