#version 330 core
uniform sampler2D depthTex;
uniform vec2 textureSize;
uniform float threshold;
uniform float edgeSoftness; 
uniform vec4 colorOutline;

in vec2 vUv;
out vec4 FragColor;

void main() {
    vec2 texelSize = 1.0 / textureSize;
    vec2 uv = clamp(vUv, texelSize, 1.0 - texelSize);
   
    float tl = texture(depthTex, uv + vec2(-texelSize.x, -texelSize.y)).r; // top-left
    float tm = texture(depthTex, uv + vec2(0.0, -texelSize.y)).r;          // top-middle
    float tr = texture(depthTex, uv + vec2(texelSize.x, -texelSize.y)).r;  // top-right
    float ml = texture(depthTex, uv + vec2(-texelSize.x, 0.0)).r;          // middle-left
    float mm = texture(depthTex, uv).r;                                    // center
    float mr = texture(depthTex, uv + vec2(texelSize.x, 0.0)).r;           // middle-right
    float bl = texture(depthTex, uv + vec2(-texelSize.x, texelSize.y)).r;  // bottom-left
    float bm = texture(depthTex, uv + vec2(0.0, texelSize.y)).r;           // bottom-middle
    float br = texture(depthTex, uv + vec2(texelSize.x, texelSize.y)).r;   // bottom-right
    
    // Sobel X kernel: [-1 0 1; -2 0 2; -1 0 1]
    float sobelX = (-1.0 * tl) + (1.0 * tr) +
                   (-2.0 * ml) + (2.0 * mr) +
                   (-1.0 * bl) + (1.0 * br);
    
    // Sobel Y kernel: [-1 -2 -1; 0 0 0; 1 2 1]
    float sobelY = (-1.0 * tl) + (-2.0 * tm) + (-1.0 * tr) +
                   ( 1.0 * bl) + ( 2.0 * bm) + ( 1.0 * br);
   

    float edgeMagnitude = sqrt(sobelX * sobelX + sobelY * sobelY);
    
    float edgeMin = threshold * (1.0 - edgeSoftness);
    float edgeMax = threshold * (1.0 + edgeSoftness);
    float edgeStrength = smoothstep(edgeMin, edgeMax, edgeMagnitude);
    
    // Method 2: Multi-level smoothstep 
    //float innerEdge = threshold * 0.5;
    //float outerEdge = threshold * 1.5;
    //float edgeStrength = smoothstep(innerEdge, threshold, edgeMagnitude) * smoothstep(outerEdge, threshold, edgeMagnitude);
    
    // float edgeStrength = 1.0 - exp(-edgeMagnitude / threshold);
    // edgeStrength = smoothstep(0.0, 1.0, edgeStrength);
    
    // Method 4: Distance-based smoothstep
    // float distance = abs(edgeMagnitude - threshold);
    // float falloff = threshold * edgeSoftness;
    // float edgeStrength = 1.0 - smoothstep(0.0, falloff, distance);
    
    if (edgeStrength > 0.0) {
        FragColor = vec4(colorOutline.rgb, colorOutline.a);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0); // Transparent
    }
}