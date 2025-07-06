#include "quad.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static uint64_t box_area(Box *box) {
    return (box->right - box->left) * (box->bottom - box->top);
}

static void calculate_histogram(const Image *image, const Box *box, uint32_t histogram[static 768]) {
    for (uint32_t row = box->top; row < box->bottom; row++) {
        for (uint32_t column = box->left; column < box->right; column++) {
            uint8_t red = image->data[row * image->width * 3 + column * 3 + 0];
            uint8_t green = image->data[row * image->width * 3 + column * 3 + 1];
            uint8_t blue = image->data[row * image->width * 3 + column * 3 + 2];

            histogram[0 + red]++; // 0 - 255
            histogram[256 + green]++; // 256 - 511
            histogram[512 + blue]++; // 512 - 768
        }
    }
}

static WeightedColor weighted_color(const uint32_t histogram[static 256]) {
    uint64_t total = 0;
    for (size_t i = 0; i < 256; i++) {
        total += histogram[i];
    }

    float mean = 0;
    for (size_t i = 0; i < 256; i++) {
        mean += histogram[i] * i;
    }
    mean /= total;

    float variance = 0;
    for (size_t i = 0; i < 256; i++) {
        float diff = i - mean;
        variance += histogram[i] * diff * diff;
    }

    variance /= total;

    return (WeightedColor) {
        .value = mean,
        .error = sqrt(variance)
    };
}

static AverageColor color_from_histogram(const uint32_t histogram[static 768]) {
    WeightedColor red = weighted_color(&histogram[0]);
    WeightedColor green = weighted_color(&histogram[256]);
    WeightedColor blue = weighted_color(&histogram[512]);

    float error = 0.299 * red.error + 0.587 * green.error + 0.114 * blue.error;

    return (AverageColor) {
        .color = (Color) {
            .red = red.value,
            .green = green.value,
            .blue = blue.value
        },
        .error = error
    };
}

Quad quad_init(const Image *image, uint32_t left, uint32_t right, uint32_t top, uint32_t bottom) {
    Box box = (Box) {
        .left = left,
        .top = top,
        .right = right,
        .bottom = bottom,
    };

    Boundary boundary = (Boundary) {
        .box = box,
        .area = box_area(&box)
    };

    uint32_t histogram[256 * 3] = {0};
    calculate_histogram(image, &box, histogram);
    AverageColor average_color = color_from_histogram(histogram);

    return (Quad) {
        .image = image,
        .boundary = boundary,
        .average_color = average_color,
        .children = nullptr
    };
}

Quad quad_init_from_image(const Image *image) {
    return quad_init(image, 0, image->width, 0, image->height);
}

Children* quad_split(Quad *quad) {
    quad->children = malloc(sizeof(Children));
    // TODO: error checking

    Box box = quad->boundary.box;
    uint32_t mlr = box.left + (box.right - box.left) / 2;
    uint32_t mtb = box.top + (box.bottom - box.top) / 2;

    quad->children->top_left = quad_init(quad->image, box.left, mlr, box.top, mtb);
    quad->children->top_right = quad_init(quad->image, mlr, box.right, box.top, mtb);
    quad->children->bottom_left = quad_init(quad->image, box.left, mlr, mtb, box.bottom);
    quad->children->bottom_right = quad_init(quad->image, mlr, box.right, mtb, box.bottom);

    return quad->children;
}
