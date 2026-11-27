# Project guidance

## Project context

Cractal is a small fractal visualizer written in C with raylib. It is meant to
be a library for many fractals. Each fractal is a GLSL fragment shader, and
the C side loads the shader, sets its uniforms, and draws it. There is no CPU
fractal code anymore.

Build and run:

- `make` builds `./cractal` (`make clean` removes it). The Makefile uses
  `pkg-config` to find raylib (currently 6.2.0 at `/usr/local`) and compiles
  `src/*.c` with `-std=c11 -Wall -Wextra -lm`.
- Run from the repository root. The shaders are loaded from relative paths
  such as `./src/shaders/julia.fs`.

Files:

- `src/fractal.h` / `src/fractal.c`: the library.
  - `BaseFractalParam`: width, height, re/im bounds, and `max_iter`. Shared by
    every fractal.
  - `MandelbrotFractalParams` and `JuliaFractalParams`: each contains a
    `BaseFractalParam base`. Julia also has `double complex c`.
  - `FractalShader`: the loaded `Shader` plus the locations of `max_iter`,
    `screen_dims`, and `actual_plane_bounds`.
  - `load_mandelbrot_shader(path, params)` and `load_julia_shader(path, params)`:
    load the shader, set its uniforms, and return a `FractalShader`. Check
    `shader.id == 0` to detect a failed load. `load_julia_shader` sets
    `julia_c` once. That value is kept in the loader, not in `FractalShader`.
  - `update_fractal_shader(shader, base_params)`: writes the shared uniforms
    each time it is called. The `max_iter` in the params is used for that draw.
- `src/shaders/mandelbrot.fs`: the Mandelbrot fragment shader (GLSL 330). It
  takes `max_iter`, `screen_dims`, and `actual_plane_bounds` (vec4 with
  xy = re_min/re_max and zw = im_min/im_max). It maps each pixel to `c`,
  iterates `z = z*z + c` from `z = 0`, and colors by a log-scaled escape count
  (blue to red; black if it never escapes).
- `src/shaders/julia.fs`: the Julia fragment shader. It is a copy of the
  Mandelbrot shader, except `z` starts at the pixel's coordinate, `c` is read
  from the `julia_c` uniform (vec2), and it iterates with that constant `c`.
- `src/main.c`: the driver and example of using the library. It builds a
  `JuliaFractalParams` (2560x1440, re in [-2, 2], im in [-1, 1],
  `max_iter = 8000`, `c = -0.8 + 0.156i`), loads the Julia shader, and runs the
  window loop.
  - While the mouse is moving, it draws a live preview at `PREVIEW_ITER = 2000`.
  - After input has been idle for 0.2 s, it renders once at `max_iter` into a
    cached `RenderTexture2D` and draws that until the view changes.
- `src/shaders/mandelbrot.fs` and `src/shaders/julia.fs` are loaded by path at
  runtime. They are not compiled into the binary.
- `README.md`: a short project description and a Julia screenshot.

Data flow: the pixel-to-complex-number mapping and the escape-count loop run
on the GPU in the fragment shader. C only sets the uniforms, which are the
bounds, the image size, `max_iter`, and (for Julia) `c`. Mandelbrot and Julia
use the same color function and the same uniform names, so a change to one
shader's coloring or bounds mapping should usually be made to the other too.

Known limits to keep in mind:

- The shader uses 32-bit `float` for bounds and iteration, so zooming very
  deep will show precision artifacts.
- The Julia constant `c` is fixed in `main.c`. There is no input to change it
  yet. Changing it later means editing `JuliaFractalParams.c`, then calling
  `load_julia_shader` again or setting `julia_c` on the existing shader.
- `main.c` only opens the Julia fractal. `load_mandelbrot_shader` is
  implemented but not used by the driver yet.

The project owner is learning C, raylib, GLSL, and fractal mathematics,
especially complex numbers. Treat this as a learning project: help them
understand the ideas and build the implementation themselves.

## How to help

- Explain the reasoning behind suggested code, including relevant C concepts
  and fractal/complex-number math, in beginner-friendly language.
- Prefer small, incremental changes and guided questions over writing or
  replacing whole features. Offer a short explanation or plan before substantial
  edits, and let the owner make design choices where practical.
- When useful, ask the owner to predict what a small piece of code or equation
  will do, then use that to build understanding. Do not make help needlessly
  cryptic or withhold a direct answer when they ask for one.
- Favor clear, idiomatic C and explain unfamiliar patterns (types, pointers,
  structs embedded by value, headers, error handling, and shader uniforms)
  when they arise.
- For raylib and GLSL work, introduce concepts step by step and connect them to
  the existing math rather than replacing it without reason.
- Preserve working Mandelbrot and Julia functionality unless the owner
  explicitly asks to change it. Avoid unrelated refactors.
- Check `make` after C changes. The project builds with no warnings today, so
  keep it that way.
- Inspect the repository before relying on build commands or project
  conventions; this file can go stale.
