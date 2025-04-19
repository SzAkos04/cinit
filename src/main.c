#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "fs_utils.h"

#include "project.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        error("incorrect usage, for help run `cinit --help`");
        return EXIT_FAILURE;
    }

    project_t *project = project_new();

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if (arg[0] == '-') {
            // check long arguments that start with `--`
            if (arg[1] == '-') {
                if (strcmp(arg, "--c") == 0) {
                    project->flags = (project->flags & ~FLAG_CPP) | FLAG_C;
                } else if (strcmp(arg, "--cpp") == 0) {
                    project->flags = (project->flags & ~FLAG_C) | FLAG_CPP;
                } else if (strcmp(arg, "--version") == 0) {
                    version();
                    project_free(project);
                    return EXIT_SUCCESS;
                } else if (strcmp(arg, "--help") == 0) {
                    help();
                    project_free(project);
                    return EXIT_SUCCESS;
                } else {
                    error("unknown argument `%s`, "
                          "for more info run `cinit --help`",
                          arg);
                    project_free(project);
                    return EXIT_FAILURE;
                }

                // check short stackable arguments with only one `-`
            } else {
                int arg_len = strlen(arg);
                // iterate through the characters
                for (int j = 1; j < arg_len; ++j) {
                    switch (arg[j]) {
                    case 'v':
                        version();
                        project_free(project);
                        return EXIT_SUCCESS;
                    case 'h':
                        help();
                        project_free(project);
                        return EXIT_SUCCESS;
                    default:
                        error("unknown argument `-%c`, "
                              "for more info run `cinit --help`",
                              arg[j]);
                        project_free(project);
                        return EXIT_FAILURE;
                    }
                }
            }

            // if the argument doesn't start with `-` it's expected to be a
            // command
        } else {
            if (strcmp(arg, "create") == 0 || strcmp(arg, "c") == 0) {
                if (i + 1 < argc && is_correct_name(argv[i + 1])) {
                    project->name = argv[++i];
                } else {
                    error("incorrect usage, for help run `cinit --help`");
                    project_free(project);
                    return EXIT_FAILURE;
                }

                const char *cur_dir = current_dir();
                if (!cur_dir) {
                    perr("failed to get current path");
                    project_free(project);
                    return EXIT_FAILURE;
                }

                int path_len = strlen(cur_dir) + strlen(project->name) + 2;
                project->path = (char *)malloc(path_len + 1);
                snprintf(project->path, path_len, "%s%c%s", cur_dir,
                         PATH_SEPARATOR, project->name);

            } else if (strcmp(arg, "init") == 0 || strcmp(arg, "i") == 0) {
                // create the project in the current directory
                project->path = current_dir();
                if (!project->path) {
                    perr("failed to get current path");
                    project_free(project);
                    return EXIT_FAILURE;
                }

                // check if the next argument starts with `-`
                // if it does the default name for the project is the name of
                // the current folder
                if (i + 1 < argc && is_correct_name(argv[i + 1])) {
                    project->name = argv[++i];
                    // if it doesn't the next argument is
                    // expected to be the name of the project
                } else {
                    warning(
                        "no name provided, using the name of the directory");
                    // find the current folders name by finding the last
                    // separator in the path
                    char *last_sep = strrchr(project->path, PATH_SEPARATOR);
                    // if the current folder's name returns NULL, fallback to
                    // the full path
                    if (last_sep && *(last_sep + 1) != '\0') {
                        project->name = last_sep + 1;
                    } else {
                        project->name = project->path;
                    }
                }

            } else {
                error("unknown command `%s`, "
                      "for more info run `cinit --help`",
                      arg);
                project_free(project);
                return EXIT_FAILURE;
            }
        }
    }

    int ret = project->generate(project) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;

    project_free(project);
    return ret;
}
