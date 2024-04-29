#include "project.h"

#include "debug.h"
#include "files.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

// on success returns 0, 1 otherwise
static char *current_dir(void) {
    char cwd[512];
#ifdef _WIN32
    if (GetCurrentDirectory(sizeof(cwd), cwd) != 0) {
        return cwd;
    }
    return NULL;
#else
    if (getcwd(cwd, sizeof(cwd))) {
        return strdup(cwd);
    }
    return NULL;
#endif
}

// on success returns 0, 1 otherwise
static int create_directory(const char *path) {
#ifdef _WIN32
    return CreateDirectory(path, NULL) == 0;
#else
    return mkdir(path, 0777) != 0;
#endif
}

static int write_file(const char *path, const char *contents) {
    FILE *outfile = fopen(path, "w");
    if (!outfile) {
        perr("failed to open file");
        return 1;
    }
    fprintf(outfile, "%s", contents);
    fclose(outfile);

    return 0;
}

static int create(Project *self) {
    char path[512];

    char relative_path[512] = "./";
    switch (self->command) {
    case CMD_CREATE:
        strncat(relative_path, self->name,
                sizeof(relative_path) - strlen(relative_path) - 1);
        if (create_directory(relative_path) != 0) {
            perr("failed to create directory");
            return 1;
        }
        break;
    case CMD_INIT:
        if (!self->name) {
            warn("no name provided, using the name of the directory");
            char *path = current_dir();
            info(path);
            if (!path) {
                perr("failed to get current directory");
                return 1;
            }
            self->name = strrchr(path, '/');
            self->name = self->name ? self->name + 1 : path;
        }
        break;
    default:
        error("no command provided, for more info run `cinit --help`");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/src", relative_path);
    path[sizeof(path) - 1] = '\0'; // make sure it is null terminated
    if (create_directory(path) != 0) {
        perr("failed to create directory");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/include", relative_path);
    path[sizeof(path) - 1] = '\0'; // make sure it is null terminated
    if (create_directory(path) != 0) {
        perr("failed to create directory");
        return 1;
    }

    if ((self->flags & FLAG_C)) {
        snprintf(path, sizeof(path), "%s/src/main.c", relative_path);
        path[sizeof(path) - 1] = '\0'; // make sure it is null terminated
        if (write_file(path, MAIN_C) != 0) {
            return 1;
        }

        snprintf(path, sizeof(path), "%s/Makefile", relative_path);
        path[sizeof(path) - 1] = '\0'; // make sure it is null terminated
        if (write_file(path, MAKEFILE_C(self->name)) != 0) {
            return 1;
        }
    } else if ((self->flags & FLAG_CPP)) {
        snprintf(path, sizeof(path), "%s/src/main.cpp", relative_path);
        path[sizeof(path) - 1] = '\0'; // make sure it is null terminated
        if (write_file(path, MAIN_CPP) != 0) {
            return 1;
        }

        snprintf(path, sizeof(path), "%s/Makefile", relative_path);
        path[sizeof(path) - 1] = '\0'; // make sure it is null terminated
        if (write_file(path, MAKEFILE_CPP(self->name)) != 0) {
            return 1;
        }
    }
    snprintf(path, sizeof(path), "%s/compile_flags.txt", relative_path);
    path[sizeof(path) - 1] = '\0'; // make sure it is null terminated
    if (write_file(path, COMPILE_FLAGS) != 0) {
        return 1;
    }

    return 0;
}

Project project_new(CMD command, unsigned int flags, char *name) {
    Project project = (Project){
        .command = command,
        .flags = flags,
        .name = name,
        .create = create,
    };

    return project;
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
           "characters and underscores\n");
    printf("  - If no language is specified, the default language is assumed "
           "to be C\n");
    printf("\n");
    printf("For more information, visit:\n");
    printf("  https://github.com/SzAkos04/cinit\n");
}