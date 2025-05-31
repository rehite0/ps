#include <GL/glew.h>
#include <unistd.h>
#include <assert.h>

#include "pse_const.h"
#include "pse.h"
#include "ball_api.h"
#include "render.h"

unsigned long vtick;

void pse_setup(void){
    vtick=0;
    ball_buff_create();
}

void pse_exit(void){
    vtick=0;
    ball_buff_del();
}
void pse_update(void){
    ++vtick;
}

