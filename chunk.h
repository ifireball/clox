#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_NEGATE,
    OP_RETURN,
} OpCode;

typedef struct {
    size_t count;
    size_t capacity;
    uint8_t* code;
    size_t linesCount;
    size_t linesCapacity;
    int8_t* lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, uint32_t line);
size_t addConstant(Chunk* chunk, Value value);
void writeConstant(Chunk* chunk, Value value, uint32_t line);
uint32_t getLine(Chunk* chunk, size_t offset);

#endif
