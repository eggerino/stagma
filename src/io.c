#include "io.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

char* io_read_entire_file(const char* file) {
    FILE* f = fopen(file, "r");
    assert(f && "Could not open file");

    // Get content size
    fseek(f, 0L, SEEK_END);
    long content_size = ftell(f);
    rewind(f);

    // Create buffer
    long buffer_size = content_size + 1;
    char* buffer = (char*)malloc(buffer_size);
    assert(buffer && "Could not create enough space to read file");

    // Read into buffer as a c string (null terminated)
    fread(buffer, content_size, 1, f);
    buffer[buffer_size - 1] = '\0';

    fclose(f);

    return buffer;
}
