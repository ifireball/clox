#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "vm.h"

VM vm;

static void resetStack() {
    vm.stack = NULL;
    vm.stackTop = NULL;
    vm.stackCount = 0;
    vm.stackCapacity = 0;
}

static void freeStack() {
    FREE_ARRAY(Value, vm.stack, vm.stackCapacity);
    resetStack();
}

void initVM() {
    resetStack();
}

void freeVM() {
    freeStack();
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
        double b = pop(); \
        double a = pop(); \
        push(a op b); \
    } while(false)

    for(;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (size_t)(vm.ip - vm.chunk->code));
#endif

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_CONSTANT_LONG: {
                size_t idx = READ_BYTE();
                idx += READ_BYTE() << 8;
                idx += READ_BYTE() << 16;
                Value constant = vm.chunk->constants.values[idx];
                push(constant);
                break;
            }
            case OP_ADD:      BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE:   BINARY_OP(/); break;
            case OP_NEGATE:   push(-pop()); break;
            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

InterpretResult interpret(const char* source)
{
    Chunk chunk;
    initChunk(&chunk);

    if(!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}

void push(Value value) {
    if (vm.stackCount >= vm.stackCapacity) {
        size_t oldCapacity = vm.stackCapacity;
        vm.stackCapacity = GROW_CAPACITY(vm.stackCapacity);
        vm.stack = GROW_ARRAY(Value, vm.stack, oldCapacity, vm.stackCapacity);
        vm.stackTop = &vm.stack[vm.stackCount];
    }
    *vm.stackTop = value;
    vm.stackTop++;
    vm.stackCount++;
}

Value pop() {
    vm.stackTop--;
    vm.stackCount--;
    return *vm.stackTop;
}
