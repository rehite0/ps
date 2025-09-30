#include <stdio.h>
#include <assert.h>
#include "par.h"
#include "pse.h"
#include "pse_const.h"
#include "motion.h"
#include "collision.h"

void par_setup(void){
	collision_setup();
}
void par_update(float v_ss_tick_time){
	++vtick;
	apply_motion(0, ball_buff.len,v_ss_tick_time);
	apply_constrains(0, ball_buff.len,v_ss_tick_time);
	
	collision_register(0,ball_buff.len);
	collision_detect(0,ball_buff.len);
	collision_reset();
	apply_constrains(0, ball_buff.len,v_ss_tick_time);
}
void par_del(void){
	collision_del();
}
