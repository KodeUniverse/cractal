#include "render.h"
#include "fractal.h"
#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define BACKGROUND_COLOR RAYWHITE
#define JULIA_SHADER_PATH "./src/shaders/julia.fs"
#define MANDELBROT_SHADER_PATH "./src/shaders/mandelbrot.fs"

static void update_view_bounds(BaseFractalParam *view, double re_center,
                               double im_center, double s) {
    double half_re = (double)(view->width - 1) * s / 2.0;
    double half_im = (double)(view->height - 1) * s / 2.0;

    view->re_min = re_center - half_re;
    view->re_max = re_center + half_re;
    view->im_min = im_center - half_im;
    view->im_max = im_center + half_im;
}

static void draw_fullscreen_quad(const FractalShader *f, int w, int h) {
    BeginShaderMode(f->shader);
    DrawRectangle(0, 0, w, h, WHITE);
    EndShaderMode();
}

// Runs the interactive loop for an already-open window and loaded shader.
// `view` holds the starting bounds; it is updated as the user pans and zooms.
static void run_fractal_window(FractalShader *fractal, BaseFractalParam *view) {
    const int screen_width = view->width;
    const int screen_height = view->height;

    double re_center = (view->re_min + view->re_max) / 2.0;
    double im_center = (view->im_min + view->im_max) / 2.0;
    double s = (view->re_max - view->re_min) / (view->width - 1);
    update_view_bounds(view, re_center, im_center, s);

    // Cached full-quality render, redrawn only when the view settles
    RenderTexture2D full_render =
        LoadRenderTexture(screen_width, screen_height);
    bool full_render_valid = false;

    const int PREVIEW_ITER = 20000;
    const double RENDER_SETTLE_DELAY = 0.2;
    double last_view_change_time = GetTime();

    while (!WindowShouldClose()) {
        // Update
        bool dragging = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        bool input_changed = false;

        if (dragging) {
            Vector2 delta = GetMouseDelta();
            if (delta.x != 0.0f || delta.y != 0.0f) {
                re_center -= (double)delta.x * s;
                im_center += (double)delta.y * s;
                input_changed = true;
            }
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            double zoom_delta = 0.2 * (double)wheel;
            double new_s = s * exp(-zoom_delta);
            if (new_s > 0.0 && isfinite(new_s) && new_s != s) {
                s = new_s;
                input_changed = true;
            }
        }

        double now = GetTime();
        if (input_changed) {
            update_view_bounds(view, re_center, im_center, s);
            last_view_change_time = now;
            full_render_valid = false;
        }

        bool settled =
            !dragging && now - last_view_change_time >= RENDER_SETTLE_DELAY;

        // Full-quality render once, after input settles
        if (settled && !full_render_valid) {
            update_fractal_shader(fractal, view);
            BeginTextureMode(full_render);
            draw_fullscreen_quad(fractal, screen_width, screen_height);
            EndTextureMode();
            full_render_valid = true;
        }

        // Draw
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        if (full_render_valid) {
            // Render textures are stored upside down: negative height flips
            // them
            Rectangle src = {0, 0, (float)screen_width, -(float)screen_height};
            DrawTextureRec(full_render.texture, src, (Vector2){0, 0}, WHITE);
        } else {
            // Live low-iteration preview while interacting. Never more
            // iterations than the final render.
            BaseFractalParam preview = *view;
            preview.max_iter = view->max_iter < PREVIEW_ITER
                                   ? view->max_iter
                                   : PREVIEW_ITER;
            update_fractal_shader(fractal, &preview);
            draw_fullscreen_quad(fractal, screen_width, screen_height);
        }
        EndDrawing();
    }

    UnloadRenderTexture(full_render);
}

int render_julia(int w, int h, int max_iter, double complex c) {
    JuliaFractalParams julia = {.base =
                                    {
                                        .width = w,
                                        .height = h,
                                        .re_min = -2,
                                        .re_max = 2,
                                        .im_min = -1,
                                        .im_max = 1,
                                        .max_iter = max_iter,
                                    },
                                .c = c};

    InitWindow(w, h, "Cractal - Julia");
    SetTargetFPS(60);

    FractalShader fractal = load_julia_shader(JULIA_SHADER_PATH, &julia);
    if (fractal.shader.id == 0) {
        fprintf(stderr, "error: could not load %s\n", JULIA_SHADER_PATH);
        CloseWindow();
        return 1;
    }

    run_fractal_window(&fractal, &julia.base);

    UnloadShader(fractal.shader);
    CloseWindow();
    return 0;
}

int render_mandelbrot(int w, int h, int max_iter) {
    MandelbrotFractalParams mandelbrot = {.base = {
                                              .width = w,
                                              .height = h,
                                              .re_min = -2,
                                              .re_max = 2,
                                              .im_min = -1,
                                              .im_max = 1,
                                              .max_iter = max_iter,
                                          }};

    InitWindow(w, h, "Cractal - Mandelbrot");
    SetTargetFPS(60);

    FractalShader fractal =
        load_mandelbrot_shader(MANDELBROT_SHADER_PATH, &mandelbrot);
    if (fractal.shader.id == 0) {
        fprintf(stderr, "error: could not load %s\n", MANDELBROT_SHADER_PATH);
        CloseWindow();
        return 1;
    }

    run_fractal_window(&fractal, &mandelbrot.base);

    UnloadShader(fractal.shader);
    CloseWindow();
    return 0;
}
