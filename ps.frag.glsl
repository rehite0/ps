#version 450

precision highp float;

in flat float posx;
in flat float posy;
in flat float rad;
in flat uint flag;
in flat vec4 color;
in vec2 uv;
//uniform vec2 hw;
//uniform vec2 tdt;


out vec4 out_color;


void main(){
	float d=length(uv);
	d=smoothstep(1.0,1.0-rad*0.05,d);
	out_color=color*d;
	//out_color=vec4(posx,posy,0.,1.);
}
