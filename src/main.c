#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL3/SDL.h>
#include <stdlib.h>

#include "stb_image.h"

typedef struct {
    uint8_t *data;
    int width;
    int height;
} Image;

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

bool context_init(SDLContext *context, int window_width, int window_height) {
    context->framebuffer = malloc(sizeof(Framebuffer));
    if (!context->framebuffer) {
        fprintf(stderr, "Failed to malloc framebuffer\n");
        return false;
    }

    context->framebuffer->width = window_width;
    context->framebuffer->height = window_height;

    context->framebuffer->data = malloc(sizeof(uint32_t) * window_height * window_width);
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

    context->texture = SDL_CreateTexture(context->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
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

void draw_image(const SDLContext *context, const Image *image) {
    // clear framebuffer with black
    for (size_t row = 0; row < context->framebuffer->height; row++) {
        for (size_t column = 0; column < context->framebuffer->width; column++) {
            context->framebuffer->data[row * context->framebuffer->width + column] = 0xFF000000;
        }
    }

    for (int row = 0; row < image->height; row++) {
        for (int column = 0; column < image->width; column++) {
            uint32_t offset = row * image->width * 3 + column * 3;
            uint32_t color = (0xFF << 24) | (image->data[offset] << 16) | (image->data[offset+1] << 8) | (image->data[offset+2]);
            context->framebuffer->data[row * context->framebuffer->width + column] = color;
        }
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

    if (!context_init(&context, image.width, image.height)) {
        context_deinit(&context);
        return -1;
    }

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        draw_image(&context, &image);
    }

    context_deinit(&context);

    return 0;
}
