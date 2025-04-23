#pragma once

#include <stdbool.h>

typedef enum { LANG_C, LANG_CPP } lang_t;
typedef enum { CMD_CREATE, CMD_INIT, CMD_NONE } cmd_t;

typedef struct {
    lang_t lang;
    bool show_version;
    bool show_help;
    cmd_t cmd;
    const char *name;
    char *path;
} cli_options_t;

cli_options_t opts_default(void);
int parse_cli(int argc, char **argv, cli_options_t *opts);

void opts_free(cli_options_t *);

bool is_correct_name(const char *str);
