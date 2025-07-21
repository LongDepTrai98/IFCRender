#version 330 core 
uniform sampler2D depthTex;
uniform vec2 resolution;
uniform float threshold = 0.1; 

in vec2 vUv;
out vec4 FragColor;

void main() {
    vec2 texelSize = 1.0 / resolution;
    
    // Sobel kernels
    float tl = texture(depthTex, vUv + vec2(-texelSize.x, -texelSize.y)).r; // top-left
    float tm = texture(depthTex, vUv + vec2(0.0, -texelSize.y)).r;          // top-middle  
    float tr = texture(depthTex, vUv + vec2(texelSize.x, -texelSize.y)).r;  // top-right
    float ml = texture(depthTex, vUv + vec2(-texelSize.x, 0.0)).r;          // middle-left
    float mm = texture(depthTex, vUv).r;                                    // center
    float mr = texture(depthTex, vUv + vec2(texelSize.x, 0.0)).r;           // middle-right
    float bl = texture(depthTex, vUv + vec2(-texelSize.x, texelSize.y)).r;  // bottom-left
    float bm = texture(depthTex, vUv + vec2(0.0, texelSize.y)).r;           // bottom-middle
    float br = texture(depthTex, vUv + vec2(texelSize.x, texelSize.y)).r;   // bottom-right
    
    // Sobel X kernel: [-1 0 1; -2 0 2; -1 0 1]
    float sobelX = (-1.0 * tl) + (0.0 * tm) + (1.0 * tr) +
                   (-2.0 * ml) + (0.0 * mm) + (2.0 * mr) +
                   (-1.0 * bl) + (0.0 * bm) + (1.0 * br);
    
    // Sobel Y kernel: [-1 -2 -1; 0 0 0; 1 2 1]  
    float sobelY = (-1.0 * tl) + (-2.0 * tm) + (-1.0 * tr) +
                   ( 0.0 * ml) + ( 0.0 * mm) + ( 0.0 * mr) +
                   ( 1.0 * bl) + ( 2.0 * bm) + ( 1.0 * br);
    
    // Calculate edge magnitude
    float edgeMagnitude = sqrt(sobelX * sobelX + sobelY * sobelY);
    
    if (edgeMagnitude > threshold) {
        // have edge
        FragColor = vec4(1.0, 1.0, 0.0, 1.0); 
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0); // Transparent
    }
}