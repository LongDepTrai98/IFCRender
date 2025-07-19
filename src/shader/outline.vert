#version 330 core 
#define attribute in 
#define varying out 
uniform mat4 modelViewMatrix; 
uniform mat4 projectionMatrix;
uniform float outlineThickness;
attribute vec3 position;
varying float vDepth; 
void main()
{
	 gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );
	 vDepth = gl_Position.z / gl_Position.w;
}