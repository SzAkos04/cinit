#include <stdlib.h>

#include "cli.h"
#include "project.h"

int main(int argc, char **argv) {
    cli_options_t opts = opts_default();
    if (parse_cli(argc, argv, &opts) != 0) {
        return EXIT_FAILURE;
    }

    int ret = project_generate(opts) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;

    opts_free(&opts);

    return ret;
}
