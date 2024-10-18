#version 450

precision highp float;

uniform vec2 u_resolv;
in vec4 oc_rgb;
in float orad;

out vec4 out_color;


void main(){
    vec2 ab=uv.xy*2.0-1.0;
    float d=1.0-length(ab);
    d=smoothstep(0.0,0.01,d);
    out_color.rgba=oc_rgb*vec4(d);

}
