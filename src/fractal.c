#include "fractal.h"
#include <stddef.h>

static FractalShader load_fractal_shader(const char *path) {
    FractalShader f;
    f.shader       = LoadShader(NULL, path);
    f.max_iter_loc = GetShaderLocation(f.shader, "max_iter");
    f.dims_loc     = GetShaderLocation(f.shader, "screen_dims");
    f.bounds_loc   = GetShaderLocation(f.shader, "actual_plane_bounds");
    return f;
}

void update_fractal_shader(FractalShader *s, const BaseFractalParam *p) {
    float dims[2]   = { (float)p->width, (float)p->height };
    float bounds[4] = { (float)p->re_min, (float)p->re_max,
                        (float)p->im_min, (float)p->im_max };
    int max_iter = p->max_iter;

    SetShaderValue(s->shader, s->max_iter_loc, &max_iter, SHADER_UNIFORM_INT);
    SetShaderValue(s->shader, s->dims_loc,     dims,      SHADER_UNIFORM_VEC2);
    SetShaderValue(s->shader, s->bounds_loc,   bounds,    SHADER_UNIFORM_VEC4);
}

FractalShader load_mandelbrot_shader(const char *path,
                                     const MandelbrotFractalParams *p) {
    FractalShader f = load_fractal_shader(path);
    update_fractal_shader(&f, &p->base);
    return f;
}

FractalShader load_julia_shader(const char *path, const JuliaFractalParams *p) {
    FractalShader f = load_fractal_shader(path);

    // julia_c is Julia-only and does not change per frame, so it is set once here.
    int c_loc = GetShaderLocation(f.shader, "julia_c");
    float c[2] = { (float)creal(p->c), (float)cimag(p->c) };
    SetShaderValue(f.shader, c_loc, c, SHADER_UNIFORM_VEC2);

    update_fractal_shader(&f, &p->base);
    return f;
}
