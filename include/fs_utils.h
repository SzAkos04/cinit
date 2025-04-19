#pragma once

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

char *current_dir(void);

int create_dir(const char *path);

int write_file(const char *path, const char *contents);
