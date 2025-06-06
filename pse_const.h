#pragma once
#define t_tick 1.0/60.0
#define tred 15
#define MAX_SIZE 1E10
enum bflags{
	DEFAULT		=0b00000000 ,
	NO_COLLISION	=0b00000001 ,
	NO_FORCE	=0b00000010 ,
	NO_CONSTRAIN	=0b00000100 ,
	NO_MOVE		=0b00001000 ,
	NO_DISPLAY	=0b00010000 ,
};
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
