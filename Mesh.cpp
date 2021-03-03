#include "Mesh.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    : vertices(std::move(vertices)), indices(std::move(indices)) {
    setupMesh();
}

void Mesh::draw(Shader &shader, GLenum mode) const {
    for (int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0+i);
        std::string number;
        std::string name = textures[i].type;

        glUniform1i(glGetUniformLocation(shader.ID, name.c_str()), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

void Mesh::loadTexture(const char *path, std::string type) {
    std::string filename = std::string(path);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    Texture texture;
    texture.type = std::move(type);
    texture.id = textureID;
    textures.push_back(texture);
}

Mesh generateSphere(float radius, unsigned int rings, unsigned int segments) {
    rings++;
    segments++;

    float const R = 1.0f/(float)(rings - 1);
    float const S = 1.0f/(float)(segments - 1);

    std::vector<Vertex> vertices(rings * segments);
    std::vector<unsigned int> indices((rings-1) * (segments-1) * 6);

    int index = 0;

    for (int r = 0; r < rings; r++)
        for (int s = 0; s < segments; s++) {
            auto pi = glm::pi<float>();

            float const x = cos(2*pi * s * S) * sin( pi * r * R );
            float const y = sin( -pi/2 + pi * r * R );
            float const z = sin(2*pi * s * S) * sin( pi * r * R );

            vertices[index].position.x = x * radius;
            vertices[index].position.y = y * radius;
            vertices[index].position.z = z * radius;

            vertices[index].normal.x = x * radius;
            vertices[index].normal.y = y * radius;
            vertices[index].normal.z = z * radius;

            vertices[index].texCoords.x = r*S;
            vertices[index].texCoords.y = s*R;
            index++;
        }

    auto i = indices.begin();
    for (int r = 0; r < rings-1; r++)
        for (int s = 0; s < segments-1; s++) {
            *i++ = r * segments + s;
            *i++ = r * segments + (s + 1);
            *i++ = (r+1) * segments + (s + 1);
            *i++ = (r+1) * segments + (s + 1);
            *i++ = (r+1) * segments + s;
            *i++ = r * segments + s;
        }

    return Mesh(vertices, indices);
}

Mesh generatePlane(float width) {
    std::vector<Vertex> vertices(4);
    float halfWidth = width / 2;
    vertices[0].position.x = 1 * halfWidth;
    vertices[0].position.y = 0.0f;
    vertices[0].position.z = 1 * halfWidth;

    vertices[0].texCoords.x = 1 * halfWidth/4;
    vertices[0].texCoords.y = 1 * halfWidth/4;

    vertices[1].position.x = -1 * halfWidth;
    vertices[1].position.y = 0.0f;
    vertices[1].position.z = 1 * halfWidth;

    vertices[1].texCoords.x = 0;
    vertices[1].texCoords.y = 1 * halfWidth/4;

    vertices[2].position.x = -1 * halfWidth;
    vertices[2].position.y = 0.0f;
    vertices[2].position.z = -1 * halfWidth;

    vertices[2].texCoords.x = 0;
    vertices[2].texCoords.y = 0;

    vertices[3].position.x = 1 * halfWidth;
    vertices[3].position.y = 0;
    vertices[3].position.z = -1 * halfWidth;

    vertices[3].texCoords.x = 1 * halfWidth/4;
    vertices[3].texCoords.y = 0;

    for (int i = 0; i < 4; i++) {
        vertices[i].normal.x = 0;
        vertices[i].normal.y = 1;
        vertices[i].normal.z = 0;
    }

    std::vector<unsigned int> indices(6);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;

    return Mesh(vertices, indices);
}
