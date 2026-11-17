#include "fractal.h"
#include <complex.h>
#include <stddef.h>

static int mandelbrot(double complex c, int max_iter) {
    double complex z = 0;
    for (int i = 0; i < max_iter; i++) {
        if (creal(z) * creal(z) + cimag(z) * cimag(z) > 4.0)
            return i; // escaped: c is outside the set
        z = z * z + c;
    }
    return max_iter; // never escaped: assume c is inside the set
}

void graph_mandelbrot(CoordinatePlane2 *plane, double re_min, double re_max,
                      double im_min, double im_max, int fractal_max_iter) {

    if (plane == NULL || plane->points == NULL || plane->width < 2 ||
        plane->height < 2)
        return;

    double re_step = (re_max - re_min) / (plane->width - 1);
    double im_step = (im_max - im_min) / (plane->height - 1);
    // row major order
    for (size_t py = 0; py < plane->height; py++) {
        for (size_t px = 0; px < plane->width; px++) {
            double re = re_min + px * re_step;
            double im = im_max - py * im_step;
            plane->points[py * plane->width + px] =
                mandelbrot(re + im * I, fractal_max_iter);
        }
    }
}
