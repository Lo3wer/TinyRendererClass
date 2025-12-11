#include "tgaimage.h"
#include "model.h"

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

int main(int argc, char** argv) {
    constexpr int width  = 1024;
    constexpr int height = 1024;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    Model model("african_head.obj");

    for(int i = 0; i < model.nfaces(); i++) {
        for(int j = 0; j < 3; j++) {
            vec3 v0 = model.vert(i, j);
            vec3 v1 = model.vert(i, (j+1)%3);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            line(x0, y0, x1, y1, framebuffer, white);
        }
    }

    framebuffer.write_tga_file("framebuffer2.tga");
    return 0;
}

// command to run cmake ..; cmake --build .; .\Debug\renderer.exe < inside the build folder
