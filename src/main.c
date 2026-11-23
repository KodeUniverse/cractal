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

    const FractalView view = {
        .width = screen_width * 6,
        .height = screen_height * 6,
        .re_min = -2,
        .re_max = 2,
        .im_min = -1,
        .im_max = 1,
        .max_iter = 5000,
    };

    InitWindow(screen_width, screen_height, "Cractal");
    SetTargetFPS(60);
    Camera2D camera = {0};

    Texture fractal_texture = create_mandelbrot_texture(&view);
    if (fractal_texture.id == 0) {
        CloseWindow();
        return 1;
    }

    camera.offset = (Vector2){screen_width / 2.0f, screen_height / 2.0f};
    camera.target = (Vector2){fractal_texture.width / 2.0f,
                              fractal_texture.height / 2.0f};
    float zoom_x = (float)screen_width / fractal_texture.width;
    float zoom_y = (float)screen_height / fractal_texture.height;
    camera.zoom = zoom_x < zoom_y ? zoom_x : zoom_y;

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
        DrawTexture(fractal_texture, 0, 0, WHITE);
        EndMode2D();
        EndDrawing();
    }

    UnloadTexture(fractal_texture);
    CloseWindow();
    return 0;
}
