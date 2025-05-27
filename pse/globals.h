#pragma once

#define use_qt
#define mthreads 15
#define substeps 1
#define gen_rand 50000
//#define gen_stream 80
#define max_v 5
#define min_t (1.0/60)/16
#define C_RAD 0.003
#define max_coll_rcv_v 2.0
enum bflags{
	DEFAULT		=0b00000000 ,
	NO_COLLISION	=0b00000001 ,
	NO_FORCE	=0b00000010 ,
	NO_CONSTRAIN	=0b00000100 ,
	NO_MOVE		=0b00001000 ,
	NO_DISPLAY	=0b00010000 
};
