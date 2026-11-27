#version 330
layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;

out vec4 final_color;

uniform int max_iter;
uniform vec2 screen_dims;
uniform vec4 actual_plane_bounds;
uniform vec2 julia_c;

// Escape radius 256 (radius^2 = 65536). A large radius makes the smooth
// iteration formula below accurate.
const float BAILOUT_SQ = 65536.0;


vec2 complex_mult(vec2 a, vec2 b) {
    float real = a.x * b.x + a.y * b.y * -1.0;
    float imag = a.x * b.y + a.y * b.x;
    return vec2(real, imag);
}

vec2 complex_add(vec2 a, vec2 b) {
    float real = a.x + b.x;
    float imag = a.y + b.y;
    return vec2(real, imag);
}  

// Returns a smooth escape count, or -1.0 if z never escaped.
float julia(vec2 z, vec2 c) {
    for (int i = 0; i < max_iter; i++) {
        float r2 = z.x * z.x + z.y * z.y;
        if (r2 > BAILOUT_SQ) {
            // Fractional part from how far past the radius z ended up:
            // nu = i + 1 - log2(ln|z|)
            float nu = float(i) + 1.0 - log(log(float(r2)) * 0.5) / log(2.0);
            return nu;
        }
        z = complex_add(complex_mult(z, z), c);
    }
    return -1.0;
}

// Color schemes. Each maps the smooth escape count nu to an RGB color.
// Pick one by changing COLOR_SCHEME below. Scales are fixed (not divided by
// max_iter), so the same point gets the same color in preview and full render.

// Rainbow: cosine palette (Inigo Quilez). It wraps every 20 iterations, so a
// color does not identify a single escape count.
vec3 scheme_rainbow(float nu) {
    float t = nu * 0.05;
    vec3 a = vec3(0.5);
    vec3 b = vec3(0.5);
    vec3 c = vec3(1.0);
    vec3 d = vec3(0.0, 0.33, 0.67);
    return a + b * cos(6.28318 * (c * t + d));
}

// Navy to orange: monotonic, so brighter/warmer means a larger escape count.
// t = 1 - exp(-nu/100) rises toward 1 without repeating.
vec3 scheme_navy_orange(float nu) {
    float t = 1.0 - exp(-nu * 0.01);
    vec3 start = vec3(0.0, 0.02, 0.15);  // dark navy
    vec3 end   = vec3(1.0, 0.6, 0.1);    // orange
    return mix(start, end, t);
}

#define COLOR_SCHEME scheme_rainbow

vec4 get_color_result(float nu) {

    // within set
    if (nu < 0.0)
        return vec4(0.0, 0.0, 0.0, 1.0);

    return vec4(COLOR_SCHEME(nu), 1.0);
}

void main() {    
    
    vec2 real_bounds = actual_plane_bounds.xy;
    vec2 imag_bounds = actual_plane_bounds.zw;

    float re_step = (real_bounds.y - real_bounds.x) / (screen_dims.x - 1.0);
    float im_step = (imag_bounds.y - imag_bounds.x) / (screen_dims.y - 1.0);
    
    ivec2 pixel = ivec2(gl_FragCoord.xy);

    // Julia: the pixel is the starting z, and c is the constant from the uniform
    vec2 z = vec2(real_bounds.x + pixel.x * re_step, imag_bounds.y - pixel.y * im_step);
    
    final_color = get_color_result(julia(z, julia_c));
}
