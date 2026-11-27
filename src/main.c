#include "fractal.h"
#include "plane.h"
#include "raylib.h"
#include "render.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

const struct Color BACKGROUND_COLOR = RAYWHITE;

static void update_view_bounds(FractalView *view, double re_center,
                               double im_center, double s) {
    double half_re = (double)(view->width - 1) * s / 2.0;
    double half_im = (double)(view->height - 1) * s / 2.0;

    view->re_min = re_center - half_re;
    view->re_max = re_center + half_re;
    view->im_min = im_center - half_im;
    view->im_max = im_center + half_im;
}

int main(void) {

    const int screen_width = 2560;
    const int screen_height = 1440;
    const Rectangle screen_rect = {0, 0, screen_width, screen_height};
    const Vector2 texture_origin = {0, 0};

    FractalView view = {
        .width = screen_width,
        .height = screen_height,
        .re_min = -2,
        .re_max = 2,
        .im_min = -1,
        .im_max = 1,
        .max_iter = 8000,
    };

    double re_center = (view.re_min + view.re_max) / 2.0;
    double im_center = (view.im_min + view.im_max) / 2.0;
    double s = (view.re_max - view.re_min) / (view.width - 1);
    update_view_bounds(&view, re_center, im_center, s);

    InitWindow(screen_width, screen_height, "Cractal");
    SetTargetFPS(60);

    Texture fractal_texture = create_mandelbrot_texture(&view);
    if (fractal_texture.id == 0) {
        CloseWindow();
        return 1;
    }

    const double PREVIEW_INTERVAL = 0.1;
    const double RENDER_SETTLE_DELAY = 0.2;
    double last_view_change_time = GetTime();
    double last_preview_time = GetTime() - PREVIEW_INTERVAL;
    bool view_changed = false;

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
            view_changed = true;
            last_view_change_time = now;
        }

        // Render a smaller texture periodically during interaction.
        if (view_changed && input_changed &&
            now - last_preview_time >= PREVIEW_INTERVAL) {
            update_view_bounds(&view, re_center, im_center, s);

            FractalView preview_view = view;
            preview_view.width = view.width / 4;
            preview_view.height = view.height / 4;

            Texture preview_texture = create_mandelbrot_texture(&preview_view);
            if (preview_texture.id != 0) {
                UnloadTexture(fractal_texture);
                fractal_texture = preview_texture;
            } else {
                fprintf(stderr, "Could not create preview texture.\n");
            }
            last_preview_time = now;
        }

        // Recompute once input settles rather than blocking every drag frame.
        if (view_changed && !dragging &&
            now - last_view_change_time >= RENDER_SETTLE_DELAY) {
            update_view_bounds(&view, re_center, im_center, s);
            Texture new_texture = create_mandelbrot_texture(&view);

            if (new_texture.id != 0) {
                UnloadTexture(fractal_texture);
                fractal_texture = new_texture;
            } else {
                fprintf(stderr, "Could not create updated fractal texture.\n");
            }
            view_changed = false;
        }

        // Draw
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        Rectangle source_rect = {0, 0, (float)fractal_texture.width,
                                 (float)fractal_texture.height};
        DrawTexturePro(fractal_texture, source_rect, screen_rect,
                       texture_origin, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadTexture(fractal_texture);
    CloseWindow();
    return 0;
}
