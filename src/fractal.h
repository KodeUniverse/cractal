#ifndef FRACTAL_H
#define FRACTAL_H

#include <complex.h>

#include "raylib.h"

/* Parameters shared by every fractal: image size, complex-plane bounds, and
 * iteration limit. */
typedef struct {
    int width;
    int height;
    double re_min;
    double re_max;
    double im_min;
    double im_max;
    int max_iter;
} BaseFractalParam;

typedef struct {
    BaseFractalParam base;
} MandelbrotFractalParams;

typedef struct {
    BaseFractalParam base;
    double complex c;
} JuliaFractalParams;

/* A loaded fragment shader plus the uniform locations shared by all fractals.
 * Fractal-specific uniforms (such as julia_c) are handled in their loaders. */
typedef struct {
    Shader shader;
    int max_iter_loc;
    int dims_loc;
    int bounds_loc;
} FractalShader;

/* Each loader loads the shader from `path`, sets its uniforms, and returns it.
 * Check shader.id == 0 to detect a failed load. */
FractalShader load_mandelbrot_shader(const char *path,
                                     const MandelbrotFractalParams *p);
FractalShader load_julia_shader(const char *path, const JuliaFractalParams *p);

/* Writes the shared uniforms (screen_dims, actual_plane_bounds, max_iter).
 * Call before drawing; the max_iter in `p` is used for this draw. */
void update_fractal_shader(FractalShader *s, const BaseFractalParam *p);

#endif
