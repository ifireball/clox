#include <stdio.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"

/* Ignore the unused main parameters*/
#pragma GCC diagnostic ignored "-Wunused-parameter"

int main(int argc, const char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);

    writeConstant(&chunk, 1.2, 123);
    writeChunk(&chunk, OP_RETURN, 123);

    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
    return 0;
}
