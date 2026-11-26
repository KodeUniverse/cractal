#version 330

out vec4 final_color;

uniform int max_iter;
uniform vec2 plane_dims;


void main() {    
    
    float r = gl_FragCoord.x / resolution.x ;
    float g = gl_FragCoord.y  / resolution.y;
    float b = gl_FragCoord.y / resolution.y;

    final_color = vec4(r, g, b, 1.0);
    
}

