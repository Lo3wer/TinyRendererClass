#include <cstdlib>
#include "sgl.h"
#include "model.h"

extern mat<4,4> ModelView, Perspective;
extern std::vector<double> zbuffer;

struct RandomShader : IShader {
    const Model &model;
    TGAColor color = {};
    vec3 tri[3];

    RandomShader(const Model &m) : model(m) {// figure out the reason this is empty, and what purpose this serves

    }

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = model.vert(face, vert);
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};
        tri[vert] = gl_Position.xyz();
        return Perspective * gl_Position;
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        return {false, color};
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    constexpr int width  = 800;    // output image size
    constexpr int height = 800;
    constexpr vec3    eye{-1,0,2}; // camera position
    constexpr vec3 center{0,0,0};  // camera direction
    constexpr vec3     up{0,1,0};  // camera up vector

    lookat(eye, center, up); // build the ModelView   matrix
    init_perspective(norm(eye-center)); // build the Perspective matrix
    init_viewport(0, 0, width, height); // build the Viewport matrix
    init_zbuffer(width,height);

    TGAImage framebuffer(width, height, TGAImage::RGB, {177, 195, 209, 255});
    for (int m=1; m<argc; m++) { // iterate through all input objects
        Model model(argv[m]);
        RandomShader shader(model);
        for (int i=0; i<model.nfaces(); i++) { // iterate through all triangles
            shader.color = {uint8_t(std::rand()%255), uint8_t(std::rand()%255), uint8_t(std::rand()%255), 255};
            Triangle clip = {shader.vertex(i,0), shader.vertex(i,1),shader.vertex(i,2)};
            rasterize(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("framebuffersgl.tga");
    return 0;
}

// command to run cmake ..; cmake --build .; .\Debug\renderer.exe < inside the build folder
