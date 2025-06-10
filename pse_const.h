#pragma once
#define vtick_time 1.0f/60.0f
#define tred 15
#define MAX_SIZE ((int)1E6)

#define	DEFAULT		(0l)
#define	NO_COLLISION	(1)
#define	NO_FORCE	(1<<1)
#define	NO_CONSTRAIN	(1<<2)
#define	NO_MOVE		(1<<3)
#define	NO_DISPLAY	(1<<4)

struct ball_bufft{
		float* posx;
		float* posy;
		float* pposx;
		float* pposy;
		float* rad;
		unsigned int* flag;
		float (*color)[4];
		unsigned int len;
		unsigned int size;
	};
typedef unsigned int BALL;
extern struct ball_bufft ball_buff;
extern BALL MOUSE_BALL;

