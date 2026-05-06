#ifndef FBXRENDERER_MESH_H
#define FBXRENDERER_MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

class Mesh {
public:
    bool loadFromFile(const std::string& path);
    void draw() const;
    glm::vec3 boundsMin;
    glm::vec3 boundsMax;
    ~Mesh();

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
    void uploadMesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& indices);
};

#endif //FBXRENDERER_MESH_H
