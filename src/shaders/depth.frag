#version 330 core
#define attribute in 
#define varying out 
uniform float near;
uniform float far;
attribute float vDepth;
varying vec4 FragColor;
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    float depth = pow(LinearizeDepth(gl_FragCoord.z) / far, 0.3); 
    FragColor = vec4(vec3(depth), 1.0);
}