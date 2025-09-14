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
	++rtick;
	apply_motion(0, ball_buff.len,v_ss_tick_time);
	apply_constrains(0, ball_buff.len,v_ss_tick_time);
	
	collision_register(0,ball_buff.len);
	collision_detect(0,ball_buff.len);
	collision_reset();
	apply_constrains(0, ball_buff.len,v_ss_tick_time);
/* 	improved collision instabality
	int j=0,i=0;
	do{
		++i;
		collision_reset();
		collision_register(0,ball_buff.len);
		j=collision_detect(0,ball_buff.len);
		apply_constrains(0, ball_buff.len,vtick_time);
		//dprintf(2,"%d.......",j);
	}while(j>10&&i<=50);
*/

}
void par_del(void){
	collision_del();
}
