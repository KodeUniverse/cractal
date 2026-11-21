#include "fractal.h"
#include "raylib.h"
#include "render.h"
#include <iso646.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

const struct Color BACKGROUND_COLOR = RAYWHITE;

struct RenderedFractalTexture {
    Texture texture;
    bool preview;
};

typedef enum {
    FRACTAL_MANDELBROT,
    FRACTAL_JULIA,
} FractalType;

static void update_view_bounds(FractalView *view, double re_center,
                               double im_center, double s) {
    double half_re = (double)(view->width - 1) * s / 2.0;
    double half_im = (double)(view->height - 1) * s / 2.0;

    view->re_min = re_center - half_re;
    view->re_max = re_center + half_re;
    view->im_min = im_center - half_im;
    view->im_max = im_center + half_im;
}

static Texture create_selected_fractal_texture(FractalType type,
                                               const FractalView *view,
                                               double complex julia_c) {
    switch (type) {
    case FRACTAL_MANDELBROT:
        return create_mandelbrot_texture(view);
    case FRACTAL_JULIA:
        return create_julia_texture(view, julia_c);
    }

    return (Texture){0};
}

int main(void) {

    const int screen_width = 2560;
    const int screen_height = 1440;
    const FractalType fractal_type = FRACTAL_MANDELBROT;
    const double complex julia_c = -0.8 + 0.156 * I;

    Rectangle screen_rect = {0, 0, screen_width, screen_height};
    Vector2 origin = {0.0, 0.0};
    FractalView view = {
        .width = screen_width,
        .height = screen_height,
        .re_min = -2,
        .re_max = 2,
        .im_min = -1,
        .im_max = 1,
        .max_iter = 5000,
    };

    FractalView preview = view;
    preview.height = view.height / 4.0;
    preview.width = view.width / 4.0;
    Rectangle preview_rect = {0, 0, preview.width, preview.height};

    double re_center = (view.re_min + view.re_max) / 2.0;
    double im_center = (view.im_min + view.im_max) / 2.0;
    double s = (view.re_max - view.re_min) / (view.width - 1);
    update_view_bounds(&view, re_center, im_center, s);

    InitWindow(screen_width, screen_height, "Cractal");
    SetTargetFPS(60);

    struct RenderedFractalTexture fractal_texture = {
        create_selected_fractal_texture(fractal_type, &view, julia_c), false};

    if (fractal_texture.texture.id == 0) {
        CloseWindow();
        return 1;
    }

    const double PREVIEW_INTERVAL = 0.1;
    const double RENDER_SETTLE_DELAY = 0.25;
    double last_view_change_time = GetTime();
    double last_preview_time = 0.0;

    bool view_changed = false;
    while (!WindowShouldClose()) {
        // Update
        bool dragging = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        bool input_changed_this_frame = false;
        if (dragging) {
            Vector2 delta = GetMouseDelta();

            if (delta.x != 0.0f || delta.y != 0.0f) {
                re_center -= (double)delta.x * s;
                im_center += (double)delta.y * s;
                view_changed = true;
                input_changed_this_frame = true;
            }
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            double zoom_delta = 0.2 * (double)wheel;
            double new_s = s * exp(-zoom_delta);
            if (new_s > 0.0 && isfinite(new_s) && new_s != s) {
                s = new_s;
                view_changed = true;
                input_changed_this_frame = true;
            }
        }

        double now = GetTime();
        if (input_changed_this_frame)
            last_view_change_time = now;

        // Throttle low-resolution previews while the view is changing.
        if (view_changed && input_changed_this_frame &&
            now - last_preview_time >= PREVIEW_INTERVAL) {
            update_view_bounds(&preview, re_center, im_center, s * 4);

            Texture preview_texture = create_selected_fractal_texture(
                fractal_type, &preview, julia_c);
            if (preview_texture.id != 0) {
                UnloadTexture(fractal_texture.texture);
                fractal_texture.texture = preview_texture;
                fractal_texture.preview = true;
            } else {
                fprintf(stderr, "ERROR: Could not create preview texture.\n");
            }
            last_preview_time = now;
        }

        // Render at full resolution after input has settled.
        if (view_changed && !dragging &&
            now - last_view_change_time >= RENDER_SETTLE_DELAY) {
            update_view_bounds(&view, re_center, im_center, s);
            Texture new_texture =
                create_selected_fractal_texture(fractal_type, &view, julia_c);
            if (new_texture.id != 0) {
                UnloadTexture(fractal_texture.texture);
                fractal_texture.texture = new_texture;
                fractal_texture.preview = false;
            } else {
                fprintf(stderr, "ERROR: Could not create fractal texture.\n");
            }
            view_changed = false;
        }

        // Draw
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        if (!fractal_texture.preview)
            DrawTexture(fractal_texture.texture, 0, 0, WHITE);
        else
            DrawTexturePro(fractal_texture.texture, preview_rect, screen_rect,
                           origin, 0.0, WHITE);
        EndDrawing();
    }

    UnloadTexture(fractal_texture.texture);
    CloseWindow();
    return 0;
}
