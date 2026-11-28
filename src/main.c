#include <complex.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "render.h"

#define MAX_DIM 15360
#define DEFAULT_JULIA_C (-0.8 + 0.156 * I)

typedef struct {
    bool help;
    bool verbose;
} BasicFlags;

typedef struct {
    int width;
    int height;
} WindowSize;

typedef struct {
    WindowSize size;
    int max_iter;
} RenderFlags;

// Fractal parameters only. Flags (like "was -c given?") do not belong here.
// Defaults are set in main() before parsing.
typedef struct {
    double complex julia_c;
} FractalArgs;

typedef enum {
    CMD_NONE,
    CMD_LIST,
    CMD_RENDER,
} Command;

typedef enum {
    SUBJECT_NONE,
    SUBJECT_FRACTAL,
    SUBJECT_THEME,
} Subject;

// One row per fractal. To add a fractal, write an adapter below and add a row
// to FRACTALS. Nothing else in this file should name a fractal.
typedef struct {
    const char *name;
    int (*render)(const RenderFlags *r, const FractalArgs *f);
} FractalInfo;

typedef struct {
    Command cmd;
    Subject subject;
    const FractalInfo *fractal; // NULL until a render target is parsed
    BasicFlags basic;
    RenderFlags render;
    FractalArgs fractal_args;
} Args;

// Adapters: unpack CLI structs into the plain values render.h expects.
static int render_mandelbrot_cmd(const RenderFlags *r, const FractalArgs *f) {
    (void)f; // Mandelbrot has no parameters yet
    return render_mandelbrot(r->size.width, r->size.height, r->max_iter);
}

static int render_julia_cmd(const RenderFlags *r, const FractalArgs *f) {
    return render_julia(r->size.width, r->size.height, r->max_iter,
                        f->julia_c);
}

static const FractalInfo FRACTALS[] = {
    {"mandelbrot", render_mandelbrot_cmd},
    {"julia", render_julia_cmd},
};
#define FRACTAL_CT (sizeof FRACTALS / sizeof FRACTALS[0])

static const FractalInfo *find_fractal(const char *name) {
    for (size_t i = 0; i < FRACTAL_CT; i++) {
        if (strcmp(FRACTALS[i].name, name) == 0)
            return &FRACTALS[i];
    }
    return NULL;
}

// Prints the registered names separated by ", " (no newline).
static void print_fractal_names(FILE *out) {
    for (size_t i = 0; i < FRACTAL_CT; i++)
        fprintf(out, "%s%s", i ? ", " : "", FRACTALS[i].name);
}

void list_theme(bool verbose) {
    if (verbose)
        printf("VERBOSE FLAG ON!\n");
    printf("Listing themes WIP\n");
}

void list_fractals(bool verbose) {

    if (verbose)
        printf("VERBOSE FLAG ON!\n");

    for (size_t i = 0; i < FRACTAL_CT; i++) {
        printf("  %s\n", FRACTALS[i].name);
    }
}

void show_prompt() {

    char *prompt =
        "                     _        _\n"
        "  ___ _ __ __ _  ___| |_ __ _| |\n"
        " / __| '__/ _` |/ __| __/ _` | |\n"
        "| (__| | | (_| | (__| || (_| | |\n"
        " \\___|_|  \\__,_|\\___|\\__\\__,_|_|\n"
        "       fractal visualizer\n\n"
        "Usage:\n  cractal [command] [OPTIONS]\n\n"
        "Commands:\n"
        "  list [fractal|theme] [OPTIONS]\n"
        "  render [fractal] [RENDER OPTIONS] [FRACTAL OPTIONS]\n\n"
        "Basic Options:\n"
        "  -h --help\tDisplay help info\n"
        "  -v --verbose\tNon-truncated output for commands that write stdout\n"
        "Render Options:\n"
        "  -s --size\tSpecify output window size, e.g. --size "
        "1920x1080\n"
        "  -i --max-iter\tSpecify max iterations to run the fractal function\n"
        "Fractal Options:\n"
        "  -c --julia-c\tJulia constant as re,im, e.g. --julia-c -0.8,0.156\n";

    printf("%s\n", prompt);
}

static bool size_arg_convert(const char *arg, WindowSize *out) {

    char *width_end, *height_end;
    long width_l = strtol(arg, &width_end, 10);

    if (*width_end != 'x') {
        return false;
    }

    long height_l = strtol(width_end + 1, &height_end, 10);

    if (*height_end != '\0') {
        return false;
    }

    if ((width_l <= 0 || width_l > MAX_DIM) ||
        (height_l <= 0 || height_l > MAX_DIM)) {
        return false;
    }
    *out = (WindowSize){.width = (int)width_l, .height = (int)height_l};
    return true;
}

// Parses "re,im" (e.g. "-0.8,0.156") into a complex number.
static bool complex_arg_convert(const char *arg, double complex *out) {
    char *re_end, *im_end;
    double re = strtod(arg, &re_end);
    if (re_end == arg || *re_end != ',')
        return false;

    double im = strtod(re_end + 1, &im_end);
    if (im_end == re_end + 1 || *im_end != '\0')
        return false;

    *out = re + im * I;
    return true;
}

// Parses a positive int, e.g. "20000".
static bool positive_int_arg_convert(const char *arg, int *out) {
    char *end;
    long v = strtol(arg, &end, 10);
    if (end == arg || *end != '\0')
        return false;
    if (v <= 0 || v > INT_MAX)
        return false;
    *out = (int)v;
    return true;
}

// Returns false on a bad command, subject, fractal, or flag, after printing an
// error to stderr. Help (-h) is recorded in args->basic.help and wins over a
// missing subject, so `render -h` still shows help.
static bool parse_args(int argc, char **argv, Args *args) {

    char *cmd = argv[1];
    if (strcmp(cmd, "-h") == 0 || strcmp(cmd, "--help") == 0) {
        args->basic.help = true;
        return true;
    }

    if (strcmp(cmd, "list") == 0)
        args->cmd = CMD_LIST;
    else if (strcmp(cmd, "render") == 0)
        args->cmd = CMD_RENDER;
    else {
        fprintf(stderr, "error: unknown command '%s' (see cractal --help)\n",
                cmd);
        return false;
    }

    // The first word after the command is the subject or fractal, unless it
    // is a flag.
    char *target = NULL;
    int i = 2;
    if (argc >= 3 && argv[2][0] != '-') {
        target = argv[2];
        i = 3;
    }

    for (; i < argc; i++) {
        char *arg = argv[i];
        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            args->basic.help = true;
        } else if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0) {
            args->basic.verbose = true;
        } else if (strcmp(arg, "-s") == 0 || strcmp(arg, "--size") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "error: %s needs a value, e.g. 1920x1080\n",
                        arg);
                return false;
            }
            char *value = argv[++i];
            if (!size_arg_convert(value, &args->render.size)) {
                fprintf(stderr,
                        "error: invalid size '%s' (expected WIDTHxHEIGHT, "
                        "each 1-%d)\n",
                        value, MAX_DIM);
                return false;
            }
        } else if (strcmp(arg, "-i") == 0 || strcmp(arg, "--max-iter") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "error: %s needs a value, e.g. 20000\n", arg);
                return false;
            }
            char *value = argv[++i];
            if (!positive_int_arg_convert(value, &args->render.max_iter)) {
                fprintf(stderr,
                        "error: invalid max-iter '%s' (expected a positive "
                        "integer)\n",
                        value);
                return false;
            }
        } else if (strcmp(arg, "-c") == 0 || strcmp(arg, "--julia-c") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "error: %s needs a value, e.g. -0.8,0.156\n",
                        arg);
                return false;
            }
            char *value = argv[++i];
            if (!complex_arg_convert(value, &args->fractal_args.julia_c)) {
                fprintf(stderr,
                        "error: invalid julia-c '%s' (expected re,im, e.g. "
                        "-0.8,0.156)\n",
                        value);
                return false;
            }
        } else {
            fprintf(stderr, "error: unknown option '%s' (see cractal --help)\n",
                    arg);
            return false;
        }
    }

    if (args->basic.help)
        return true;

    if (args->cmd == CMD_LIST) {
        if (target == NULL) {
            fprintf(stderr, "error: list needs a subject: fractal or theme\n");
            return false;
        }
        if (strcmp(target, "fractal") == 0)
            args->subject = SUBJECT_FRACTAL;
        else if (strcmp(target, "theme") == 0)
            args->subject = SUBJECT_THEME;
        else {
            fprintf(stderr,
                    "error: unknown list subject '%s' (expected fractal or "
                    "theme)\n",
                    target);
            return false;
        }
    } else {
        if (target == NULL) {
            fprintf(stderr, "error: render needs a fractal. Available: ");
            print_fractal_names(stderr);
            fprintf(stderr, "\n");
            return false;
        }
        args->fractal = find_fractal(target);
        if (args->fractal == NULL) {
            fprintf(stderr, "error: unknown fractal '%s'. Available: ", target);
            print_fractal_names(stderr);
            fprintf(stderr, "\n");
            return false;
        }
    }
    return true;
}

bool execute_cmd(Args *args) {
    switch (args->cmd) {
    case CMD_NONE:
        break;
    case CMD_LIST:
        if (args->subject == SUBJECT_FRACTAL)
            list_fractals(args->basic.verbose);
        else if (args->subject == SUBJECT_THEME)
            list_theme(args->basic.verbose);
        break;
    case CMD_RENDER:
        // parse_args guarantees args->fractal is set here.
        return args->fractal->render(&args->render, &args->fractal_args) == 0;
    }
    return true;
}

int main(int argc, char **argv) {

    BasicFlags basic = {false, false};
    RenderFlags render = {.size = {.width = 1920, .height = 1080},
                          .max_iter = 20000};
    FractalArgs fractal_args = {.julia_c = DEFAULT_JULIA_C};
    Args args = {.cmd = CMD_NONE,
                 .subject = SUBJECT_NONE,
                 .fractal = NULL,
                 .basic = basic,
                 .render = render,
                 .fractal_args = fractal_args};

    if (argc == 1) {
        show_prompt();
        return 0;
    }
    if (!parse_args(argc, argv, &args))
        return 1;
    if (args.basic.help) {
        show_prompt();
        return 0;
    }
    return execute_cmd(&args) ? 0 : 1;
}
