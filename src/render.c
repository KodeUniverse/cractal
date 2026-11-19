#include "plane.h"
#include "raylib.h"
#include <stdlib.h>

typedef struct {
    Color *points;
    size_t width;
    size_t height;
} RenderGrid2D;

RenderGrid2D create_render_grid2(size_t width, size_t height) {
    RenderGrid2D grid = {NULL, 0, 0};
    Color *points = calloc(width * height, sizeof(Color));
    if (points == NULL)
        return grid;

    grid.points = points;
    grid.width = width;
    grid.height = height;
    return grid;
}

void destroy_render_grid2(RenderGrid2D *grid) {
    if (grid == NULL || grid->points == NULL)
        return;

    free(grid->points);
    grid->points = NULL;
    grid->height = 0;
    grid->width = 0;
}

static Color color_for_escape_count(double iterations, int max_iter) {
    float reference_count = 0.03f * (float)max_iter;
    float progress = (float)iterations / ((float)iterations + reference_count);

    // hue in degrees. 240 is blue, goes to red depending on progess (i.e. as
    // points escape)
    float hue = 240.0f * (1.0f - progress);

    return ColorFromHSV(hue, 0.9f, 1.0f);
}

void render_mandelbrot(CoordinatePlane2 *plane, int max_iter) {
    for (size_t py = 0; py < plane->height; py++) {
        for (size_t px = 0; px < plane->width; px++) {
            double point = plane->points[py * plane->width + px];

            Color pixel_color;

            if (point == max_iter) {
                // in set
                pixel_color = BLACK;
            } else {
                pixel_color = color_for_escape_count(point, max_iter);
            }

            DrawPixel(px, py, pixel_color);
        }
    }
}

Texture create_fractal_texture(CoordinatePlane2 *plane, int max_iter) {
    // create and fill pixel grid
    RenderGrid2D px_grid = create_render_grid2(plane->width, plane->height);
    for (size_t py = 0; py < plane->height; py++) {
        for (size_t px = 0; px < plane->width; px++) {
            double point = plane->points[py * plane->width + px];

            Color pixel_color;

            if (point == max_iter) {
                // in set
                pixel_color = BLACK;
            } else {
                pixel_color = color_for_escape_count(point, max_iter);
            }
            px_grid.points[py * plane->width + px] = pixel_color;
            // DrawPixel(px, py, pixel_color);
        }
    }

    Image img = {.data = px_grid.points,
                 .width = px_grid.width,
                 .height = px_grid.height,
                 1,
                 .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
    Texture texture = LoadTextureFromImage(img);
    destroy_render_grid2(&px_grid);
    return texture;
}
