#include <cstdlib>
#include <algorithm>
#include "sgl.h"
#include "model.h"

extern mat<4,4> ModelView, Perspective;
extern std::vector<double> zbuffer;

struct SmoothShader : IShader {
    const Model &model;
    TGAColor color = {};
    vec3 tri[3];
    mat<3,3> normalVectors;
    mat<3,2> uvCoords; // UV coordinates for the triangle's 3 vertices
    vec3 lightSource;
    vec3 eye;
    int ambientWeight, diffuseWeight, specularWeight, darkness, shinyness;
    TGAImage *normalMap; // Pointer to the normal map texture


    SmoothShader(const Model &m) : model(m), normalMap(nullptr) {

    }

    virtual vec4 vertex(const int face, const int vert) {
        normalVectors[vert] = model.normalVert(face, vert);
        uvCoords[vert] = model.uv(face, vert); // Store UV coordinates
        vec3 v = model.vert(face, vert);
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};
        tri[vert] = gl_Position.xyz();
        return Perspective * gl_Position;
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        vec3 normal;
        
        // If we have a normal map, sample it using interpolated UV coordinates
        if (normalMap) {
            // Interpolate UV coordinates across the triangle using barycentric coordinates
            vec2 uv = uvCoords.transpose() * bar;
            
            // Sample the normal map at the UV position
            int x = int(uv.x * normalMap->width());
            int y = int(uv.y * normalMap->height());
            
            // Get RGB color from normal map
            TGAColor c = normalMap->get(x, y);
            
            // Convert RGB (0-255) to XYZ (-1 to +1)
            normal.x = (c[2] / 255.0) * 2.0 - 1.0; // R → X
            normal.y = (c[1] / 255.0) * 2.0 - 1.0; // G → Y  
            normal.z = (c[0] / 255.0) * 2.0 - 1.0; // B → Z
            normal = normalized(normal);
        } else {
            // Fallback to interpolated vertex normals
            normal = normalized(normalVectors.transpose()*bar);
        }
        
        vec3 reflection = 2*normal*(normal*lightSource) - lightSource; //reflection of light
        double diffuseDegree = std::max<double>(0.,normal*lightSource);
        double specularDegree = std::pow<double>(std::max<double>(0.,eye*reflection),shinyness);
        uint8_t sumColor = uint8_t(std::min<int>(255,255*(ambientWeight + diffuseDegree*diffuseWeight + specularDegree*specularWeight)/(ambientWeight + diffuseWeight + specularWeight + darkness))); //normalizing everything to uint8_t
        TGAColor temp = {sumColor, sumColor, sumColor, 255};
        return {false, temp};
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
    constexpr vec3 lightSource{1,0,1}; // source of the light

    lookat(eye, center, up); // build the ModelView   matrix
    init_perspective(norm(eye-center)); // build the Perspective matrix
    init_viewport(0, 0, width, height); // build the Viewport matrix
    init_zbuffer(width,height);

    TGAImage framebuffer(width, height, TGAImage::RGB, {0, 0, 0, 255});
    
    // Load the normal map (example path - adjust to your actual file)
    TGAImage normalMap;
    bool hasNormalMap = normalMap.read_tga_file("obj/african_head/african_head_nm.tga");
    if (!hasNormalMap) {
        std::cerr << "Warning: Could not load normal map. Using vertex normals instead." << std::endl;
    }
    
    for (int m=1; m<argc; m++) { // iterate through all input objects
        Model model(argv[m]);
        SmoothShader shader(model);
        //shader init
        shader.eye = eye;
        shader.lightSource = lightSource;
        shader.ambientWeight = 1;
        shader.diffuseWeight = 3;
        shader.specularWeight = 2;
        shader.darkness = 3;
        shader.shinyness = 1;
        
        // Set the normal map if loaded successfully
        if (hasNormalMap) {
            shader.normalMap = &normalMap;
        }
        
        for (int i=0; i<model.nfaces(); i++) { // iterate through all triangles
            Triangle clip = {shader.vertex(i,0),shader.vertex(i,1),shader.vertex(i,2)};
            rasterize(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("framebufferphong.tga");
    return 0;
}

// command to run cmake ..; cmake --build .; .\Debug\renderer.exe < inside the build folder
