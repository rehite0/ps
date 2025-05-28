#pragma once
#include <stdlib.h>
#include "globals.h"
#include "gtypes.h"
#include "qt/quadtree.h"
#include "forces_and_constrains.h"

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
#ifdef mthreads
	pthread_t tid[mthreads];
#endif

void (*force_buff[])(BALL*)={
//		ckeinetic_stablity
//		cair_resis
//		fgravity
		fcentergrav
		,celastic_wall
//		,cinelastic_wall
};
#define force_num sizeof(force_buff)/sizeof(*force_buff)
