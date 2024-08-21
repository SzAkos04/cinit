#include "debug.h"
#include "project.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        error("incorrect usage, for help run `cinit --help`");
        return EXIT_FAILURE;
    }

    char err_msg[128];
    CMD command = CMD_NONE;
    unsigned int flags = FLAG_C; // make c default
    const char *name = NULL;

    // parse arguments
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        int len = strlen(arg);
        if (len > 1 && arg[0] == '-') {
            if (arg[1] == '-') {
                if (!strcmp(arg, "--help")) {
                    help();
                    return EXIT_SUCCESS;
                } else if (!strcmp(arg, "--version")) {
                    version();
                    return EXIT_SUCCESS;
                } else if (!strcmp(arg, "--c")) {
                    flags = (flags & ~FLAG_CPP) | FLAG_C;
                } else if (!strcmp(arg, "--cpp")) {
                    flags = (flags & ~FLAG_C) | FLAG_CPP;
                } else {
                    snprintf(err_msg, sizeof(err_msg),
                             "unknown argument `%s`, "
                             "for more info run `cinit --help`",
                             arg);
                    error(err_msg);
                    return EXIT_FAILURE;
                }
            } else {
                for (int j = 1; j < len; j++) {
                    switch (arg[j]) {
                    case 'h':
                        help();
                        return EXIT_SUCCESS;
                    case 'v':
                        version();
                        return EXIT_SUCCESS;
                    default:
                        snprintf(err_msg, sizeof(err_msg),
                                 "unknown argument `-%c', "
                                 "for more info run `cinit --help`",
                                 arg[j]);
                        error(err_msg);
                        return EXIT_FAILURE;
                    }
                }
            }
        } else {
            if (!strcmp(arg, "create") || !strcmp(arg, "c")) {
                if (i + 1 < argc && is_correct_name(argv[i + 1])) {
                    command = CMD_CREATE;
                    name = argv[++i];
                } else {
                    error("incorrect usage, for help run `cinit --help`");
                    return EXIT_FAILURE;
                }
            } else if (!strcmp(arg, "init") || !strcmp(arg, "i")) {
                if (i + 1 < argc && is_correct_name(argv[i + 1])) {
                    name = argv[++i];
                }
                command = CMD_INIT;
            } else {
                snprintf(err_msg, sizeof(err_msg),
                         "unknown command `%s`, "
                         "for more info run `cinit --help`",
                         arg);
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

    snprintf(err_msg, sizeof(err_msg), "project `%s` successfully created",
             project.name);
    success(err_msg);
    return EXIT_SUCCESS;
}
