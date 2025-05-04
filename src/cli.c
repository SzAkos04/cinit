#include "cli.h"

#include "debug.h"
#include "fs_utils.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

cli_options_t opts_default(void) {
    return (cli_options_t){
        .lang = LANG_C,
        .show_version = false,
        .show_help = false,
        .cmd = CMD_NONE,
        .name = NULL,
    };
}

int parse_cli(int argc, char **argv, cli_options_t *opts) {
    if (argc < 2) {
        error("incorrect usage, for help run `cinit --help`");
        return -1;
    }

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        int arg_len = strlen(arg);
        if (arg[0] == '-') { // arguments
            if (arg_len < 2) {
                error("unknown argument `%s`, "
                      "for more info run `cinit --help`",
                      arg);
                return -1;
            }
            if (arg[1] == '-') { // long arguments
                if (strcmp(arg, "--c") == 0) {
                    opts->lang = LANG_C;
                } else if (strcmp(arg, "--cpp") == 0) {
                    opts->lang = LANG_CPP;
                } else if (strcmp(arg, "--version") == 0) {
                    opts->show_version = true;
                } else if (strcmp(arg, "--help") == 0) {
                    opts->show_help = true;
                } else {
                    error("unknown argument `%s`, "
                          "for more info run `cinit --help`",
                          arg);
                    return -1;
                }
            } else { // short arguments
                for (int j = 1; j < arg_len; ++j) {
                    switch (arg[j]) {
                    case 'v':
                        opts->show_version = true;
                        break;
                    case 'h':
                        opts->show_help = true;
                        break;
                    default:
                        error("unknown argument `-%c`, "
                              "for more info run `cinit --help`",
                              arg[j]);
                        return -1;
                    }
                }
            }
        } else { // commands
            if (strcmp(arg, "create") == 0 || strcmp(arg, "c") == 0) {
                if (i + 1 < argc && is_correct_name(argv[i + 1])) {
                    opts->name = argv[++i];
                } else {
                    error("incorrect usage, for help run `cinit --help`");
                    return -1;
                }

                char *cur_dir = current_dir();
                if (!cur_dir) {
                    perr("failed to get current path");
                    return -1;
                }

                int path_len = strlen(cur_dir) + strlen(opts->name) + 2;
                opts->path = (char *)malloc(path_len + 1);
                snprintf(opts->path, path_len, "%s%c%s", cur_dir,
                         PATH_SEPARATOR, opts->name);

                free(cur_dir);

            } else if (strcmp(arg, "init") == 0 || strcmp(arg, "i") == 0) {
                // create the project in the current directory
                opts->path = current_dir();
                if (!opts->path) {
                    perr("failed to get current path");
                    return -1;
                }

                // check if the next argument starts with `-`
                // if it does the default name for the project is the name of
                // the current folder
                if (i + 1 < argc && is_correct_name(argv[i + 1])) {
                    opts->name = argv[++i];
                    // if it doesn't the next argument is
                    // expected to be the name of the project
                } else {
                    warning(
                        "no name provided, using the name of the directory");
                    // find the current folders name by finding the last
                    // separator in the path
                    char *last_sep = strrchr(opts->path, PATH_SEPARATOR);
                    // if the current folder's name returns NULL, fallback to
                    // the full path
                    if (last_sep && *(last_sep + 1) != '\0') {
                        opts->name = last_sep + 1;
                    } else {
                        opts->name = opts->path;
                    }
                }

            } else {
                error("unknown command `%s`, "
                      "for more info run `cinit --help`",
                      arg);
                return -1;
            }
        }
    }

    return 0;
}

void opts_free(cli_options_t *opts) { free(opts->path); }

bool is_correct_name(const char *str) {
    if (!isalpha(str[0])) {
        return false;
    }
    int len = strlen(str);
    if (len > 32) {
        return false;
    }
    for (int i = 0; i < len; i++) {
        if (!(isalnum(str[i]) || str[i] == '_')) {
            return false;
        }
    }
    return true;
}
