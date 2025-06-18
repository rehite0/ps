#pragma once
#include "pse_const.h"
void collision_setup(void);
void collision_del(void);
void collision_reset(void);
void collision_register(BALL start,BALL stop);
int collision_detect(BALL start,BALL stop);
