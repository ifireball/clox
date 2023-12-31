#include <stdlib.h>

#include "memory.h"

/* Ignore the unused oldSize parameter */
#pragma GCC diagnostic ignored "-Wunused-parameter"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    void* result = realloc(pointer, newSize);
    if (result == NULL) exit(1);
    return result;
}
