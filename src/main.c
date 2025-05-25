#include <stdlib.h>

#include "cli.h"
#include "project.h"

int main(int argc, char **argv) {
    cli_options_t opts;
    if (parse_cli(argc, argv, &opts) != 0) {
        opts_free(&opts);
        return EXIT_FAILURE;
    }

    if (opts.show_version) {
        version();
        return EXIT_SUCCESS;
    }
    if (opts.show_help) {
        help();
        return EXIT_SUCCESS;
    }

    int ret = project_generate(&opts) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;

    opts_free(&opts);

    return ret;
}
