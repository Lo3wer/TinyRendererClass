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
        else if (prefix == "vn"){
            vec3 vn;
            iss >> vn.x >> vn.y >> vn.z;
            normals.push_back(vn);
        }
        else if (prefix == "f") {
            // Read face: f v1 v2 v3 (or f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3)
            // Only use vertex indices (v1, v2, v3) add 3 indices to faces vector
            std::string vertex;
            std::string v;
            std::string vt;
            std::string vn;
            int counter = 0;

            while (iss >> vertex && counter < 3) {//ensure there are only 3 vertices per face
                // Handle "v/vt/vn" or "v//vn" format
                int slash_pos1 = vertex.find('/');
                int slash_pos2 = vertex.find('/', slash_pos1 + 1);
                
                if (slash_pos1 != std::string::npos) {
                    v = vertex.substr(0, slash_pos1);
                    if (slash_pos2 != std::string::npos) {
                        // Format is v/vt/vn or v//vn
                        if (slash_pos2 > slash_pos1 + 1) {
                            // v/vt/vn format
                            vt = vertex.substr(slash_pos1 + 1, slash_pos2 - slash_pos1 - 1);
                        }
                        // Extract normal index
                        vn = vertex.substr(slash_pos2 + 1);
                    }
                } else {
                    // No slashes, just vertex index
                    v = vertex;
                }

                int vertidx = std::stoi(v);  // Convert string to int
                faces.push_back(vertidx-1); // OBJ indices are 1-based
                
                if (!vn.empty()) {
                    int normidx = std::stoi(vn);
                    normalIndices.push_back(normidx-1);
                } else {
                    normalIndices.push_back(-1); // No normal for this vertex
                }
                
                counter++;
            }
        }
    }

    file.close();
    std::cout << "Loaded " << filename << ": "
        << vertices.size() << " vertices, "
        << faces.size() << " faces," 
        << normals.size() << " normals" << std::endl;
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

int Model::nnormals() const {
    return normals.size();
}

// Return vertex at index i
vec3 Model::vert(const int i) const {
    if (i < 0 || i >= vertices.size()) {
        //std::cerr << "Index out of bounds in Model::vert()" << std::endl;
        return vec3();
    }
    return vertices[i];
}

// Return the nth vertex of face iface
vec3 Model::vert(const int iface, const int nthvert) const {
    int index = iface * 3 + nthvert;
    if (index < 0 || index >= faces.size()) {
        //std::cerr << "Index out of bounds in Model::vert()" << std::endl;
        return vec3();
    }
    int face_index = faces[index];
    if (face_index < 0 || face_index >= vertices.size()) {
        //std::cerr << "Vertex index out of bounds in Model::vert()" << std::endl;
        return vec3();
    }
    return vertices[face_index];
}

vec3 Model::normalVert(const int normalIndex, const int nthNormal) const {
    int index = normalIndex * 3 + nthNormal;
    if (index < 0 || index >= normalIndices.size()) {
        return vec3();
    }
    int normal_index = normalIndices[index];
    if (normal_index < 0 || normal_index >= normals.size()) {
        return vec3();
    }
    return normals[normal_index];
}