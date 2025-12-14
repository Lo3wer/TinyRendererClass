#include "tgaimage.h"
#include "model.h"

constexpr int width  = 2000;
constexpr int height = 2000;

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

float signedTriangleArea(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5 * (ax*(by-cy) + bx*(cy-ay) + cx*(ay-by));
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    int minx = std::min(std::min(ax, bx), cx);
    int miny = std::min(std::min(ay, by), cy);
    int maxx = std::max(std::max(ax, bx), cx);
    int maxy = std::max(std::max(ay, by), cy);
    float area = signedTriangleArea(ax, ay, bx, by, cx, cy);
    if(area < 1.0) {
        return;
    }
    #pragma omp parallel for
    for (int x=minx; x<=maxx; x++) {
        for (int y=miny; y<=maxy; y++) {
            float alpha = signedTriangleArea(x, y, bx, by, cx, cy) / area;
            float beta  = signedTriangleArea(ax, ay, x, y, cx, cy) / area;
            float gamma = signedTriangleArea(ax, ay, bx, by, x, y) / area;
            TGAColor combinedColor = {
                alpha * red[0] + beta * green[0] + gamma * blue[0],
                alpha * red[1] + beta * green[1] + gamma * blue[1],
                alpha * red[2] + beta * green[2] + gamma * blue[2],
                255
            };
            if (alpha >= 0.0 && beta >= 0.0 && gamma >= 0.0) {
                framebuffer.set(x, y, combinedColor);
            }
        }
    }
}

std::pair<int, int> project(const vec3& v) {
    int x = (v.x + 1.) * width / 2.;
    int y = (v.y + 1.) * height / 2.;
    return {x, y};
}

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    Model model("african_head.obj");

    for (int i=0; i<model.nfaces(); i++) {
        auto [ax, ay] = project(model.vert(i, 0));
        auto [bx, by] = project(model.vert(i, 1));
        auto [cx, cy] = project(model.vert(i, 2));
        TGAColor rnd;
        for (int c=0; c<3; c++) rnd[c] = std::rand()%255;
        triangle(ax, ay, bx, by, cx, cy, framebuffer, rnd);
    }

    framebuffer.write_tga_file("rainbowGuy.tga");
    // triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    // triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    // triangle(115, 83, 80,  90, 85, 120, framebuffer, green);
    // framebuffer.write_tga_file("rainbowLines.tga");

    return 0;
}

// command to run cmake ..; cmake --build .; .\Debug\renderer.exe < inside the build folder
