#pragma once

#include "globals.h"
#include "gtypes.h"

void iter_phy();
void phy_logic(int i);
#ifdef mthreads
void iter_phy_mt();
#endif
void update_model();


void genclick(float x, float y, float vx, float vy, BALL* b);
//void genblast(int x, int y, float velx, float vely, int num);

void model_setup();
void free_model();

void* prep_buff(int* size,int* stride);
#ifdef mthreads
void* prep_buff_mt(int* size,int* stride);
#endif

