#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect sparkTex;

void main() {
/*
	vec2 st = gl_TexCoord[0].st;
	gl_FragColor = texture2DRect(sparkTex, st);
*/

	vec2 st = gl_TexCoord[0].st;
	vec4 color = texture2DRect(sparkTex, st);
	
	vec4 BaseColor = vec4(0.5, 1.0, 1.0, 1.0); 
	color = color * BaseColor;
	
	gl_FragColor = color;
}