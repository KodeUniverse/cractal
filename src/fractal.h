#ifndef FRACTAL_H
#define FRACTAL_H

#include <complex.h>
#include <stddef.h>

#include "plane.h"

/* Shared image resolution, complex-plane bounds, and iteration limit. */
typedef struct {
    size_t width;
    size_t height;
    double re_min;
    double re_max;
    double im_min;
    double im_max;
    int max_iter;
} FractalView;

void graph_mandelbrot(CoordinatePlane2 *plane, double re_min, double re_max,
                      double im_min, double im_max, int fractal_max_iter);

void graph_julia(CoordinatePlane2 *plane, _Complex double c, double re_min,
                 double re_max, double im_min, double im_max,
                 int fractal_max_iter);
#endif
