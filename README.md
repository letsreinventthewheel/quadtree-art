# Quadtree Art

A small project exploring how to decompose and render an image using a quadtree based approach. Written in modern C (C23) with SDL3.

This project evolves from a simple framebuffer renderer to a recursive quadtree system and finally to a priority guided image subdivision using a custom min heap (priority queue).

---

## 🎥 Watch the YouTube Series

1. [Quadtree art: render pipeline setup in modern C and SDL](https://youtu.be/lCIzISWEVok)
2. [Quadtree art: linear image subdivision](https://youtu.be/0BZQA85i_Ms)
3. [Quadtree art: prioritized subdivision with a custom priority queue](https://youtu.be/ZPUmQKPxDR0)

---

## 📚 Companion Blog Posts

- [Quadtree Art, Part 1: Setting Up the Canvas](https://letsreinventthewheel.github.io/blog/quadtree-art-part1-sdl/)
- [Quadtree Art, Part 2: Linear Subdivision with Quads](https://letsreinventthewheel.github.io/blog/quadtree-art-part2-linear/)
- [Quadtree Art, Part 3: Prioritized Subdivision with Binary Heap](https://letsreinventthewheel.github.io/blog/quadtree-art-part3-heap/)

---

## 🧠 What You'll Learn

- How to build a pixel renderer with SDL3 and C23
- How to load and process image data using `stb_image.h`
- How to calculate average color and color error using histograms
- How to recursively subdivide an image using quadtrees
- How to implement a custom priority queue using a heap
- How to drive rendering based on visual importance (error × area)

---

## 🧪 Try It Out

### Build

```bash
cmake -B build
cmake --build build
```

### Run

```bash
./build/qta assets/heart.jpg
```

Press any key to split the next 10 quads.

---

## 🛠️ Dependencies

- SDL3 (via pkg-config or CMake config)
- stb_image.h (included)
- stb_ds.h (included)
- C23-compatible compiler (tested with Clang and GCC)
- CMake 3.25+

---

## 🖼️ Output

The image is subdivided and rendered progressively as filled rectangles. Each rectangle represents a quad and is colored by its average RGB value. In the final version high error areas are refined first for a visually smart result.

---

## 🙏 Acknowledgements

- [Quads by Michael Fogleman](https://github.com/fogleman/Quads) - original idea
