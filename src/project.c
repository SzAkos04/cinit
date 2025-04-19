#include "project.h"

#include "asprintf.h"
#include "debug.h"
#include "fs_utils.h"
#include "template_files.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int project_generate(project_t *self) {
    log("generating files for `%s%s%s`", BOLD, self->name, RESET);

    // get current path
    char *cur_dir = current_dir();
    if (!cur_dir) {
        perr("failed to get current path");
        return 1;
    }

    /* char relative[64]; */
    char *relative = NULL;
    /* snprintf(relative, sizeof(relative), ".%c", PATH_SEPARATOR); */
    if (asprintf(&relative, ".%c", PATH_SEPARATOR) == -1) {
        perr("failed to allocate memory for relative path");
        return 1;
    }

    // create the project folder if it doesn't already exist
    if (strcmp(self->path, cur_dir) != 0) {
        if (create_dir(self->path) != 0) {
            perr("failed to create project folder");
            return 1;
        }
        /* strcat(relative, self->name); */
        char *tmp = relative;
        free(relative);
        if (asprintf(&relative, "%s%s", tmp, self->name) == -1) {
            perr("failed to allocate memory for relative path");
            return 1;
        }
    }
    free(cur_dir);

    /* char path[128]; */
    char *path = NULL;

    /* snprintf(path, sizeof(path), "%s%c%s", relative, PATH_SEPARATOR, "src");
     */
    if (asprintf(&path, "%s%c%s", relative, PATH_SEPARATOR, "src") == -1) {
        perr("failed to allocate memory for path");
        return 1;
    }
    if (create_dir(path) != 0) {
        perr("failed to create `src/` folder");
        return 1;
    }
    free(path);

    /* snprintf(path, sizeof(path), "%s%c%s", relative, PATH_SEPARATOR,
     * "include"); */
    if (asprintf(&path, "%s%c%s", relative, PATH_SEPARATOR, "include") == -1) {
        perr("failed to allocate memory for path");
        return 1;
    }
    if (create_dir(path) != 0) {
        perr("failed to create `include/` folder");
        return 1;
    }
    free(path);

    /* snprintf(path, sizeof(path), "%s%c%s", relative, PATH_SEPARATOR, */
    /*          "Makefile"); */
    if (asprintf(&path, "%s%c%s", relative, PATH_SEPARATOR, "Makefile") == -1) {
        perr("failed to allocate memory for path");
        return 1;
    }
    char *makefile = (self->flags & FLAG_C) ? makefile_c(self->name)
                                            : makefile_cpp(self->name);
    if (!makefile) {
        return 1;
    }
    if (write_file(path, makefile) != 0) {
        return 1;
    }
    free(makefile);
    free(path);

    /* snprintf(path, sizeof(path), "%s%c%s", relative, PATH_SEPARATOR, */
    /*          (self->flags & FLAG_C) ? "src/main.c" : "src/main.cpp"); */
    if (asprintf(&path, "%s%c%s", relative, PATH_SEPARATOR,
                 (self->flags & FLAG_C) ? "src/main.c" : "src/main.cpp") ==
        -1) {
        perr("failed to allocate memory for path");
        return 1;
    }
    if (write_file(path, (self->flags & FLAG_C) ? main_c() : main_cpp()) != 0) {
        return 1;
    }
    free(path);

    /* snprintf(path, sizeof(path), "%s%c%s", relative, PATH_SEPARATOR, */
    /*          "compile_flags.txt"); */
    if (asprintf(&path, "%s%c%s", relative, PATH_SEPARATOR,
                 "compile_flags.txt") == -1) {
        perr("failed to allocate memory for path");
        return 1;
    }
    if (write_file(path, compile_flags()) != 0) {
        return 1;
    }
    free(path);
    free(relative);

    success("Successfully generated project files for `%s%s%s`", BOLD,
            self->name, RESET);
    return 0;
}

project_t *project_new(void) {
    project_t *project = (project_t *)malloc(sizeof(project_t));

    project->flags = FLAG_C; // set C as default
    project->generate = project_generate;

    project->name = NULL;
    project->path = NULL;

    return project;
}

void project_free(project_t *project) {
    free(project->path);
    free(project);
}

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

void version(void) {
    printf("%scinit v0.0.2%s\n", BOLD, RESET);
    printf("%sGit commit%s: %s\n", BOLD, RESET, VERSION);
    printf("%sBuilt on  %s: %s\n", BOLD, RESET, BUILD_DATE);
    // TODO: update release date before publishing
    printf("%sRelease   %s: <TBD>, 2025\n", BOLD, RESET);
}

void help(void) {
    printf("%s%sUsage:%s\n", BOLD, UNDERLINED, RESET);
    printf("  cinit [create|c] [name] <args>\n");
    printf("  cinit [init|i] ([name]) <args>\n");
    printf("  cinit --help\n");
    printf("\n");
    printf("%s%sOptions:%s\n", BOLD, UNDERLINED, RESET);
    printf("  init|i    Create a new project in the current directory\n");
    printf("  create|c  Create a new project in the given directory\n");
    printf("\n");
    printf("%s%sArguments:%s\n", BOLD, UNDERLINED, RESET);
    printf("  name      The name of the project\n");
    printf("  --c       Set language to C (default)\n");
    printf("  --cpp     Set language to C++\n");
    printf("  --help    Show this message\n");
    printf("  --version Show installed version\n");
    printf("\n");
    printf("%s%sExamples:%s\n", BOLD, UNDERLINED, RESET);
    printf("  %scinit init my_project%s\n", BOLD, RESET);
    printf("    Initializes a new project in the current directory\n");
    printf("  %scinit create my_project%s\n", BOLD, RESET);
    printf("    Creates a new project called `my_project`\n");
    printf("  %scinit create my_project --cpp%s\n", BOLD, RESET);
    printf("    Creates a new C++ project called `my_project`\n");
    printf("\n");
    printf("%s%sNotes:%s\n", BOLD, UNDERLINED, RESET);
    printf("  - Project names must be alphanumeric or underscores only\n");
    printf("  - Maximum length: 32 characters\n");
    printf("\n");
    printf("%s%sFor more information, visit:%s\n", BOLD, UNDERLINED, RESET);
    printf("  %shttps://github.com/SzAkos04/cinit%s\n", BOLD, RESET);
}
