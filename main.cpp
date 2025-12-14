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

int signedTriangleArea(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5 * (ax*(by-cy) + bx*(cy-ay) + cx*(ay-by));
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    int minx = std::min(std::min(ax, bx), cx);
    int miny = std::min(std::min(ay, by), cy);
    int maxx = std::max(std::max(ax, bx), cx);
    int maxy = std::max(std::max(ay, by), cy);
    #pragma omp parallel for
    for (int x=minx; x<=maxx; x++) {
        for (int y=miny; y<=maxy; y++) {
            int alpha = signedTriangleArea(x, y, bx, by, cx, cy);
            int beta  = signedTriangleArea(ax, ay, x, y, cx, cy);
            int gamma = signedTriangleArea(ax, ay, bx, by, x, y);
            if(signedTriangleArea(ax, ay, bx, by, cx, cy) < 0) {
                alpha = -alpha;
                beta  = -beta;
                gamma = -gamma;
            }
            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                framebuffer.set(x, y, color);
            }
        }
    }
}

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);
    framebuffer.write_tga_file("rastWithShoelace.tga");
    return 0;
}

// command to run cmake ..; cmake --build .; .\Debug\renderer.exe < inside the build folder
