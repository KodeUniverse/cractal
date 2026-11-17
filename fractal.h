#include <complex.h>
#include <stddef.h>

#ifndef FRACTAL_H
#define FRACTAL_H

#include "plane.h"

void graph_mandelbrot(CoordinatePlane2 *plane, double re_min, double re_max,
                      double im_min, double im_max, int fractal_max_iter);
#endif
