#include "plane.h"
#include <stdio.h>
#include <stdlib.h>

CoordinatePlane2 create_plane2(size_t w, size_t h) {

    double *grid = calloc(w * h, sizeof(double));

    CoordinatePlane2 plane = {w, h, grid};
    return plane;
}

void destroy_plane2(CoordinatePlane2 *plane) {

    if (plane == NULL) {
        fprintf(stderr, "Null pointer passed to destroy_plane2.");
        return;
    }

    free(plane->points);
    plane->points = NULL;
    plane->height = 0;
    plane->width = 0;
}
