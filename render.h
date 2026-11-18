#ifndef RENDER_H
#define RENDER_H

#include "plane.h"
#include "raylib.h"

void render_mandelbrot(CoordinatePlane2 *plane, int max_iter);
Texture create_mandelbrot_texture(CoordinatePlane2 *plane, int max_iter);

#endif
