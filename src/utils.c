
#include <stdlib.h>
#include "utils.h"

void *utils_malloc(size_t size)
{
    void *result = malloc(size);
    if (result == NULL)
    {
        printf("failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    return result;
}

void *utils_realloc(void *p, size_t size)
{
    void *result = realloc(p, size);
    if (result == NULL)
    {
        printf("failed to reallocate memory");
        exit(EXIT_FAILURE);
    }
    return result;
}

char *utils_getFileContent(char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("failed to open file: %s", path);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    char *content = utils_malloc((len + 1) * sizeof(char));

    fseek(file, 0, SEEK_SET);
    fread(content, sizeof(char), len, file);
    fclose(file);

    content[len] = '\0';
    return content;
}