#include "geometry.h"
#include <vector>

class Model {
    std::vector<vec3> vertices = {}; // array of vertices
    std::vector<vec3> normals = {}; // array of normal vectors at given points.
    std::vector<vec2> uvs = {}; // array of texture coordinates
    std::vector<int> normalIndices = {}; // normals of triangles defined by faces. faces = normalIndices
    std::vector<int> uvIndices = {}; // texture coordinate indices
    std::vector<int> faces = {}; // triangles defined by vertex indices every 3 indices is a face 0-2, 3-5, ...
public:
    Model(const std::string& filename);
    ~Model();
    int nverts() const; // number of vertices
    int nnormals() const; // number of normal vectors
    int nfaces() const; // number of triangles -> should equal nfaces

    vec3 vert(const int i) const; // 0 <= i < nverts()
    vec3 vert(const int iface, const int nthvert) const; // 0 <= iface <= nfaces(), 0 <= nthvert < 3
    vec3 normalVert(const int normalIndex, const int nthNormal) const; // 0 <= normalIndex <= nfaces(), 0 <= nthvert < 3
    vec2 uv(const int iface, const int nthvert) const; // get UV coordinate for nth vertex of face iface
};