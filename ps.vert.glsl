#version 450
layout(location=0) in float posx;
layout(location=1) in float posy;
layout(location=2) in float pposx;
layout(location=3) in float pposy;
layout(location=4) in float rad;
layout(location=5) in uint flags;
layout(location=6) in vec4 color;

const vec3 vert[4]=vec3[4](
			vec3(+1.1,+1.1,+0.0),
			vec3(+1.1,-1.1,+0.0),
 			vec3(-1.1,-1.1,+0.0),
 			vec3(-1.1,+1.1,+0.0)
 			);
const int idx[6]=int[6](0,1,2, 3,0,2);

void main(){
	mat4 pos=mat4(
		 vec4(1.,0.,0.,posx)
		,vec4(0.,1.,0.,posy)
		,vec4(0.,0.,1.,0.)
		,vec4(0.,0.,0.,1.)
	);
	gl_Position=pos*vec4(rad*vert[idx[gl_VertexID]],1.0);
}

