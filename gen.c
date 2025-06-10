#include <stdlib.h>
#include "gen.h"
#include "pse.h"
#include "pse_const.h"
#include "ball_api.h"
#include "gen.h"
void generate_random(int num){
	for(int i=0;i<num;++i) ball_append(
		-1.f+2.0f*(float)rand()/(float)RAND_MAX
		,-1.f+2.f*(float)rand()/(float)RAND_MAX
		,0.0,0.0,
		COMMON_RADIUS,
		DEFAULT,
		1.0,0.5,0.0,1.0
		);	
}
void generate_stream(int stream_num
		,char flag
		,float posx,float posy
		,float velx,float vely
		,float r,float g,float b,float a
		)
{
	(void)flag;
	(void)velx;
	(void)vely;
	if (vtick%20==0){
		float dy=0.f,dx=0.f;
		for (int i=0;i<stream_num;++i) ball_append(
		    (float)i*dx+posx,(float)i*dy+posy,
		    0.0,0.0,
		    COMMON_RADIUS,
		    DEFAULT,
		    r,g,b,a
		    );	
	}
	
}
