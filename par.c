#include "par.h"
#include "pse.h"
#include "pse_const.h"
#include "motion.h"
#include "collision.h"

void par_setup(void){
	collision_setup();
}
void par_update(void){
	++rtick;
	apply_motion(0, ball_buff.len,vtick_time);
	apply_constrains(0, ball_buff.len,vtick_time);
	collision_reset();
	collision_register(0,ball_buff.len);
	collision_detect(0,ball_buff.len);
}
void par_del(void){
	collision_del();
}
