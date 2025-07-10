#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL3/SDL.h>
#include <stdlib.h>

#include "heap.h"
#include "quad.h"
#include "stb_image.h"
#include "stb_ds.h"

const uint32_t PADDING = 1;

typedef struct {
    uint32_t *data;
    uint32_t width;
    uint32_t height;
} Framebuffer;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    Framebuffer *framebuffer;
} SDLContext;

bool context_init(SDLContext *context, int window_width, int window_height, int image_width, int image_height) {
    context->framebuffer = malloc(sizeof(Framebuffer));
    if (!context->framebuffer) {
        fprintf(stderr, "Failed to malloc framebuffer\n");
        return false;
    }

    context->framebuffer->width = image_width;
    context->framebuffer->height = image_height;

    context->framebuffer->data = malloc(sizeof(uint32_t) * image_height * image_width);
    if (!context->framebuffer->data) {
        fprintf(stderr, "Failed to malloc framebuffer data\n");
        return false;
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        fprintf(stderr, "SDL Init failed: %s\n", SDL_GetError());
        return false;
    }

    context->window = SDL_CreateWindow("Quadtree art", window_width, window_height, 0);
    if (!context->window) {
        fprintf(stderr, "SDL create window failed: %s\n", SDL_GetError());
        return false;
    }

    context->renderer = SDL_CreateRenderer(context->window, nullptr);
    if (!context->renderer) {
        fprintf(stderr, "SDL create renderer failed: %s\n", SDL_GetError());
        return false;
    }

    context->texture = SDL_CreateTexture(context->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, image_width, image_height);
    if (!context->texture) {
        fprintf(stderr, "SDL create texture failed: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void context_deinit(SDLContext *context) {
    if (context->texture) {
        SDL_DestroyTexture(context->texture);
    }
    if (context->renderer) {
        SDL_DestroyRenderer(context->renderer);
    }
    if (context->window) {
        SDL_DestroyWindow(context->window);
    }

    SDL_Quit();
}

void draw_rectangle(Framebuffer *framebuffer, uint32_t left, uint32_t top, uint32_t width, uint32_t height, uint32_t color) {
    for (size_t row = top; row < top + height; row++) {
        for (size_t column = left; column < left + width; column++) {
            framebuffer->data[row * framebuffer->width + column] = color;
        }
    }
}

void draw_image(const SDLContext *context, Heap *heap) {
    // clear framebuffer with black
    draw_rectangle(context->framebuffer, 0, 0, context->framebuffer->width, context->framebuffer->height, 0xFF000000);

    for (size_t i = 0; i < heap->length; i++) {
        const Quad *quad = heap->data[i].quad;

        Box box = quad->boundary.box;
        uint32_t color = (0xFF << 24) | (quad->average_color.color.red << 16) | (quad->average_color.color.green << 8) | (quad->average_color.color.blue);
        draw_rectangle(
            context->framebuffer,
            box.left + PADDING,
            box.top + PADDING,
            box.right - box.left - PADDING,
            box.bottom - box.top - PADDING,
            color
        );
    }

    SDL_UpdateTexture(context->texture, nullptr, context->framebuffer->data, sizeof(uint32_t) * context->framebuffer->width);
    SDL_RenderTexture(context->renderer, context->texture, nullptr, nullptr);
    SDL_RenderPresent(context->renderer);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stdout, "Usage: %s <image>\n", argv[0]);
        return 0;
    }

    Image image;
    image.data = stbi_load(argv[1], &image.width, &image.height, nullptr, 3);
    if (!image.data) {
        fprintf(stderr, "Failed to load image %s\n", argv[1]);
        return -1;
    }

    SDLContext context;

    int window_width = image.width + PADDING;
    int window_height = image.height + PADDING;
    float aspect_ratio = (float)window_width / (float)window_height;

    if (window_width > 1024 || window_height > 1024) {
        if (aspect_ratio >= 1.0) {
            window_width = 1024;
            window_height = 1024 / aspect_ratio;
        } else {
            window_height = 1024;
            window_width = 1024 * aspect_ratio;
        }
    }

    if (!context_init(&context, window_width + PADDING, window_height + PADDING, image.width + PADDING, image.height + PADDING)) {
        context_deinit(&context);
        return -1;
    }

    Heap heap;
    heap_init(&heap);

    Quad root = quad_init_from_image(&image);
    heap_push(&heap, &root);

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                for (size_t i = 0; i < 10; i++) {
                    Quad *quad = heap_pop(&heap);
                    Children *children = quad_split(quad);
                    heap_push(&heap, &children->top_left);
                    heap_push(&heap, &children->top_right);
                    heap_push(&heap, &children->bottom_left);
                    heap_push(&heap, &children->bottom_right);
                }
            }
        }

        draw_image(&context, &heap);
    }

    context_deinit(&context);

    return 0;
}
