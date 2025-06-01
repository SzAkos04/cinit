#include "project.h"

#include "asprintf.h"
#include "cli.h"
#include "debug.h"
#include "fs_utils.h"
#include "template_files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// MUST BE FREED
static int define_relative(const cli_options_t *opts, char **relative) {
    char *cur_dir = current_dir();
    if (!cur_dir) {
        perr("failed to get current path");
        return -1;
    }

    *relative = NULL;
    if (asprintf(relative, ".%c", PATH_SEPARATOR) == -1) {
        perr("failed to allocate memory for relative path");
        free(cur_dir);
        return -1;
    }

    // create the project folder if it doesn't already exist
    if (strcmp(opts->path, cur_dir) != 0) {
        if (create_dir(opts->path) != 0) {
            perr("failed to create `%s` folder", opts->path);
            free(*relative);
            free(cur_dir);
            return -1;
        }
        char *tmp = strdup(*relative);
        free(*relative);
        if (asprintf(relative, "%s%s", tmp, opts->name) == -1) {
            perr("failed to allocate memory for relative path");
            free(tmp);
            free(cur_dir);
            return -1;
        }
        free(tmp);
    }
    free(cur_dir);

    return 0;
}

static int create_dirs(const char *relative) {
    char *path = NULL;

    // create `src/`
    if (asprintf(&path, "%s%csrc", relative, PATH_SEPARATOR) == -1) {
        perr("failed to allocate memory for `src` path");
        return -1;
    }
    if (create_dir(path) != 0) {
        perr("failed to create `%s` folder", path + 2);
        free(path);
        return -1;
    }
    free(path);

    // create `include/`
    if (asprintf(&path, "%s%cinclude", relative, PATH_SEPARATOR) == -1) {
        perr("failed to allocate memory for `include` path");
        return -1;
    }
    if (create_dir(path) != 0) {
        perr("failed to create `%s` folder", path + 2);
        free(path);
        return -1;
    }
    free(path);

    return 0;
}

static int create_files(const cli_options_t *opts, char *relative) {
    char *path = NULL;

    // create `Makefile`
    if (asprintf(&path, "%s%c%s", relative, PATH_SEPARATOR, "Makefile") == -1) {
        perr("failed to allocate memory for path");
        return -1;
    }
    char *makefile = generate_makefile(opts->name, opts->lang);
    if (!makefile) {
        free(path);
        return -1;
    }
    if (write_file(path, makefile) != 0) {
        perr("failed to write to `%s`", strchr(path, PATH_SEPARATOR) + 1);
        free(makefile);
        free(path);
        return -1;
    }
    free(makefile);
    free(path);

    // create `main.c(pp)`
    if (asprintf(&path, "%s%c%s", relative, PATH_SEPARATOR,
                 (opts->lang == LANG_C) ? "src/main.c" : "src/main.cpp") ==
        -1) {
        perr("failed to allocate memory for path");
        return -1;
    }
    if (write_file(path, (opts->lang == LANG_C) ? main_c() : main_cpp()) != 0) {
        perr("failed to write to `%s`", strchr(path, PATH_SEPARATOR) + 1);
        free(path);
        return -1;
    }
    free(path);

    // create `compile_flags.txt`
    if (asprintf(&path, "%s%c%s", relative, PATH_SEPARATOR,
                 "compile_flags.txt") == -1) {
        perr("failed to allocate memory for path");
        return -1;
    }
    if (write_file(path, compile_flags()) != 0) {
        perr("failed to write to `%s`", strchr(path, PATH_SEPARATOR) + 1);
        free(path);
        return -1;
    }
    free(path);

    if (opts->implement_debug) {
        char *src = (opts->lang == LANG_C ? debug_h(opts->name)
                                          : debug_hpp(opts->name));
        if (asprintf(&path, "%s%c%s", relative, PATH_SEPARATOR,
                     (opts->lang == LANG_C ? "include/debug.h"
                                           : "include/debug.hpp")) == -1) {
            perr("failed to allocate memory for path");
            return -1;
        }

        if (write_file(path, src) != 0) {
            perr("failed to write to `%s`", strchr(path, PATH_SEPARATOR) + 1);
            free(src);
            free(path);
            return -1;
        }
        free(path);
        free(src);
        info("make sure to define `use_color` to enable output coloring "
             "and `silent` to silence output in your source");
    }

    return 0;
}

int project_generate(const cli_options_t *opts) {
    clock_t start = clock();

    info("Generating files for `%s%s%s`", BOLD, opts->name, RESET);

    // get current path
    char *relative;
    if (define_relative(opts, &relative) != 0) {
        return -1;
    }

    if (create_dirs(relative) != 0) {
        free(relative);
        return -1;
    }

    if (create_files(opts, relative) != 0) {
        free(relative);
        return -1;
    }

    free(relative);

    clock_t end = clock();
    double time_taken = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

    success("Generated %sproject%s `%s%s%s` in %s%.2fms%s", DIM, RESET, BOLD,
            opts->name, RESET, CYAN, time_taken, RESET);
    return 0;
}

#ifndef VERSION
#define VERSION "UNKNOWN"
#endif
#ifndef BUILD_DATE
#define BUILD_DATE "UNKNOWN"
#endif

void version(void) {
    printf("%scinit v0.0.6%s\n"
           "%sGit commit%s: %s\n"
           "%sBuilt on  %s: %s\n"
           // TODO: update release date before publishing
           "%sRelease   %s: 31/05/2025\n",
           BOLD, RESET, 
           BOLD, RESET, VERSION,
           BOLD, RESET, BUILD_DATE);
}

void help(void) {
    printf("%s%sUsage:%s\n"
           "  cinit [create|c] [name] <args>\n"
           "  cinit [init|i] ([name]) <args>\n"
           "  cinit --help\n"
           "\n"
           "%s%sOptions:%s\n"
           "  init|i    Create a new project in the current directory\n"
           "  create|c  Create a new project in the given directory\n"
           "\n"
           "%s%sArguments:%s\n"
           "  name       The name of the project\n"
           "  --c        Set language to C (default)\n"
           "  --cpp      Set language to C++\n"
           "  --help     Show this message\n"
           "  --version  Show installed version\n"
           "  --silent   Disable output (except errors)\n"
           "  --no-color Disable output coloring\n"
           "\n"
           "%s%sExamples:%s\n"
           "  %scinit init my_project%s\n"
           "    Initializes a new project in the current directory\n"
           "  %scinit create my_project%s\n"
           "    Creates a new project called `my_project`\n"
           "  %scinit create my_project --cpp%s\n"
           "    Creates a new C++ project called `my_project`\n"
           "\n"
           "%s%sNotes:%s\n"
           "  - Project names must be alphanumeric or underscores only\n"
           "  - Maximum length: 32 characters\n"
           "\n"
           "%s%sFor more information, visit:%s\n"
           "  %shttps://github.com/SzAkos04/cinit%s\n",
           BOLD, UNDERLINE, RESET,
           BOLD, UNDERLINE, RESET,
           BOLD, UNDERLINE, RESET,
           BOLD, UNDERLINE, RESET,
           BOLD, RESET,
           BOLD, RESET,
           BOLD, RESET
           BOLD, UNDERLINE, RESET
           BOLD, UNDERLINE, RESET);
}
