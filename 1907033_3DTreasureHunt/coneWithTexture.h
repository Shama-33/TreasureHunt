#ifndef CONE_WITH_TEXTURE_H
#define CONE_WITH_TEXTURE_H

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

#define PI 3.1416

using namespace std;

class ConeWithTexture
{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    // Texture maps
    unsigned int diffuseMap;
    unsigned int specularMap;

    // Constructor
    ConeWithTexture(float radius = 1.0f, float height = 2.0f, int sectorCount = 20,
        glm::vec3 amb = glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3 diff = glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f),
        float shiny = 32.0f,
        unsigned int diffuseTexture = 0,
        unsigned int specularTexture = 0)
        : radius(radius), height(height), sectorCount(sectorCount > 3 ? sectorCount : 3),
        ambient(amb), diffuse(diff), specular(spec), shininess(shiny),
        diffuseMap(diffuseTexture), specularMap(specularTexture), verticesStride(32)
    {
        buildCoordinatesAndIndices();
        buildVertices();
        setupVAO();
    }

    ~ConeWithTexture()
    {
        glDeleteVertexArrays(1, &coneVAO);
    }

    // Draw method with textures
    void drawCone(Shader& lightingShader, glm::mat4 model, glm::vec3 viewPos) const
    {
        lightingShader.use();

        // Pass material properties
        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuseColor", this->diffuse);
        lightingShader.setVec3("material.specularColor", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        // Pass texture units
        lightingShader.setInt("material.diffuse", 0);
        lightingShader.setInt("material.specular", 1);

        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // Set transformation matrices
        lightingShader.setMat4("model", model);
        lightingShader.setVec3("viewPos", viewPos);

        // Draw the cone
        glBindVertexArray(coneVAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int coneVAO;
    float radius;
    float height;
    int sectorCount;
    vector<float> vertices;
    vector<float> normals;
    vector<float> texCoords; // Texture coordinates
    vector<float> coordinates;
    vector<unsigned int> indices;
    int verticesStride;

    void buildCoordinatesAndIndices()
    {
        float sectorStep = 2 * PI / sectorCount;
        float sectorAngle = 0.0f;

        // Bottom center
        coordinates.push_back(0.0f);
        coordinates.push_back(0.0f);
        coordinates.push_back(0.0f);
        normals.push_back(0.0f);
        normals.push_back(-1.0f);
        normals.push_back(0.0f);
        texCoords.push_back(0.5f);
        texCoords.push_back(0.5f);

        // Bottom circle
        for (int i = 0; i <= sectorCount; ++i)
        {
            sectorAngle = i * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);
            coordinates.push_back(x);
            coordinates.push_back(0.0f);
            coordinates.push_back(z);
            normals.push_back(0.0f);
            normals.push_back(-1.0f);
            normals.push_back(0.0f);
            texCoords.push_back((x / radius + 1.0f) * 0.5f);
            texCoords.push_back((z / radius + 1.0f) * 0.5f);
        }

        // Top (apex) of the cone
        coordinates.push_back(0.0f);
        coordinates.push_back(height);
        coordinates.push_back(0.0f);
        normals.push_back(0.0f);
        normals.push_back(1.0f);
        normals.push_back(0.0f);
        texCoords.push_back(0.5f);
        texCoords.push_back(0.5f);

        // Indices for bottom circle
        for (int i = 1; i <= sectorCount; ++i)
        {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        // Indices for cone sides
        int apexIndex = coordinates.size() / 3 - 1;
        for (int i = 1; i <= sectorCount; ++i)
        {
            indices.push_back(i);
            indices.push_back(i + 1);
            indices.push_back(apexIndex);
        }
    }

    void buildVertices()
    {
        size_t count = coordinates.size();
        for (size_t i = 0; i < count; i += 3)
        {
            vertices.push_back(coordinates[i]);
            vertices.push_back(coordinates[i + 1]);
            vertices.push_back(coordinates[i + 2]);

            vertices.push_back(normals[i]);
            vertices.push_back(normals[i + 1]);
            vertices.push_back(normals[i + 2]);

            vertices.push_back(texCoords[i / 3 * 2]);
            vertices.push_back(texCoords[i / 3 * 2 + 1]);
        }
    }

    void setupVAO()
    {
        glGenVertexArrays(1, &coneVAO);
        glBindVertexArray(coneVAO);

        // Create VBO
        unsigned int coneVBO;
        glGenBuffers(1, &coneVBO);
        glBindBuffer(GL_ARRAY_BUFFER, coneVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        // Create EBO
        unsigned int coneEBO;
        glGenBuffers(1, &coneEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coneEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Configure vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, verticesStride, (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, verticesStride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, verticesStride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};

#endif /* CONE_WITH_TEXTURE_H */
