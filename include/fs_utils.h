#pragma once

#include <stddef.h>

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

#define FILE_NOT_FOUND ((char *)-1)

char *current_dir(void);

int create_dir(const char *path);

char *read_file(const char *filename, size_t *file_size);
int write_file(const char *path, const char *contents);
