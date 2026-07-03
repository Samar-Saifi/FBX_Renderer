#include "mesh.h"
#include "glad/glad.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>

static void FindAndProcessMeshes(aiNode* node, const aiScene* scene, std::vector<std::vector<Vertex>>& outVertices, std::vector<std::vector<unsigned int>>& outIndices) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            Vertex v;
            v.position = glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
            if (mesh->HasNormals()) {
                v.normal = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
            }else {
                v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
            vertices.push_back(v);
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                indices.push_back(face.mIndices[k]);
            }
        }

        outVertices.push_back(vertices);
        outIndices.push_back(indices);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        FindAndProcessMeshes(node->mChildren[i], scene, outVertices, outIndices);
}

bool Mesh::LoadFromFile(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    if (scene->mNumMeshes == 0) {
        std::cout << "No meshes found in file!" << std::endl;
        return false;
    }

    std::vector <std::vector<Vertex>> allVertices;
    std::vector <std::vector<unsigned int>> allIndices;
    FindAndProcessMeshes(scene->mRootNode, scene, allVertices, allIndices);
    Clear();

    for (int i = 0; i < allVertices.size(); i++) {
        ExpandBounds(allVertices[i]);
        UploadMesh(allVertices[i], allIndices[i]);
    }

    return true;
}

void Mesh::ExpandBounds(const std::vector<Vertex>& verts) {
    for (const auto& v : verts) {
        boundsMin = glm::min(boundsMin, v.position);
        boundsMax = glm::max(boundsMax, v.position);
    }
}
 
void Mesh::Clear() {
    for (auto& b : buffersList) {
        glDeleteVertexArrays(1, &b.VAO);
        glDeleteBuffers(1, &b.VBO);
        glDeleteBuffers(1, &b.EBO);
    }
    buffersList.clear();
 
    totalVertices = 0;
    totalPolygons = 0;
 
    boundsMin = glm::vec3(std::numeric_limits<float>::max());
    boundsMax = glm::vec3(std::numeric_limits<float>::lowest());
}


void Mesh::UploadMesh(const std::vector<Vertex>& verts,
                      const std::vector<unsigned int>& indices)
{

    Buffers buffers;

    buffers.indexCount = (unsigned int)indices.size();
    totalVertices += (unsigned int)verts.size();
    totalPolygons += (unsigned int)(indices.size() / 3);

    glGenVertexArrays(1, &buffers.VAO);
    glGenBuffers(1, &buffers.VBO);
    glGenBuffers(1, &buffers.EBO);

    glBindVertexArray(buffers.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 verts.size() * sizeof(Vertex),
                 verts.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    buffersList.push_back(buffers);
}


void Mesh::draw() const {
    for (auto& b : buffersList) {
        glBindVertexArray(b.VAO);
        glDrawElements(GL_TRIANGLES, b.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

Mesh::~Mesh() {
    Clear();
}
