#include "geometry.h"
#include <vector>

class Model {
    std::vector<vec3> vertices = {}; // array of vertices
    std::vector<int> faces = {}; // triangles defined by vertex indices every 3 indices is a face 0-2, 3-5, ...
public:
    Model(const std::string& filename);
    ~Model();
    int nverts() const; // number of vertices
    int nfaces() const; // number of triangles
    vec3 vert(const int i) const; // 0 <= i < nverts()
    vec3 vert(const int iface, const int nthvert) const; // 0 <= iface <= nfaces(), 0 <= nthvert < 3
};