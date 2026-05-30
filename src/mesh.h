#ifndef FBXRENDERER_MESH_H
#define FBXRENDERER_MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

struct Buffers {
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
};

class Mesh {
public:
    glm::vec3 boundsMin;
    glm::vec3 boundsMax;
    std::vector<Buffers> buffersList;
    unsigned int totalVertices = 0;
    unsigned int totalPolygons = 0;

    bool LoadFromFile(const std::string& path);
    void draw() const;
    ~Mesh();

private:
    void UploadMesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& indices);
};

#endif //FBXRENDERER_MESH_H
