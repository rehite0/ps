#include "par.h"
#include "pse.h"
#include "pse_const.h"
#include "motion.h"

void par_setup(void){
	(void)0;
}
void par_update(void){
	++rtick;
	apply_motion(0, ball_buff.len,vtick_time);
	apply_constrains(0, ball_buff.len,vtick_time);
}
void par_del(void){
	(void)0;
}
