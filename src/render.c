#include "fractal.h"
#include "raylib.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    Color *points;
    size_t width;
    size_t height;
} RenderGrid2D;

static RenderGrid2D create_render_grid2(size_t width, size_t height) {
    RenderGrid2D grid = {NULL, 0, 0};

    if (height == 0 || width > SIZE_MAX / height)
        return grid;

    size_t point_count = width * height;
    if (point_count > SIZE_MAX / sizeof(*grid.points))
        return grid;

    grid.points = calloc(point_count, sizeof(*grid.points));
    if (grid.points == NULL)
        return grid;

    grid.width = width;
    grid.height = height;
    return grid;
}

static void destroy_render_grid2(RenderGrid2D *grid) {
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

static bool valid_fractal_view(const FractalView *view) {
    if (view == NULL || view->width < 2 || view->height < 2 ||
        view->width > INT_MAX || view->height > INT_MAX ||
        view->max_iter <= 0 || view->re_min >= view->re_max ||
        view->im_min >= view->im_max)
        return false;

    if (view->width > SIZE_MAX / view->height)
        return false;

    size_t point_count = view->width * view->height;
    return point_count <= SIZE_MAX / sizeof(double) &&
           point_count <= SIZE_MAX / sizeof(Color);
}

static Texture create_texture_from_plane(CoordinatePlane2 *plane,
                                         int max_iter) {
    if (plane == NULL || plane->points == NULL || plane->width < 2 ||
        plane->height < 2 || plane->width > INT_MAX ||
        plane->height > INT_MAX || max_iter <= 0)
        return (Texture){0};

    // create and fill pixel grid
    RenderGrid2D px_grid = create_render_grid2(plane->width, plane->height);
    if (px_grid.points == NULL)
        return (Texture){0};

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
        }
    }

    Image img = {.data = px_grid.points,
                 .width = (int)px_grid.width,
                 .height = (int)px_grid.height,
                 .mipmaps = 1,
                 .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
    Texture texture = LoadTextureFromImage(img);
    destroy_render_grid2(&px_grid);
    return texture;
}

Texture create_mandelbrot_texture(const FractalView *view) {
    if (!valid_fractal_view(view))
        return (Texture){0};

    CoordinatePlane2 plane = create_plane2(view->width, view->height);
    if (plane.points == NULL)
        return (Texture){0};

    graph_mandelbrot(&plane, view->re_min, view->re_max, view->im_min,
                     view->im_max, view->max_iter);
    Texture texture = create_texture_from_plane(&plane, view->max_iter);
    destroy_plane2(&plane);
    return texture;
}

Texture create_julia_texture(const FractalView *view, double complex c) {
    if (!valid_fractal_view(view))
        return (Texture){0};

    CoordinatePlane2 plane = create_plane2(view->width, view->height);
    if (plane.points == NULL)
        return (Texture){0};

    graph_julia(&plane, c, view->re_min, view->re_max, view->im_min,
                view->im_max, view->max_iter);
    Texture texture = create_texture_from_plane(&plane, view->max_iter);
    destroy_plane2(&plane);
    return texture;
}
