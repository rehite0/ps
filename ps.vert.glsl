#version 450
layout(location=0) in vec2 pos;
layout(location=1) in vec4 c_rgb;
layout(location=3) in float rad;

out vec4 oc_rgb;
out float orad;

const vec3 vert[4]=vec3[4](
					vec3(+0.33,+0.33,+0.0),
					vec3(+0.33,-0.33,+0.0),
					vec3(-0.33,-0.33,+0.0),
					vec3(-0.33,+0.33,+0.0)
					);
const int idx[6]=int[6](0,1,3, 1,2,3);
const mat4 ptrans//ic

void main(){
	gl_Position=ptrans*vec4(vert[idx[gl_VertexID]],1.0);
	oc_rgb=c_rgb;
	orad=rad;
}

