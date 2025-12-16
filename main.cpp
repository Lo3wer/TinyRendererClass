#define _USE_MATH_DEFINES
#include <cmath>
#include "tgaimage.h"
#include "model.h"

constexpr int width = 2000;
constexpr int height = 2000;

constexpr TGAColor white = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};
constexpr TGAColor blue = {255, 128, 64, 255};
constexpr TGAColor yellow = {0, 200, 255, 255};

vec3 rot(vec3 v)
{
    static const double a = M_PI / 6;
    static const mat<3, 3> Ry = {{{std::cos(a), 0, std::sin(a)}, {0, 1, 0}, {-std::sin(a), 0, std::cos(a)}}};
    return Ry * v;
}

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color)
{
    bool steep = false;
    if (std::abs(ax - bx) < std::abs(ay - by))
    {
        std::swap(ax, ay);
        std::swap(bx, by);
        steep = true;
    }
    if (ax > bx)
    {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    for (float x = ax; x <= bx; x += 1)
    {
        float t = (float)((x - ax) / (bx - ax));
        int y = round(ay * (1.0 - t) + by * t);
        if (steep)
        {
            framebuffer.set(y, x, color);
        }
        else
        {
            framebuffer.set(x, y, color);
        }
    }
}

float signedTriangleArea(int ax, int ay, int bx, int by, int cx, int cy)
{
    return 0.5 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
}

void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage &framebuffer, TGAImage &zbuffer, TGAColor color)
{
    int minx = std::min(std::min(ax, bx), cx);
    int miny = std::min(std::min(ay, by), cy);
    int maxx = std::max(std::max(ax, bx), cx);
    int maxy = std::max(std::max(ay, by), cy);
    float area = signedTriangleArea(ax, ay, bx, by, cx, cy);
    if (area < 1.0)
    {
        return;
    }
#pragma omp parallel for
    for (int x = minx; x <= maxx; x++)
    {
        for (int y = miny; y <= maxy; y++)
        {
            float alpha = signedTriangleArea(x, y, bx, by, cx, cy) / area;
            float beta = signedTriangleArea(ax, ay, x, y, cx, cy) / area;
            float gamma = signedTriangleArea(ax, ay, bx, by, x, y) / area;
            if (alpha >= 0.0 && beta >= 0.0 && gamma >= 0.0)
            {
                unsigned char z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
                if (z > zbuffer.get(x, y)[0])
                {
                    zbuffer.set(x, y, {z});
                    framebuffer.set(x, y, color);
                }
            }
        }
    }
}

std::tuple<int, int, int> project(const vec3 &v)
{
    int x = (v.x + 1.) * width / 2.;
    int y = (v.y + 1.) * height / 2.;
    int z = (v.z + 1.) * 255 / 2.;
    return {x, y, z};
}

int main(int argc, char **argv)
{
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    Model model("african_head.obj");
    // Model model("C:\\Users\\TestUser\\TinyRendererClass\\obj\\diablo3_pose\\diablo3_pose.obj");

    for (int i = 0; i < model.nfaces(); i++)
    {
        auto [ax, ay, az] = project(rot(model.vert(i, 0)));
        auto [bx, by, bz] = project(rot(model.vert(i, 1)));
        auto [cx, cy, cz] = project(rot(model.vert(i, 2)));
        TGAColor rnd;
        for (int c = 0; c < 3; c++)
            rnd[c] = std::rand() % 255;
        triangle(ax, ay, az, bx, by, bz, cx, cy, cz, framebuffer, zbuffer, rnd);
    }

    framebuffer.write_tga_file("rotatedHead.tga");
    zbuffer.write_tga_file("rotatedHeadZBuffer.tga");

    return 0;
}

// command to run cmake ..; cmake --build .; .\Debug\renderer.exe < inside the build folder
