

#version 330

out vec4 final_color;

uniform vec2 resolution;

void main() {    
    
    float r = gl_FragCoord.x / resolution.x ;
    float g = gl_FragCoord.y  / resolution.y;
    float b = gl_FragCoord.y / resolution.y;

    final_color = vec4(r, g, b, 1.0);
    
}
