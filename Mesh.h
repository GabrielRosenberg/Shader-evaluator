#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <glad.h>
#include "Shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
};

/*
 * Basic mesh class based on code from learnopengl.com
 *
 * */

class Mesh {
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    //Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void loadTexture(const char *path, std::string type);
    void draw(Shader &shader, GLenum mode) const;
private:
    //  render data
    unsigned int VBO, EBO;

    void setupMesh();
};

// Functions to generate primitive meshes
Mesh generateSphere(float radius, unsigned int rings = 16, unsigned int segments = 32);
Mesh generatePlane(float width = 1);