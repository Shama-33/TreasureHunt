#ifndef HALFCYL_H
#define HALFCYL_H

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

#define PI 3.1416

class halfCylinderWithTexture {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    // Texture maps
    unsigned int diffuseMap;
    unsigned int specularMap;

    halfCylinderWithTexture(float baseRadius = 1.0f, float topRadius = 1.0f, float height = 2.0f,
        int sectorCount = 20, int stackCount = 1,
        glm::vec3 amb = glm::vec3(0.2f, 0.2f, 0.2f),
        glm::vec3 diff = glm::vec3(0.8f, 0.8f, 0.8f),
        glm::vec3 spec = glm::vec3(1.0f, 1.0f, 1.0f), float shiny = 32.0f,
        unsigned int diffuseTexture = 0, unsigned int specularTexture = 0)
        : verticesStride(32), diffuseMap(diffuseTexture), specularMap(specularTexture) {
        set(baseRadius, topRadius, height, sectorCount, stackCount, amb, diff, spec, shiny);
        buildCoordinatesAndIndices();
        buildVertices();
        setupVAO();
    }

    ~halfCylinderWithTexture() {
        glDeleteVertexArrays(1, &cylinderVAO);
    }

    void drawCylinder(Shader& lightingShader, glm::mat4 model, glm::vec3 viewPos) const {
        lightingShader.use();

        // Pass material properties
        lightingShader.setVec3("material.ambient", ambient);
        lightingShader.setVec3("material.diffuseColor", diffuse);
        lightingShader.setVec3("material.specularColor", specular);
        lightingShader.setFloat("material.shininess", shininess);

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

        // Draw the half-cylinder
        glBindVertexArray(cylinderVAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int cylinderVAO;
    float baseRadius, topRadius, height;
    int sectorCount, stackCount;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<float> coordinates;
    std::vector<unsigned int> indices;
    int verticesStride;

    void set(float baseRadius, float topRadius, float height, int sectors, int stacks,
        glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny) {
        this->baseRadius = baseRadius;
        this->topRadius = topRadius;
        this->height = height;
        this->sectorCount = std::max(sectors, 3);
        this->stackCount = std::max(stacks, 1);
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
    }

    void buildCoordinatesAndIndices() {
        float sectorStep = PI / sectorCount;  // Half-circle step
        float stackStep = height / stackCount;
        float sectorAngle;

        // Generate vertices
        for (int i = 0; i <= stackCount; ++i) {
            float y = -height / 2 + i * stackStep;
            float radius = baseRadius + (topRadius - baseRadius) * i / stackCount;

            for (int j = 0; j <= sectorCount; ++j) {
                sectorAngle = j * sectorStep;
                float x = radius * cosf(sectorAngle);
                float z = radius * sinf(sectorAngle);

                // Position
                coordinates.push_back(x);
                coordinates.push_back(y);
                coordinates.push_back(z);

                // Normal
                glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));
                normals.push_back(normal.x);
                normals.push_back(normal.y);
                normals.push_back(normal.z);

                // Texture coordinates
                texCoords.push_back((float)j / sectorCount);
                texCoords.push_back((float)i / stackCount);
            }
        }

        // Generate indices
        int k1, k2;
        for (int i = 0; i < stackCount; ++i) {
            k1 = i * (sectorCount + 1);
            k2 = k1 + sectorCount + 1;

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }

        // Generate caps
        if (topRadius > 0.0f) buildCap(topRadius, height / 2, true);
        if (baseRadius > 0.0f) buildCap(baseRadius, -height / 2, false);
    }

    void buildCap(float radius, float y, bool isTop) {
        float sectorStep = PI / sectorCount;
        float sectorAngle;

        int centerIndex = coordinates.size() / 3;
        coordinates.push_back(0.0f);
        coordinates.push_back(y);
        coordinates.push_back(0.0f);

        normals.push_back(0.0f);
        normals.push_back(isTop ? 1.0f : -1.0f);
        normals.push_back(0.0f);

        texCoords.push_back(0.5f);
        texCoords.push_back(0.5f);

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);

            coordinates.push_back(x);
            coordinates.push_back(y);
            coordinates.push_back(z);

            normals.push_back(0.0f);
            normals.push_back(isTop ? 1.0f : -1.0f);
            normals.push_back(0.0f);

            texCoords.push_back((x / radius + 1.0f) * 0.5f);
            texCoords.push_back((z / radius + 1.0f) * 0.5f);

            if (j < sectorCount) {
                if (isTop) {
                    indices.push_back(centerIndex);
                    indices.push_back(centerIndex + j + 1);
                    indices.push_back(centerIndex + j + 2);
                }
                else {
                    indices.push_back(centerIndex);
                    indices.push_back(centerIndex + j + 2);
                    indices.push_back(centerIndex + j + 1);
                }
            }
        }
    }

    void buildVertices() {
        vertices.clear();
        size_t vertexCount = coordinates.size() / 3;
        for (size_t i = 0; i < vertexCount; ++i) {
            vertices.push_back(coordinates[i * 3]);
            vertices.push_back(coordinates[i * 3 + 1]);
            vertices.push_back(coordinates[i * 3 + 2]);

            vertices.push_back(normals[i * 3]);
            vertices.push_back(normals[i * 3 + 1]);
            vertices.push_back(normals[i * 3 + 2]);

            vertices.push_back(texCoords[i * 2]);
            vertices.push_back(texCoords[i * 2 + 1]);
        }
    }

    void setupVAO() {
        glGenVertexArrays(1, &cylinderVAO);
        glBindVertexArray(cylinderVAO);

        unsigned int VBO, EBO;
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        int stride = verticesStride;
        glEnableVertexAttribArray(0); // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

        glEnableVertexAttribArray(1); // Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(2); // Texture coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

        glBindVertexArray(0);
    }
};

#endif // HALFCYLINDER_WITH_TEXTURE_H
