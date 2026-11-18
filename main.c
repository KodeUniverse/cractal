#include "fractal.h"
#include "plane.h"
#include "raylib.h"
#include "raymath.h"
#include "render.h"
#include <stdio.h>

const struct Color BACKGROUND_COLOR = RAYWHITE;

int terminal_render() {

    CoordinatePlane2 plane = create_plane2(100, 30);
    if (plane.points == NULL)
        return 1;

    int max_iter = 50;
    // oraph_mandelbrot(&plane, -2.5, 1.0, -1.0, 1.0, max_iter);
    graph_julia(&plane, -0.8 + 0.156 * I, -1.6, 1.6, -0.9, 0.9, 100);
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

int main(void) {

    const int screen_width = 2560;
    const int screen_height = 1440;

    int max_iter = 1000;
    CoordinatePlane2 plane = create_plane2(screen_width, screen_height);
    graph_mandelbrot(&plane, -2, 2, -1, 1, max_iter);

    InitWindow(screen_width, screen_height, "Cractal");
    SetTargetFPS(60);
    Camera2D camera = {0};
    camera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        // Update

        // click and drag
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        Vector2 render_scale = {(float)GetRenderWidth() / screen_width,
                                (float)GetRenderHeight() / screen_height};

        // mouse wheel zoom
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            Vector2 mouse_world_pos = GetScreenToWorld2D(
                GetMousePosition(),
                camera); // get world point location of cursor
            camera.offset = Vector2Multiply(
                GetMousePosition(),
                render_scale); // set offset to mouse position. mouse is scaled
                               // to viewport space, but offset is in unscaled
                               // pixels.
            // set camera target to cursor position for zooming
            camera.target = mouse_world_pos;

            // zoom
            float scale = 0.2f * wheel;
            camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);
        }
        // Draw
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(BACKGROUND_COLOR);

        render_mandelbrot(&plane, max_iter);
        EndMode2D();
        EndDrawing();
    }

    return 0;
}
