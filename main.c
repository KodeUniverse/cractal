#include "fractal.h"
#include <stdio.h>

int main(void) {
    CoordinatePlane2 plane = create_plane2(100, 30);
    if (plane.points == NULL)
        return 1;

    int max_iter = 50;
    graph_mandelbrot(&plane, -2.5, 1.0, -1.0, 1.0, max_iter);

    for (size_t y = 0; y < plane.height; y++) {
        for (size_t x = 0; x < plane.width; x++) {
            int n = (int)plane.points[y * plane.width + x];
            putchar(n == max_iter ? '#' : n > 10 ? '+' : n > 4 ? '.' : ' ');
        }
        putchar('\n');
    }

    destroy_plane2(&plane);
    return 0;
}
