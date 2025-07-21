#version 330 core
#define attribute in 
#define varying out 
attribute vec2 vUv;
out vec4 FragColor;
uniform sampler2D depthTex;
void main()
{
  //float d = texture(depthTex, vUv).r;
  FragColor = texture(colorTex, vUv);
}