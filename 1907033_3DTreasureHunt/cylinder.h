//
//  cylinder.h
//

#ifndef cylinder_h
#define cylinder_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

#define PI 3.1416

using namespace std;

const int MIN_CYL_SECTOR_COUNT = 3;
const int MIN_CYL_STACK_COUNT = 1;

class Cylinder
{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    Cylinder(float baseRadius = 1.0f, float topRadius = 1.0f, float height = 2.0f,
        int sectorCount = 20, int stackCount = 1,
        glm::vec3 amb = glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3 diff = glm::vec3(0.0f, 1.0f, 1.0f),
        glm::vec3 spec = glm::vec3(0.5f, 0.5f, 1.0f), float shiny = 32.0f)
        : verticesStride(24)
    {
        set(baseRadius, topRadius, height, sectorCount, stackCount, amb, diff, spec, shiny);
        buildCoordinatesAndIndices();
        buildVertices();

        glGenVertexArrays(1, &cylinderVAO);
        glBindVertexArray(cylinderVAO);

        // Create VBO
        unsigned int cylinderVBO;
        glGenBuffers(1, &cylinderVBO);
        glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
        glBufferData(GL_ARRAY_BUFFER, getVertexSize(), getVertices(), GL_STATIC_DRAW);

        // Create EBO
        unsigned int cylinderEBO;
        glGenBuffers(1, &cylinderEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndexSize(), getIndices(), GL_STATIC_DRAW);

        // Enable attribute arrays
        glEnableVertexAttribArray(0); // Position
        glEnableVertexAttribArray(1); // Normal

        // Set attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, false, verticesStride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, verticesStride, (void*)(sizeof(float) * 3));

        // Unbind VAO and buffers
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    ~Cylinder() {}

    void set(float baseRadius, float topRadius, float height, int sectors, int stacks,
        glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
    {
        this->baseRadius = baseRadius;
        this->topRadius = topRadius;
        this->height = height;
        this->sectorCount = sectors < MIN_CYL_SECTOR_COUNT ? MIN_CYL_SECTOR_COUNT : sectors;
        this->stackCount = stacks < MIN_CYL_STACK_COUNT ? MIN_CYL_STACK_COUNT : stacks;
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
    }

    unsigned int getVertexCount() const { return (unsigned int)coordinates.size() / 3; }
    unsigned int getVertexSize() const { return (unsigned int)vertices.size() * sizeof(float); }
    int getVerticesStride() const { return verticesStride; }
    const float* getVertices() const { return vertices.data(); }
    unsigned int getIndexSize() const { return (unsigned int)indices.size() * sizeof(unsigned int); }
    const unsigned int* getIndices() const { return indices.data(); }
    unsigned int getIndexCount() const { return (unsigned int)indices.size(); }

    void drawCylinder(Shader& lightingShader, glm::mat4 model) const
    {
        lightingShader.use();
        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);
        lightingShader.setMat4("model", model);

        glBindVertexArray(cylinderVAO);
        glDrawElements(GL_TRIANGLES, getIndexCount(), GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }

private:
    void buildCoordinatesAndIndices()
    {
        float sectorStep = 2 * PI / sectorCount;
        float stackStep = height / stackCount;
        float sectorAngle;

        // Generate coordinates
        for (int i = 0; i <= stackCount; ++i)
        {
            float y = -height / 2 + i * stackStep; // Height along y-axis
            float radius = baseRadius + (topRadius - baseRadius) * i / stackCount;

            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;

                float x = radius * cosf(sectorAngle);
                float z = radius * sinf(sectorAngle); // Use z as the circular base

                coordinates.push_back(x);
                coordinates.push_back(y);
                coordinates.push_back(z);

                glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z)); // Adjust normal
                normals.push_back(normal.x);
                normals.push_back(normal.y);
                normals.push_back(normal.z);
            }
        }

        // Generate indices
        int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);
            k2 = k1 + sectorCount + 1;

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }

        // Generate caps
        if (topRadius > 0.0f)
            buildCap(topRadius, height / 2, true); // Top cap at height / 2 along y-axis

        if (baseRadius > 0.0f)
            buildCap(baseRadius, -height / 2, false); // Bottom cap at -height / 2 along y-axis
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

    void buildCap(float radius, float y, bool isTop)
    {
        float sectorStep = 2 * PI / sectorCount;
        float sectorAngle;

        int centerIndex = coordinates.size() / 3; // Index of the center vertex
        coordinates.push_back(0.0f);
        coordinates.push_back(y); // Cap center along y-axis
        coordinates.push_back(0.0f);

        normals.push_back(0.0f);
        normals.push_back(isTop ? 1.0f : -1.0f); // Normal along y-axis
        normals.push_back(0.0f);

        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);

            coordinates.push_back(x);
            coordinates.push_back(y); // Points on cap along y-axis
            coordinates.push_back(z);

            normals.push_back(0.0f);
            normals.push_back(isTop ? 1.0f : -1.0f); // Normal along y-axis
            normals.push_back(0.0f);

            if (j < sectorCount)
            {
                if (isTop)
                {
                    indices.push_back(centerIndex);
                    indices.push_back(centerIndex + j + 1);
                    indices.push_back(centerIndex + j + 2);
                }
                else
                {
                    indices.push_back(centerIndex);
                    indices.push_back(centerIndex + j + 2);
                    indices.push_back(centerIndex + j + 1);
                }
            }
        }
    }

    unsigned int cylinderVAO;
    float baseRadius, topRadius, height;
    int sectorCount, stackCount;
    vector<float> vertices;
    vector<float> normals;
    vector<unsigned int> indices;
    vector<float> coordinates;
    int verticesStride; // Bytes between consecutive vertices
};

#endif /* cylinder_h */
