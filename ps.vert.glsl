#version 450
layout(location=0) in vec4 c_rgb;
layout(location=1) in float rad;
layout(location=2) in mat4 pos;

out vec4 oc_rgb;
out float orad;
out vec2 ocen;

const vec3 vert[4]=vec3[4](
					vec3(+1.1,+1.1,+0.0),
					vec3(+1.1,-1.1,+0.0),
					vec3(-1.1,-1.1,+0.0),
					vec3(-1.1,+1.1,+0.0)
					);
const int idx[6]=int[6](0,1,2, 3,0,2);

void main(){
	gl_Position=pos*vec4(rad*vert[idx[gl_VertexID]],1.0);
	oc_rgb=c_rgb;
	ocen=vec2(pos[3].xy);
	orad=rad;
}

