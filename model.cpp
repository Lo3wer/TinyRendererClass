#include "model.h"

#include <iostream>
#include <fstream>
#include <sstream>

// Constructor - loads the OBJ file
Model::Model(const std::string& filename) {
    std::ifstream file(filename); //get file
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {// while more lines to read
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line); // process line with input string stream
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            vec3 v;
            iss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if (prefix == "f") {
            // Read face: f v1 v2 v3 (or f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3)
            // Only use vertex indices (v1, v2, v3) add 3 indices to faces vector
            std::string vertex;
            int counter = 0;

            while (iss >> vertex && counter < 3) {//ensure there are only 3 vertices per face
                // Handle "v/vt/vn" format - we only want the first number
                int slash_pos = vertex.find('/');
                if (slash_pos != -1) {
                    vertex = vertex.substr(0, slash_pos);
                }
                // No slashes, use the whole string if condition fails

                int idx = std::stoi(vertex);  // Convert string to int
                faces.push_back(idx - 1); // OBJ indices are 1-based
                counter++;
            }
        }
    }

    file.close();
    std::cout << "Loaded " << filename << ": "
        << vertices.size() << " vertices, "
        << faces.size() << " faces" << std::endl;
}

// Destructor
Model::~Model() {

}

// Return number of vertices
int Model::nverts() const {
    return vertices.size();
}

// Return number of faces
int Model::nfaces() const {
    return faces.size();
}

// Return vertex at index i
vec3 Model::vert(const int i) const {
    if (i < 0 || i >= vertices.size()) {
        std::cerr << "Index out of bounds in Model::vert()" << std::endl;
        return vec3();
    }
    return vertices[i];
}

// Return the nth vertex of face iface
vec3 Model::vert(const int iface, const int nthvert) const {
    int index = iface * 3 + nthvert;
    if (index < 0 || index >= faces.size()) {
        std::cerr << "Index out of bounds in Model::vert()" << std::endl;
        return vec3();
    }
    int face_index = faces[index];
    if (face_index < 0 || face_index >= vertices.size()) {
        std::cerr << "Vertex index out of bounds in Model::vert()" << std::endl;
        return vec3();
    }
    return vertices[face_index];
}