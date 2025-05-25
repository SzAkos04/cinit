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
    printf("%scinit v0.0.5%s\n", BOLD, RESET);
    printf("%sGit commit%s: %s\n", BOLD, RESET, VERSION);
    printf("%sBuilt on  %s: %s\n", BOLD, RESET, BUILD_DATE);
    // TODO: update release date before publishing
    printf("%sRelease   %s: 05/05/2025\n", BOLD, RESET);
}

void help(void) {
    printf("%s%sUsage:%s\n", BOLD, UNDERLINE, RESET);
    printf("  cinit [create|c] [name] <args>\n");
    printf("  cinit [init|i] ([name]) <args>\n");
    printf("  cinit --help\n");
    printf("\n");
    printf("%s%sOptions:%s\n", BOLD, UNDERLINE, RESET);
    printf("  init|i    Create a new project in the current directory\n");
    printf("  create|c  Create a new project in the given directory\n");
    printf("\n");
    printf("%s%sArguments:%s\n", BOLD, UNDERLINE, RESET);
    printf("  name      The name of the project\n");
    printf("  --c       Set language to C (default)\n");
    printf("  --cpp     Set language to C++\n");
    printf("  --help    Show this message\n");
    printf("  --version Show installed version\n");
    printf("\n");
    printf("%s%sExamples:%s\n", BOLD, UNDERLINE, RESET);
    printf("  %scinit init my_project%s\n", BOLD, RESET);
    printf("    Initializes a new project in the current directory\n");
    printf("  %scinit create my_project%s\n", BOLD, RESET);
    printf("    Creates a new project called `my_project`\n");
    printf("  %scinit create my_project --cpp%s\n", BOLD, RESET);
    printf("    Creates a new C++ project called `my_project`\n");
    printf("\n");
    printf("%s%sNotes:%s\n", BOLD, UNDERLINE, RESET);
    printf("  - Project names must be alphanumeric or underscores only\n");
    printf("  - Maximum length: 32 characters\n");
    printf("\n");
    printf("%s%sFor more information, visit:%s\n", BOLD, UNDERLINE, RESET);
    printf("  %shttps://github.com/SzAkos04/cinit%s\n", BOLD, RESET);
}
