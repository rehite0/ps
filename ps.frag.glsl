#version 450

precision highp float;

layout(location=0) in flat float posx;
layout(location=1) in flat float posy;
layout(location=4) in flat float rad;
layout(location=5) in flat uint flags;
layout(location=6) in flat vec4 color;
//uniform vec2 hw;
//uniform vec2 tdt;

// in vec2 ocen;
// in vec4 oc_rgb;
// in float orad;

out vec4 out_color;


void main(){
    // vec2 uv=((gl_FragCoord.xy/hw.xy)*2.0-1.0)-ocen;
    // float d=length(uv);
    // d=1.0-smoothstep(orad,orad+0.0001,d);
    // out_color=oc_rgb*vec4(d);
    out_color=color;
}
