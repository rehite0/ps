#pragma once
#include "pse_const.h"

#define DIVISIONS 200
#define COLL_COFFICIENT (0.98f)


void collision_setup(void);
void collision_del(void);
void collision_reset(void);
void collision_register(BALL start,BALL stop);
int collision_detect();
