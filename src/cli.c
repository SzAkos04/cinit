#include "cli.h"

#include "cinitrc.h"
#include "debug.h"
#include "fs_utils.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool use_color = true;
bool silent = false;

static int opts_default(cli_options_t *opts) {
    // check if there is a ~/.cinitrc file
    const char *home = getenv("HOME");
    if (!home) {
        perr("failed to resolve home directory");
        return -1;
    }

    char path[512];
    snprintf(path, sizeof(path), "%s/.cinitrc", home);

    FILE *file = fopen(path, "rb");
    if (file) {
        if (parse_cinitrc(path, opts) != 0) {
            return -1;
        }
        fclose(file);
    } else {
        *opts = (cli_options_t){
            .lang = LANG_C,
            .show_version = false,
            .show_help = false,
            .cmd = CMD_NONE,
            .name = NULL,
            .implement_debug = false,
        };
    }

    return 0;
}

static int parse_long_arg(const char *arg, cli_options_t *opts) {
    if (strcmp(arg, "--c") == 0) {
        opts->lang = LANG_C;
    } else if (strcmp(arg, "--cpp") == 0) {
        opts->lang = LANG_CPP;
    } else if (strcmp(arg, "--version") == 0) {
        opts->show_version = true;
    } else if (strcmp(arg, "--help") == 0) {
        opts->show_help = true;
    } else if (strcmp(arg, "--debug") == 0) {
        opts->implement_debug = true;
    } else if (strcmp(arg, "--silent") == 0) {
        silent = true;
    } else if (strcmp(arg, "--no-color") == 0) {
        use_color = false;
    } else {
        error("unknown argument `%s`, "
              "for more info run `cinit --help`",
              arg);
        return -1;
    }

    return 0;
}

static int parse_short_arg(const char *arg, cli_options_t *opts) {
    for (int i = 1; arg[i]; ++i) {
        switch (arg[i]) {
        case 'v':
            opts->show_version = true;
            break;
        case 'h':
            opts->show_help = true;
            break;
        default:
            error("unknown argument `-%c`, "
                  "for more info run `cinit --help`",
                  arg[i]);
            return -1;
        }
    }

    return 0;
}

static int parse_arg(const char *arg, cli_options_t *opts) {
    if (strlen(arg) < 2) {
        error("unknown argument `%s`, "
              "for more info run `cinit --help`",
              arg);
        return -1;
    }

    if (arg[1] == '-') { // long arguments
        if (parse_long_arg(arg, opts) != 0) {
            return -1;
        }
    } else { // short arguments
        if (parse_short_arg(arg, opts) != 0) {
            return -1;
        }
    }

    return 0;
}

static int handle_create_command(int argc, char **argv, int *i,
                                 cli_options_t *opts) {
    if (*i + 1 >= argc || !is_correct_name(argv[*i + 1])) {
        error("incorrect usage, for help run `cinit --help`");
        return -1;
    }
    opts->name = argv[++(*i)];
    char *cur_dir = current_dir();
    if (!cur_dir) {
        perr("failed to get current path");
        return -1;
    }

    int path_len = strlen(cur_dir) + strlen(opts->name) + 2;
    opts->path = (char *)malloc(path_len + 1);
    if (!opts->path) {
        perr("failed to allocate memory for path");
        free(cur_dir);
        return -1;
    }
    snprintf(opts->path, path_len, "%s%c%s", cur_dir, PATH_SEPARATOR,
             opts->name);
    free(cur_dir);

    return 0;
}

static int handle_init_command(int argc, char **argv, int *i,
                               cli_options_t *opts) {
    opts->path = current_dir();
    if (!opts->path) {
        perr("failed to get current path");
        return -1;
    }

    if (*i + 1 < argc && is_correct_name(argv[*i + 1])) {
        opts->name = argv[++(*i)];
    } else {
        warning("no name provided, using the name of the directory");
        char *last_sep = strrchr(opts->path, PATH_SEPARATOR);
        opts->name =
            (last_sep && *(last_sep + 1) != '\0') ? last_sep + 1 : opts->path;
    }

    return 0;
}

static int handle_command(const char *arg, int argc, char **argv, int *i,
                          cli_options_t *opts) {
    if (strcmp(arg, "create") == 0 || strcmp(arg, "c") == 0) {
        return handle_create_command(argc, argv, i, opts);
    } else if (strcmp(arg, "init") == 0 || strcmp(arg, "i") == 0) {
        return handle_init_command(argc, argv, i, opts);
    } else {
        error("unknown command `%s`, for more info run `cinit --help`", arg);
        return -1;
    }
}

int parse_cli(int argc, char **argv, cli_options_t *opts) {
    if (opts_default(opts) != 0) {
        return -1;
    }
    if (argc < 2) {
        error("incorrect usage, for help run `cinit --help`");
        return -1;
    }

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (arg[0] == '-') { // arguments
            if (parse_arg(arg, opts) != 0) {
                return -1;
            }
        } else { // commands
            if (handle_command(arg, argc, argv, &i, opts) != 0) {
                return -1;
            }
        }
    }

    if (!opts->name && !opts->show_version && !opts->show_help) {
        error("incorrect usage, for help run `cinit --help`");
        return -1;
    }

    return 0;
}

void opts_free(cli_options_t *opts) {
    free(opts->path);
    opts->path = NULL;
}

bool is_correct_name(const char *str) {
    if (!isalnum(str[0])) {
        return false;
    }
    if (strlen(str) > 32) {
        return false;
    }
    for (int i = 0; str[i]; i++) {
        if (!(isalnum(str[i]) || str[i] == '_')) {
            return false;
        }
    }
    return true;
}
