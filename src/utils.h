#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

void *utils_malloc(size_t size);

void *utils_realloc(void *p, size_t size);

char *utils_getFileContent(char *path);

#endif