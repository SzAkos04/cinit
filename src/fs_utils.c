#include "fs_utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

char *current_dir(void) {
    char cwd[512];
#ifdef _WIN32
    if (GetCurrentDirectory(sizeof(cwd), cwd) != 0) {
        return strdup(cwd);
    }
    return NULL;
#else
    if (getcwd(cwd, sizeof(cwd))) {
        return strdup(cwd);
    }
    return NULL;
#endif
}

// on success returns 0, on failure returns 1
int create_dir(const char *path) {
#ifdef _WIN32
    return CreateDirectory(path, NULL) ? 0 : 1;
#else
    return mkdir(path, 0777) == 0 ? 0 : 1;
#endif
}

// MUST BE FREED
char *read_file(const char *filename, size_t *file_size) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(*file_size + 1);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, *file_size, file);
    if (bytesRead != *file_size) {
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[*file_size] = '\0';

    fclose(file);
    return buffer;
}

int write_file(const char *path, const char *contents) {
    FILE *outfile = fopen(path, "w");
    if (!outfile) {
        return 1;
    }
    fputs(contents, outfile);
    fclose(outfile);

    return 0;
}
