#version 120
#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

uniform float size;
uniform float imgWidth, imgHeight;

void main(void){
	/********************
	CPU側で
		updateRender.setGeometryInputType(GL_POINTS); // num inputs = 1
	なので
		gl_VerticesIn = 1
		
	meshに頂点dataが詰め込まれているので、CPUからGPUにはone commandで飛んでくるが、
	shaderは各GeometryInputについてvertex/geom/fragと流れる.
	
	以下では、1つのGeometryInputに対して、6頂点をemitしている.
	********************/
	for(int i = 0; i < gl_VerticesIn; i++){
		gl_Position = gl_ModelViewProjectionMatrix * ( gl_PositionIn[i] + vec4(-size,-size,0.0,0.0));
		gl_TexCoord[0].x = 0.0;
		gl_TexCoord[0].y = 0.0;
		EmitVertex();
		
		gl_Position = gl_ModelViewProjectionMatrix * (gl_PositionIn[i] + vec4(size,-size,0.0,0.0));
		gl_TexCoord[0].x = imgWidth;
		gl_TexCoord[0].y = 0.0;
		EmitVertex();
		
		gl_Position = gl_ModelViewProjectionMatrix * (gl_PositionIn[i] + vec4(size,size,0.0,0.0));
		gl_TexCoord[0].x = imgWidth;
		gl_TexCoord[0].y = imgHeight;
		EmitVertex();
		EndPrimitive();

		gl_Position = gl_ModelViewProjectionMatrix * (gl_PositionIn[i] + vec4(-size,-size,0.0,0.0));
		gl_TexCoord[0].x = 0.0;
		gl_TexCoord[0].y = 0.0;
		EmitVertex();
		
		gl_Position = gl_ModelViewProjectionMatrix * (gl_PositionIn[i] + vec4(-size,size,0.0,0.0));
		gl_TexCoord[0].x = 0.0;
		gl_TexCoord[0].y = imgHeight;
		EmitVertex();
		
		gl_Position = gl_ModelViewProjectionMatrix * (gl_PositionIn[i] + vec4(size,size,0.0,0.0));
		gl_TexCoord[0].x = imgWidth;
		gl_TexCoord[0].y = imgHeight;
		EmitVertex();
		
		
		EndPrimitive();
	}
}