#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    bool help;
    bool verbose;
} BasicFlags;

typedef int WindowSize[2];

typedef struct {
    WindowSize size;
    int max_iter;
} RenderFlags;

typedef struct {
    BasicFlags flags;
    RenderFlags render_flags;
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

void parse_args(int argc, char **argv, Flags flags) {

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (strcmp(arg, "list") == 0)
            list_fractals();
        else if (strcmp(arg, "render") == 0) {
            // rendering path here
        } else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            show_prompt();
        } else if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0) {
            flags.verbose = true;
        } else if (strcmp(arg, "-s") == 0 || strcmp(arg, "--size") == 0) {
            // need to collect the next arg and update WindowSize
        } else {
            show_prompt();
        }
    }
}
int main(int argc, char **argv) {

    BasicFlags basic = {false, false};
    RenderFlags render = {1920, 1080, 20000};
    Flags flags = {.flags = basic, .render_flags = render};

    if (argc == 1)
        show_prompt();
    else if (argc >= 2) {
        parse_args(argc, argv);
    }
    return 0;
}
