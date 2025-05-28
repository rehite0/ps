#pragma once

#include "globals.h"
#include "gtypes.h"
#include "mfun.h"
#include "forces_and_constrains.h"

void
fgravity(BALL* a){
	a->acc[1]-=3.5;
}//fn

void
fcentergrav( BALL* a){
	float pv=sqrt(abs2(a->pos));
	a->acc[0]-=a->pos[0]*(1.5/substeps)/pv;
	a->acc[1]-=a->pos[1]*(1.5/substeps)/pv;
}//fn
void
celastic_wall(BALL* a){
	if (ckflg(a->flag,NO_CONSTRAIN)) return;

	float coll_coff=0.8,v=0.0;

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

void ckeinetic_stablity(BALL* a){
	float vx=ptov(a->ppos[0],a->pos[0])
		,vy=ptov(a->ppos[1],a->pos[1]);
	if (mod(vx)>max_v)
		a->ppos[0]=vtop(vx-0.15*vx/max_v,a->pos[0]);
	if (mod(vy)>max_v)
		a->ppos[1]=vtop(vy-0.15*vy/max_v,a->pos[1]);

}//fn

void cair_resis(BALL* a){
	return;///////////////////////////////////////////////////////////////////////////////////////////
	float vx=ptov(a->ppos[0],a->pos[0])
		,vy=ptov(a->ppos[1],a->pos[1]);

	a->ppos[0]=vtop(vx,a->pos[0]);
	a->ppos[1]=vtop(vy,a->pos[1]);
	//fprintf(stdout,"%f:%f\n",a->ppos[0],a->ppos[1]);/////////////////////////////////////////////

}//fn
