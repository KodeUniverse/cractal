#ifndef RENDER_H
#define RENDER_H

#include "fractal.h"
#include "raylib.h"

void render_mandelbrot(CoordinatePlane2 *plane, int max_iter);

/* Call after InitWindow. The caller owns the returned texture and must
 * unload it.
 */
Texture create_mandelbrot_texture(const FractalView *view);
Texture create_julia_texture(const FractalView *view, double complex c);

#endif
