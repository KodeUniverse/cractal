#include "raylib.h"
#include <stdlib.h>

int main(void) {

    const int screen_w = 1920;
    const int screen_h = 1080; 
    InitWindow(screen_w, screen_h, "Shader Testing");
    SetTargetFPS(60);

    Shader shader = LoadShader(NULL, "./mandelbrot.fs");

    int max_iter_loc = GetShaderLocation(shader, "max_iter");
    int dims_loc     = GetShaderLocation(shader, "screen_dims");
    int bounds_loc   = GetShaderLocation(shader, "actual_plane_bounds");

    // Values
    int max_iter = 20000;

    float screen_dims[2] = { (float)screen_w, (float)screen_h };

    // Real axis -2.5..1.0, imaginary range chosen to match the window's aspect ratio
    float re_min = -2.5f, re_max = 1.0f;
    float im_half = (re_max - re_min) * ((float)screen_h / screen_w) * 0.5f;
    // layout: xy = (re_min, re_max), zw = (im_min, im_max)
    float bounds[4] = { re_min, re_max, -im_half, im_half };

    // Uniforms persist, so setting them once is enough while they don't change
    SetShaderValue(shader, max_iter_loc, &max_iter,   SHADER_UNIFORM_INT);
    SetShaderValue(shader, dims_loc,     screen_dims, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, bounds_loc,   bounds,      SHADER_UNIFORM_VEC4);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            BeginShaderMode(shader);
                DrawRectangle(0, 0, screen_w, screen_h, WHITE);
            EndShaderMode();
        EndDrawing();
    }

    UnloadShader(shader);
    CloseWindow();
    return 0;
}
