#pragma once

#include "globals.h"
#include "gtypes.h"

extern void iter_phy();
extern void phy_logic(int i);
#ifdef mthreads
extern void iter_phy_mt();
#endif
extern void update_model();


extern void genclick(float x, float y, float vx, float vy, BALL* b);
//void genblast(int x, int y, float velx, float vely, int num);

extern void model_setup();
extern void free_model();

extern void* prep_buff(int* size,int* stride);
#ifdef mthreads
extern void* prep_buff_mt(int* size,int* stride);
#endif

