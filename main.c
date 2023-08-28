#include <stdio.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

/* Ignore the unused main parameters*/
#pragma GCC diagnostic ignored "-Wunused-parameter"

int main(int argc, const char* argv[]) {
    initVM();

    Chunk chunk;
    initChunk(&chunk);

    writeConstant(&chunk, 1.2, 123);
    /* for (int i = 0; i < 300; ++i) {
        writeConstant(&chunk, 3.3 + (Value)i, 124);
    }*/
    writeChunk(&chunk, OP_RETURN, 123);

    disassembleChunk(&chunk, "test chunk");
    printf("== test chunk (run) ==\n");
    interpret(&chunk);

    freeVM();
    freeChunk(&chunk);

    return 0;
}
