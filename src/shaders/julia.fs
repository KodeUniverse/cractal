#version 330
layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;

out vec4 final_color;

uniform int max_iter;
uniform vec2 screen_dims;
uniform vec4 actual_plane_bounds;
uniform vec2 julia_c;


vec2 complex_mult(vec2 a, vec2 b) {
    float real = a.x * b.x + a.y * b.y * -1;
    float imag = a.x * b.y + a.y * b.x;
    return vec2(real, imag);
}

vec2 complex_add(vec2 a, vec2 b) {
    float real = a.x + b.x;
    float imag = a.y + b.y;
    return vec2(real, imag);
}  

int julia(vec2 z, vec2 c, int max_iter) {
    
    for (int i = 0; i < max_iter; i++) {
        if (z.x * z.x + z.y * z.y > 4.0)
            return i;
        z = complex_add(complex_mult(z, z), c);
    }
    return max_iter;
}

vec4 get_color_result(int result_iter, int max_iter) {

    // within set
    if (result_iter >= max_iter)
        return vec4(0.0, 0.0, 0.0, 1.0);

    // 0...1 range
    float blue_shift = log(float(result_iter) + 1.0) / log(float(max_iter) + 1.0);

    return vec4(blue_shift, 0.0, 1 - blue_shift, 1.0);
}

void main() {    
    
    vec2 real_bounds = actual_plane_bounds.xy;
    vec2 imag_bounds = actual_plane_bounds.zw;

    float re_step = (real_bounds.y - real_bounds.x) / (screen_dims.x - 1);
    float im_step = (imag_bounds.y - imag_bounds.x) / (screen_dims.y - 1);
    
    ivec2 pixel = ivec2(gl_FragCoord.xy);

    // Julia: the pixel is the starting z, and c is the constant from the uniform
    vec2 z = vec2(real_bounds.x + pixel.x * re_step, imag_bounds.y - pixel.y * im_step);
    
    final_color = get_color_result(julia(z, julia_c, max_iter), max_iter);
}
