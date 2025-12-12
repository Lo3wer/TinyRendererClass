#include "tgaimage.h"
#include "model.h"

constexpr int width  = 128;
constexpr int height = 128;

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color) {
    bool steep = false;
    if(std::abs(ax - bx) < std::abs(ay - by)) {
        std::swap(ax, ay);
        std::swap(bx, by);
        steep = true;
    }
    if(ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    for(float x = ax; x <= bx; x += 1) {
        float t = (float)((x - ax) / (bx - ax));
        int y = round(ay * (1.0 - t) + by * t);
        if (steep) {
            framebuffer.set(y, x, color);
        } else {
            framebuffer.set(x, y, color);
        }
    }
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    //sorting by y-coordinate ay >= by >= cy
    if (ay < by) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    if (ay < cy) {
        std::swap(ax, cx);
        std::swap(ay, cy);
    }
    if (by < cy) {
        std::swap(bx, cx);
        std::swap(by, cy);
    }
    // top half
    for(int y = ay; y > by; y--){
        int x1 = ax + ((y - ay) * (bx - ax)) / (by - ay);
        int x2 = ax + ((y - ay) * (cx - ax)) / (cy - ay);
        if (x1 > x2) std::swap(x1, x2);
        for (int x = x1; x <= x2; x++) {
            framebuffer.set(x, y, color);
        }
    }
    // bottom half
    for(int y = by; y >= cy; y--){
        int x1 = bx + ((y - by) * (cx - bx)) / (cy - by);
        int x2 = ax + ((y - ay) * (cx - ax)) / (cy - ay);
        if (x1 > x2) std::swap(x1, x2);
        for (int x = x1; x <= x2; x++) {
            framebuffer.set(x, y, color);
        }
    }
}

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);
    framebuffer.write_tga_file("rastWithInt.tga");
    return 0;
}

// command to run cmake ..; cmake --build .; .\Debug\renderer.exe < inside the build folder
