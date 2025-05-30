#include "cinitrc.h"

#include "cli.h"
#include "debug.h"
#include "fs_utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper: Trim leading and trailing whitespace
static char *trim(char *str) {
    while (isspace((unsigned char)*str)) {
        str++;
    }
    if (*str == 0) {
        return str;
    }

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';
    return str;
}

// On success returns 0
// When file doesn't exist returns 1
// On failure returns -1
int parse_cinitrc(const char *path, cli_options_t *opts) {
    size_t len;
    char *buf = read_file(path, &len);
    if (!buf) {
        return -1;
    } else if (buf == FILE_NOT_FOUND) {
        return 1;
    }

    char *line = strtok(buf, "\n");
    while (line) {
        char *trimmed = trim(line);
        if (trimmed[0] == '\0' || trimmed[0] == '#') {
            line = strtok(NULL, "\n");
            continue;
        }

        char *eq = strchr(trimmed, '=');
        if (!eq) {
            debug("missing `=` in config");
            line = strtok(NULL, "\n");
            continue;
        }

        *eq = '\0';
        char *key = trim(trimmed);
        char *val = trim(eq + 1);

        if (strcmp(key, "lang") == 0) {
            if (strcmp(val, "c") == 0) {
                opts->lang = LANG_C;
            } else if (strcmp(val, "cpp") == 0) {
                opts->lang = LANG_CPP;
            } else {
                warning("unknown lang value `%s`, defaulting to `c`", val);
                opts->lang = LANG_C;
            }
        } else if (strcmp(key, "implement_debug") == 0) {
            opts->implement_debug = strcmp(val, "true") == 0;
        } else {
            warning("unknown config option `%s` in config", key);
        }

        line = strtok(NULL, "\n");
    }

    free(buf);
    return 0;
}
