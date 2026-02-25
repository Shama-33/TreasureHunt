#ifndef CONE_H
#define CONE_H

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

#define PI 3.1416

using namespace std;

class Cone
{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    // Constructor
    Cone(float radius = 1.0f, float height = 2.0f, int sectorCount = 20, glm::vec3 amb = glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3 diff = glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f), float shiny = 32.0f)
    {
        set(radius, height, sectorCount, amb, diff, spec, shiny);
        buildCoordinatesAndIndices();
        buildVertices();

        // Create VAO
        glGenVertexArrays(1, &coneVAO);
        glBindVertexArray(coneVAO);

        // Create VBO
        unsigned int coneVBO;
        glGenBuffers(1, &coneVBO);
        glBindBuffer(GL_ARRAY_BUFFER, coneVBO);
        glBufferData(GL_ARRAY_BUFFER, this->getVertexSize(), this->getVertices(), GL_STATIC_DRAW);

        // Create EBO
        unsigned int coneEBO;
        glGenBuffers(1, &coneEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coneEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->getIndexSize(), this->getIndices(), GL_STATIC_DRAW);

        // Enable attributes
        glEnableVertexAttribArray(0); // Position
        glEnableVertexAttribArray(1); // Normal
        glVertexAttribPointer(0, 3, GL_FLOAT, false, verticesStride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, verticesStride, (void*)(sizeof(float) * 3));

        glBindVertexArray(0);
    }

    ~Cone() {}

    void drawCone(Shader& lightingShader, glm::mat4 model) const
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);

        glBindVertexArray(coneVAO);
        glDrawElements(GL_TRIANGLES, this->getIndexCount(), GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }

private:
    unsigned int coneVAO;
    float radius;
    float height;
    int sectorCount;
    vector<float> vertices;
    vector<float> normals;
    vector<unsigned int> indices;
    vector<float> coordinates;
    int verticesStride = 24;

    void set(float radius, float height, int sectorCount, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
    {
        this->radius = radius;
        this->height = height;
        this->sectorCount = sectorCount > 3 ? sectorCount : 3;
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
    }

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
        }

        // Top (apex) of the cone
        coordinates.push_back(0.0f);
        coordinates.push_back(height);
        coordinates.push_back(0.0f);
        normals.push_back(0.0f);
        normals.push_back(1.0f);
        normals.push_back(0.0f);

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
        }
    }

    unsigned int getVertexCount() const { return (unsigned int)coordinates.size() / 3; }
    unsigned int getVertexSize() const { return (unsigned int)vertices.size() * sizeof(float); }
    const float* getVertices() const { return vertices.data(); }
    unsigned int getIndexSize() const { return (unsigned int)indices.size() * sizeof(unsigned int); }
    const unsigned int* getIndices() const { return indices.data(); }
    unsigned int getIndexCount() const { return (unsigned int)indices.size(); }
};

#endif /* CONE_H */
#pragma once
