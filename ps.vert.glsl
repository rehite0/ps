#version 450
layout(location=0) in float vposx;
layout(location=1) in float vposy;
layout(location=2) in float vpposx;
layout(location=3) in float vpposy;
layout(location=4) in float vrad;
layout(location=5) in uint vflag;
layout(location=6) in vec4 vcolor;

out flat float posx;
out flat float posy;
out flat float rad;
out flat uint flag;
out flat vec4 color;
out vec2 uv;

const vec3 vert[4]=vec3[4](
			vec3(+1.0,+1.0,+0.0),
			vec3(+1.0,-1.0,+0.0),
 			vec3(-1.0,-1.0,+0.0),
 			vec3(-1.0,+1.0,+0.0)
 			);
const int idx[6]=int[6](0,1,2, 3,0,2);

void main(){
	posx=vposx;
	posy=vposy;
	rad=vrad;
	flag=vflag;
	color=vcolor;
	mat4 pos=mat4(
		 vec4(vrad,0.,0.,0.)
		,vec4(0.,vrad,0.,0.)
		,vec4(0.,0.,vrad,0.)
		,vec4(vposx,vposy,0.,1.)
	);
	int i=idx[gl_VertexID];
	uv=vert[i].xy;
	gl_Position=pos*vec4(vert[i],1.0);
	color*=vec4(((vflag&1<<4)==1<<4)?0.0:1.);	//NO_DISPLAY
}

