#include <stdint.h>

typedef struct {
    uint8_t *data;
    int width;
    int height;
} Image;

typedef struct {
    uint32_t left, right, top, bottom;
} Box;

typedef struct {
    Box box;
    uint64_t area;
} Boundary;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Color;

typedef struct {
    uint8_t value;
    float error;
} WeightedColor;

typedef struct {
    Color color;
    float error;
} AverageColor;

struct Children;
typedef struct Children Children;

typedef struct {
    const Image *image;
    Boundary boundary;
    AverageColor average_color;
    Children *children;
} Quad;

typedef struct Children {
    Quad top_left;
    Quad top_right;
    Quad bottom_left;
    Quad bottom_right;
} Children;

Quad quad_init_from_image(const Image *image);
Children* quad_split(Quad *quad);
