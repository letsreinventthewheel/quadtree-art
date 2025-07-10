#pragma once

#include <stddef.h>

#include "quad.h"

typedef struct {
    Quad *quad;
    float score;
} HeapNode;

typedef struct {
    HeapNode *data;
    size_t length;
} Heap;

void heap_init(Heap *heap);
void heap_push(Heap *heap, Quad *quad);
Quad* heap_pop(Heap *heap);
