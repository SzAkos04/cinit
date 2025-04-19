#include "project.h"

#include "debug.h"
#include "fs_utils.h"
#include "template_files.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int project_generate(project_t *self) {
    log("generating files for `%s%s%s`", BOLD, self->name, RESET);

    log("%s", BUILD_DATE);

    // get current path
    const char *cur_dir = current_dir();
    if (!cur_dir) {
        perr("failed to get current path");
        return 1;
    }

    char relative[64];
    snprintf(relative, sizeof(relative), ".%c", PATH_SEPARATOR);

    // create the project folder if it doesn't already exist
    if (strcmp(self->path, cur_dir) != 0) {
        if (create_dir(self->path) != 0) {
            perr("failed to create project folder");
            return 1;
        }
        strcat(relative, self->name);
    }

    char path[128];

    snprintf(path, sizeof(path), "%s%c%s", relative, PATH_SEPARATOR, "src");
    if (create_dir(path) != 0) {
        perr("failed to create `src/` folder");
        return 1;
    }

    snprintf(path, sizeof(path), "%s%c%s", relative, PATH_SEPARATOR, "include");
    if (create_dir(path) != 0) {
        perr("failed to create `include/` folder");
        return 1;
    }

    snprintf(path, sizeof(path), "%s%c%s", relative, PATH_SEPARATOR,
             "Makefile");
    char *makefile = (self->flags & FLAG_C) ? makefile_c(self->name)
                                            : makefile_cpp(self->name);
    if (!makefile) {
        return 1;
    }
    if (write_file(path, makefile) != 0) {
        return 1;
    }
    free(makefile);

    snprintf(path, sizeof(path), "%s%c%s", relative, PATH_SEPARATOR,
             (self->flags & FLAG_C) ? "src/main.c" : "src/main.cpp");
    if (write_file(path, (self->flags & FLAG_C) ? main_c() : main_cpp()) != 0) {
        return 1;
    }

    snprintf(path, sizeof(path), "%s%c%s", relative, PATH_SEPARATOR,
             "compile_flags.txt");
    if (write_file(path, compile_flags()) != 0) {
        return 1;
    }

    return 0;
}

project_t *project_new(void) {
    project_t *project = (project_t *)malloc(sizeof(project_t));

    project->flags = FLAG_C; // set C as default
    project->generate = project_generate;

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
    printf("cinit version 0.0.2\n");
    // TODO: fix date on release
    printf("Released on xxx, 2025\n");
}

void help(void) {
    printf("Usage:\n");
    printf("  cinit [create|c] [name] <args>\n");
    printf("  cinit [init|i] ([name]) <args>\n");
    printf("  cinit --help\n");
    printf("\n");
    printf("Options:\n");
    printf("  init|i    Create a new project in the current directory\n");
    printf("  create|c  Create a new project in the given directory\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  name      The name of the project\n");
    printf("  --c       Create a C project\n");
    printf("  --cpp     Create a C++ project\n");
    printf("  --help    Show this message\n");
    printf("\n");
    printf("Examples:\n");
    printf("  cinit init my_project\n");
    printf("    Initializes a new project in the current directory\n");
    printf("  cinit create my_project\n");
    printf("    Creates a new project called `my_project`\n");
    printf("  cinit create my_project --cpp\n");
    printf("    Creates a new C++ project called `my_project`\n");
    printf("\n");
    printf("Notes:\n");
    printf("  - The project name should consist only of alphanumeric "
           "characters and underscores, max length is 32 characters\n");
    printf("  - If no language is specified, the default language is assumed "
           "to be C\n");
    printf("\n");
    printf("For more information, visit:\n");
    printf("  https://github.com/SzAkos04/cinit\n");
}
