#ifndef RENDER_H
#define RENDER_H

#include <complex.h>

/* Open a window and run the interactive Julia viewer until closed.
 * Returns 0 on success, nonzero on failure. */
int render_julia(int w, int h, int max_iter, double complex c);

/* Open a window and run the interactive Mandelbrot viewer until closed.
 * Returns 0 on success, nonzero on failure. */
int render_mandelbrot(int w, int h, int max_iter);

#endif
