#include <math.h>

#include "heap.h"
#include "stb_ds.h"

static void heap_swap(Heap *heap, size_t a, size_t b) {
    HeapNode tmp = heap->data[a];
    heap->data[a] = heap->data[b];
    heap->data[b] = tmp;
}

static void heapify_up(Heap *heap, size_t index) {
    if (index == 0) {
        return;
    }

    size_t parent = (index - 1) / 2;
    if (heap->data[parent].score > heap->data[index].score) {
        heap_swap(heap, index, parent);
        heapify_up(heap, parent);
    }
}

static void heapify_down(Heap *heap, size_t index) {
    size_t left = index * 2 + 1;
    size_t right = index * 2 + 2;

    if (left >= heap->length) {
        return;
    }

    size_t smallest = index;
    if (heap->data[left].score < heap->data[index].score) {
        smallest = left;
    }

    if (right < heap->length && heap->data[right].score < heap->data[smallest].score) {
        smallest = right;
    }

    if (smallest != index) {
        heap_swap(heap, index, smallest);
        heapify_down(heap, smallest);
    }
}

void heap_init(Heap *heap) {
    heap->data = nullptr;
    heap->length = 0;
}

void heap_push(Heap *heap, Quad *quad) {
    Box box = quad->boundary.box;
    bool is_leaf = (box.right - box.left <= 4) || (box.bottom - box.top <= 4);

    float score = -quad->average_color.error * pow(quad->boundary.area, 0.25) + (is_leaf ? 1000000 : 0);
    HeapNode node = (HeapNode) {
        .quad = quad,
        .score = score
    };

    arrpush(heap->data, node);
    heap->length += 1;
    heapify_up(heap, heap->length - 1);
}

Quad* heap_pop(Heap *heap) {
    heap_swap(heap, 0, heap->length - 1);
    heap->length--;
    HeapNode node = arrpop(heap->data);

    heapify_down(heap, 0);

    return node.quad;
}
