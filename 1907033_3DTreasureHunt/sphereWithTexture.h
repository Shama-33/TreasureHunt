#ifndef sphereWithTexture_h
#define sphereWithTexture_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

#define PI 3.14159265359

using namespace std;



class SphereWithTexture
{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    // Texture maps
    unsigned int diffuseMap;
    unsigned int specularMap;

    // Constructor with texture maps
    SphereWithTexture(float radius = 1.0f, int sectorCount = 20, int stackCount = 18,
        glm::vec3 amb = glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3 diff = glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f),
        float shiny = 32.0f,
        unsigned int diffuseTexture = 0,
        unsigned int specularTexture = 0)
        : radius(radius), sectorCount(sectorCount), stackCount(stackCount),
        ambient(amb), diffuse(diff), specular(spec), shininess(shiny),
        diffuseMap(diffuseTexture), specularMap(specularTexture), verticesStride(32)
    {
        buildCoordinatesAndIndices();
        buildVertices();
        setupVAO();
    }

    ~SphereWithTexture()
    {
        glDeleteVertexArrays(1, &sphereVAO);
    }

    // Drawing method with textures
    void drawSphere(Shader& lightingShader, glm::mat4 model,  glm::vec3 viewPos) const
    {
        lightingShader.use();

        // Pass material properties
        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuseColor", this->diffuse); // Surface diffuse color
        lightingShader.setVec3("material.specularColor", this->specular); // Surface specular color
        lightingShader.setFloat("material.shininess", this->shininess);

        // Pass texture units
        lightingShader.setInt("material.diffuse", 0);  // Texture unit 0 for diffuse
        lightingShader.setInt("material.specular", 1); // Texture unit 1 for specular

        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // Set transformation matrices
        lightingShader.setMat4("model", model);

        // Set view position
        lightingShader.setVec3("viewPos", viewPos);

        // Draw the sphere
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    // Member variables
    unsigned int sphereVAO;
    float radius;
    int sectorCount; // Longitude
    int stackCount;  // Latitude
    vector<float> vertices;  // Interleaved vertex data
    vector<float> normals;   // Normal vectors
    vector<float> texCoords; // Texture coordinates
    vector<float> coordinates; // Vertex positions
    vector<unsigned int> indices; // Indices for element drawing
    int verticesStride;      // 32 bytes (3 pos + 3 norm + 2 tex)

    // Builds coordinates, normals, and texture coordinates
    void buildCoordinatesAndIndices()
    {
        float x, y, z, xz;
        float nx, ny, nz, lengthInv = 1.0f / radius;
        float s, t;

        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;

        for (int i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep; // From pi/2 to -pi/2
            xz = radius * cosf(stackAngle);
            y = radius * sinf(stackAngle);

            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;

                // Position
                x = xz * cosf(sectorAngle);
                z = xz * sinf(sectorAngle);
                coordinates.push_back(x);
                coordinates.push_back(y);
                coordinates.push_back(z);

                // Normalized normals
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);

                // Texture coordinates
                s = (float)j / sectorCount;
                t = (float)i / stackCount;
                texCoords.push_back(s);
                texCoords.push_back(t);
            }
        }

        // Indices
        int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // Beginning of current stack
            k2 = k1 + sectorCount + 1;      // Beginning of next stack

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                if (i != 0) // Not the top stack
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                if (i != (stackCount - 1)) // Not the bottom stack
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
    }

    // Populates vertices into a flat buffer for OpenGL
    void buildVertices()
    {
        vertices.clear();
        size_t vertexCount = coordinates.size() / 3;

        for (size_t i = 0; i < vertexCount; ++i)
        {
            // Position
            vertices.push_back(coordinates[i * 3]);
            vertices.push_back(coordinates[i * 3 + 1]);
            vertices.push_back(coordinates[i * 3 + 2]);

            // Normal
            vertices.push_back(normals[i * 3]);
            vertices.push_back(normals[i * 3 + 1]);
            vertices.push_back(normals[i * 3 + 2]);

            // Texture coordinates
            vertices.push_back(texCoords[i * 2]);
            vertices.push_back(texCoords[i * 2 + 1]);
        }
    }

    // Sets up the VAO and VBO
    void setupVAO()
    {
        glGenVertexArrays(1, &sphereVAO);
        glBindVertexArray(sphereVAO);

        // Create VBO
        unsigned int sphereVBO;
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        // Create EBO
        unsigned int sphereEBO;
        glGenBuffers(1, &sphereEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Configure vertex attributes
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

#endif /* SPHERE_WITH_TEXTURE_H */
