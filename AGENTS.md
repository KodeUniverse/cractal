# Project guidance

## Project context

This is a small fractal visualizer written in C. It currently has working
Mandelbrot and Julia set calculations and a simple terminal display. The next
goal is to begin rendering with raylib.

The repository currently contains:

- `fractal.c` / `fractal.h`: Mandelbrot and Julia iteration and functions that
  fill a plane with iteration counts. The complex-number calculations use C's
  `<complex.h>` support.
- `plane.c` / `plane.h`: `CoordinatePlane2`, which stores width, height, and a
  dynamically allocated, row-major `double` array of per-pixel iteration
  counts. `create_plane2` allocates it and `destroy_plane2` frees it.
- `main.c`: creates a 100-by-30 plane, currently calculates the Julia set, then
  maps iteration counts to terminal characters. The Mandelbrot call is
  commented out. Its display variable is `max_iter = 50`, but the Julia call
  currently uses a limit of 100; keep this mismatch in mind when discussing
  the terminal character mapping, and explain it rather than silently changing
  behavior.
- `README.md`: project name and an example terminal-rendered fractal; there is
  currently no checked-in build configuration or documented build command.

The fractal functions map plane columns/rows to real/imaginary coordinates and
store how many iterations each point takes to escape. A value equal to the
iteration limit is treated as not having escaped within that limit. The plane
is a useful seam between the math and display: prefer building on it when
introducing raylib, and explain any changes to that data flow.

The project owner is learning C and fractal mathematics, especially complex
numbers. Treat this as a learning project: help them understand the ideas and
build the implementation themselves.

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
  memory ownership, headers, and error handling) when they arise.
- For raylib work, introduce rendering concepts step by step and connect them
  to the existing fractal calculations rather than replacing those calculations
  without reason.
- Preserve working Mandelbrot, Julia, and terminal functionality unless the
  owner explicitly asks to change it. Avoid unrelated refactors.
- Do not assume build commands, raylib setup, or project conventions; inspect
  the repository before relying on them.
