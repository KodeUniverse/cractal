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
  iterates `z = z*z + c` from `z = 0`, and colors by a smooth escape count
  (escape radius 256, `nu = i + 1 - log2(ln|z|)`) and maps it to a color with
  `COLOR_SCHEME`. Black if it never escapes. Two schemes are defined in each
  shader: `scheme_navy_orange` (the default, monotonic) and `scheme_rainbow`
  (cosine, wraps every 20 iterations). To switch, change the `#define
  COLOR_SCHEME` line in both `mandelbrot.fs` and `julia.fs`. Scales are fixed,
  not divided by `max_iter`, so preview and full render colors match.
- `src/shaders/julia.fs`: the Julia fragment shader. It is a copy of the
  Mandelbrot shader, except `z` starts at the pixel's coordinate, `c` is read
  from the `julia_c` uniform (vec2), and it iterates with that constant `c`.
- `src/render.h` / `src/render.c`: the window layer.
  - `render_julia(w, h, max_iter, c)` and `render_mandelbrot(w, h, max_iter)`
    open a window, load the shader, run the interactive loop, and return 0 on
    success. They take plain values and know nothing about the CLI.
  - Both use one static loop, `run_fractal_window`. While the mouse is moving,
    it draws a live preview at `min(max_iter, PREVIEW_ITER)` with
    `PREVIEW_ITER = 20000`. After input has been idle for 0.2 s, it renders
    once at the full `max_iter` into a cached `RenderTexture2D` and draws that
    until the view changes. The preview cap is only for responsiveness. The
    final render is not capped.
  - Mandelbrot view: re in [-2, 2], im in [-1, 1]. Julia uses the same bounds.
- `src/main.c`: the CLI. It parses `list` and `render` commands and flags into
  an `Args` struct. `Args` holds `RenderFlags` (size, max_iter), a
  `FractalArgs` (fractal parameters only, currently `julia_c`), and a pointer
  to the chosen `FractalInfo`. `parse_args` returns false on any bad command,
  subject, fractal, flag, or value after printing an error to stderr, and
  `main` exits 1 without running anything. `-h` shows help and exits 0.
  - Defaults: 1920x1080 window, `max_iter = 20000`, Julia `c = -0.8 + 0.156i`.
    `main()` sets these before parsing, so `FractalArgs` never holds "unset".
  - Flags: `-s/--size WxH`, `-i/--max-iter n`, `-c/--julia-c re,im`,
    `-v/--verbose`, `-h/--help`.
  - Fractal registry: `FRACTALS[]` maps each name to a render adapter. The
    parser looks names up there, error messages and `list fractal` read the
    names from there, and `execute_cmd` calls the adapter. Adding a fractal
    means writing an adapter and adding one row. Do not hardcode fractal names
    anywhere else.
  - Option checking is not implemented yet. `-c` is accepted for any render
    and ignored by fractals that do not use it (e.g. `render mandelbrot -c 1,0`
    runs normally). This will be redesigned later.
  - Project scope is not just Julia and Mandelbrot. Do not write messages,
    comments, or docs that assume only those two exist.
- Conventions for CLI and params:
  - `FractalArgs` holds fractal parameters only (values with defaults). Do not
    add flags, "was given" markers, or other CLI bookkeeping to it. Track that
    kind of state in `Args` if it is ever needed.
  - The render layer (`render.*`) takes plain values and does not depend on CLI
    types.
- `src/shaders/mandelbrot.fs` and `src/shaders/julia.fs` are loaded by path at
  runtime. They are not compiled into the binary. The loop runs `i < max_iter`
  with no fixed cap.
- `README.md`: a short project description and a Julia screenshot.

Data flow: the pixel-to-complex-number mapping and the escape-count loop run
on the GPU in the fragment shader. C only sets the uniforms, which are the
bounds, the image size, `max_iter`, and (for Julia) `c`. Mandelbrot and Julia
use the same coloring code and the same uniform names, so a change to one
shader's coloring or bounds mapping should usually be made to the other too.

Known limits to keep in mind:

- The shader uses 32-bit `float` for bounds and iteration, so zooming very
  deep will show precision artifacts.
- Julia's `c` comes from `-c/--julia-c` in the CLI, or the default in
  `main.c`. The render layer receives it as a parameter.
- Options are not validated per fractal yet (see the `main.c` notes above).
- The Mandelbrot view is fixed (no CLI knobs yet). The window aspect ratio is
  not corrected: the re/im range is fixed, so non-1920x1080 sizes stretch.

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
