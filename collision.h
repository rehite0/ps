#pragma once
#include "pse_const.h"

#define DIVISIONS 128
#define NUM_CELLS (DIVISIONS*DIVISIONS)
#define COLL_COFFICIENT 0.999f

void collision_setup(void);
void collision_del(void);
void collision_reset(void);
void collision_register(BALL start,BALL stop);
int collision_detect(BALL start,BALL stop);
