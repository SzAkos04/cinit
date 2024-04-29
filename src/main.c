#include "debug.h"
#include "project.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char MAYBE_UNUSED **argv) {
    if (argc < 2) {
        error("incorrect usage, for help run `cinit --help`");
        return EXIT_FAILURE;
    }
    char err_msg[128];

    CMD command = CMD_NONE;
    unsigned int flags = 0;
    char *name = NULL;

    flags |= FLAG_C; // make c default

    // parse arguments
    for (int i = 1; i < argc; i++) {
        int len = strlen(argv[i]);
        if (len > 1 && argv[i][0] == '-') {
            if (argv[i][1] == '-') {
                if (!strcmp(argv[i], "--help")) {
                    help();
                    return EXIT_SUCCESS;
                } else if (!strcmp(argv[i], "--c")) {
                    flags &= ~FLAG_CPP;
                    flags |= FLAG_C;
                } else if (!strcmp(argv[i], "--cpp")) {
                    flags &= ~FLAG_C;
                    flags |= FLAG_CPP;
                } else {
                    snprintf(err_msg, sizeof(err_msg),
                             "unknown argument `%s`, "
                             "for more info run `cinit --help`",
                             argv[i]);
                    error(err_msg);
                    return EXIT_FAILURE;
                }
            } else {
                for (int j = 1; j < len; j++) {
                    switch (argv[i][j]) {
                    case 'h':
                        help();
                        return EXIT_SUCCESS;
                    default:
                        snprintf(err_msg, sizeof(err_msg),
                                 "unknown argument `-%c', "
                                 "for more info run `cinit --help",
                                 argv[i][j]);
                        error(err_msg);
                        return EXIT_FAILURE;
                    }
                }
            }
        } else {
            if (!strcmp(argv[i], "create") || !strcmp(argv[i], "c")) {
                if (i + 1 < argc && is_correct_name(argv[i + 1])) {
                    command = CMD_CREATE;
                    name = argv[++i];
                } else {
                    error("incorrect usage, for help run `cinit --help`");
                    return EXIT_FAILURE;
                }
            } else if (!strcmp(argv[i], "init") || !strcmp(argv[i], "i")) {
                if (i + 1 < argc && is_correct_name(argv[i + 1])) {
                    name = argv[++i];
                }
                command = CMD_INIT;
            } else {
                snprintf(err_msg, sizeof(err_msg),
                         "unknown command `%s`, "
                         "for more info run `cinit --help`",
                         argv[i]);
                error(err_msg);
                return EXIT_FAILURE;
            }
        }
    }

    if (command == CMD_NONE) {
        error("no command provided, for more info run `cinit --help`");
        return EXIT_FAILURE;
    }

    Project project = project_new(command, flags, name);
    if (project.create(&project) != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
