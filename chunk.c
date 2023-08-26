#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->linesCount = 0;
    chunk->linesCapacity = 0;
    chunk->lines = NULL;
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(unsigned, chunk->lines, chunk->capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void addLineData(Chunk *chunk, int8_t ld) {
    if (chunk->linesCapacity < chunk->linesCount + 1) {
        size_t oldCapacity = chunk->linesCapacity;
        chunk->linesCapacity = GROW_CAPACITY(oldCapacity);
        chunk->lines = GROW_ARRAY(int8_t, chunk->lines, oldCapacity, chunk->linesCapacity);
    }
    chunk->lines[chunk->linesCount++] = ld;
}

void addLine(Chunk *chunk, uint32_t line) {
    if (chunk->linesCount > 0) {
        uint32_t currentLine = getLine(chunk, chunk->count - 1);
        if (line == currentLine) {
            chunk->lines[chunk->linesCount - 1]--;
            return;
        }
        if (line == currentLine + 1) {
            addLineData(chunk, -1);
            return;
        }
    }
    if (line != 0 || chunk->linesCount > 0) {
        for (uint8_t i = 0; i < 4; ++i) {
            addLineData(chunk, line & 0x7f);
            line = line >> 7;
            if (line == 0) break;
        }
    }
    addLineData(chunk, -1);
}

void writeChunk(Chunk* chunk, uint8_t byte, uint32_t line) {
    if (chunk->capacity < chunk->count + 1) {
        size_t oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    addLine(chunk, line);
    chunk->count++;
}

uint8_t addConstant(Chunk *chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

uint32_t getLine(Chunk *chunk, size_t offset) {
    uint32_t line = 0;
    size_t lc = chunk->linesCount;
    size_t seenOfs = 0;
    for (size_t ln = 0; ln < lc; ++ln) {
        if (chunk->lines[ln] >= 0) {
            line = 0;
            for (uint8_t i = 0; ln < lc && i < 4 && chunk->lines[ln] >= 0; ++i, ++ln) {
                line += (uint32_t)(chunk->lines[ln]) << (7 * i);
            }
        }
        if (chunk->lines[ln] < 0) {
            seenOfs += (size_t)(-chunk->lines[ln]);
        }
        if (seenOfs > offset) break;
        line++;
    }
    return line;
}
