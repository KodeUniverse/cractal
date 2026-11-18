#include "plane.h"
#include "raylib.h"

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
