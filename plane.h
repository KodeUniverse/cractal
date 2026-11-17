#ifndef PLANE_H
#define PLANE_H

#include <stddef.h>

typedef struct {
    size_t width;
    size_t height;

    double *points;

} CoordinatePlane2;

CoordinatePlane2 create_plane2(size_t w, size_t h);
void destroy_plane2(CoordinatePlane2 *plane);

#endif
