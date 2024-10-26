#version 450

precision highp float;

uniform vec2 hw;
uniform vec2 tdt;

//in vec4 opos;
in vec4 oc_rgb;
in float orad;

out vec4 out_color;


void main(){
	//vec2 uv=(gl_FragCoord/hw.yx)*2.0-1.0;
    //float d=1.0-length(uv);
    //d=smoothstep(0.0,0.01,d);
    out_color=oc_rgb*vec4(1.0);

}
