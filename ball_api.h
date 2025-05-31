#pragma once

extern struct ball_bufft ball_buff;
extern void ball_buff_create(void);

extern void ball_buff_del(void);

extern int ball_append(
    float posx,
    float posy,
    float pposx,
    float pposy,
    float rad,
    unsigned int flag,
    float colorr,
    float colorg,
    float colorb,
    float colora
);
/*********************************************************\
ball_append(
    0.0,0.0,		//pos-x-y
    0.0,0.0,		//ppos-x-y
    rad,		//radius
    DEFAULT,		//flags
    1.0,0.5,0.0,1.0	//rgba
    )
\*********************************************************/
