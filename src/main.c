#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIM 15360

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

typedef struct {
    BasicFlags basic;
    RenderFlags render;
} Flags;

const char *FRACTAL_LIST[2] = {"mandelbrot", "julia"};
const int FRACTAL_CT = 2;

void list_fractals() {
    for (int i = 0; i < FRACTAL_CT; i++) {
        printf("  %s\n", FRACTAL_LIST[i]);
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
        "Usage:\n  cractal [command] [flags]\n\n"
        "Commands:\n"
        "  list [OPTIONS]\n"
        "  render [FRACTAL] [RENDER OPTIONS]\n\n"
        "Basic Options:\n"
        "  -h --help\tDisplay help info\n"
        "  -v --verbose\tNon-truncated output for commands that write stdout\n"
        "Render Options:\n"
        "  -s --size\tSpecify output window size, e.g. --size "
        "1920x1080\n"
        "  -i --max-iter\tSpecify max iterations to run the fractal function\n";

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

void parse_args(int argc, char **argv, Flags *flags) {

    char *cmd = argv[1];
    if (strcmp(cmd, "list") != 0 && strcmp(cmd, "render") != 0) {
        show_prompt();
    } else {
        for (int i = 2; i < argc; i++) {
            char *arg = argv[i];
            if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
                show_prompt();
            } else if (strcmp(arg, "-v") == 0 ||
                       strcmp(arg, "--verbose") == 0) {
                flags->basic.verbose = true;
            } else if (strcmp(arg, "-s") == 0 || strcmp(arg, "--size") == 0) {
                if (i + 1 >= argc) {
                    show_prompt();
                } else {
                    char *next_arg = argv[++i];
                    WindowSize size;
                    if (!size_arg_convert(next_arg, &size))
                        show_prompt();
                    else {
                        flags->render.size = size;
                    }
                }
            } else {
                show_prompt();
            }
        }
    }
}

int main(int argc, char **argv) {

    BasicFlags basic = {false, false};
    RenderFlags render = {.size = {.width = 1920, .height = 1080},
                          .max_iter = 10000};
    Flags flags = {.basic = basic, .render = render};

    if (argc == 1)
        show_prompt();
    else if (argc >= 2) {
        parse_args(argc, argv, &flags);
    }
    return 0;
}
