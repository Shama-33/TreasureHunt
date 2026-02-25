
#ifndef LEFT_FACE_TEXTURED_CUBE_H
#define LEFT_FACE_TEXTURED_CUBE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

class LeftFaceTexturedCube {
public:
    glm::vec3 ambient;    // Ambient material properties
    glm::vec3 diffuse;    // Diffuse material properties
    glm::vec3 specular;   // Specular material properties
    float shininess;      // Shininess factor
    unsigned int diffuseMap;  // Diffuse texture map
    unsigned int specularMap; // Specular texture map

    // Constructor with material and texture parameters
    LeftFaceTexturedCube( unsigned int dMap, unsigned int sMap, glm::vec3 amb=glm::vec3(1.0f), glm::vec3 diff = glm::vec3(1.0f), glm::vec3 spec = glm::vec3(1.0f), float shiny=30.2f)
        : ambient(amb), diffuse(diff), specular(spec), shininess(shiny), diffuseMap(dMap), specularMap(sMap)
    {
        setUpCubeVertexDataAndConfigureVertexAttribute();
    }

    ~LeftFaceTexturedCube()
    {
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteBuffers(1, &cubeEBO);
    }

    void drawCubeWithTexture(Shader& shader, glm::mat4 model = glm::mat4(1.0f))
    {
        shader.use();

        // Bind the diffuse texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->diffuseMap);
        shader.setInt("material.diffuse", 0);

        // Bind the specular texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->specularMap);
        shader.setInt("material.specular", 1);

        // Set material properties
        shader.setVec3("material.ambient", this->ambient);
        shader.setVec3("material.diffuseColor", this->diffuse);
        shader.setVec3("material.specularColor", this->specular);
        shader.setFloat("material.shininess", this->shininess);

        // Pass the transformation matrix to the shader
        shader.setMat4("model", model);

        // Render the cube
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

private:
    unsigned int cubeVAO, cubeVBO, cubeEBO;

    void setUpCubeVertexDataAndConfigureVertexAttribute()
    {
        // Vertex data for a cube, with texture only applied to the left face
        float cubeVertices[] = {
            // positions         // normals          // texture coords (only for left face)
            // Front face
            0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

            // Back face
            0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
            1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
            1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
            0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

            // Left face
            0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
            0.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
            0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            0.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

            // Right face
            1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
            1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            1.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

            // Top face
            0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
            1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
            0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,

            // Bottom face
            0.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            1.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        };

        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9, 10, 10, 11, 8, // Left face
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glGenBuffers(1, &cubeEBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};

#endif // LEFT_FACE_TEXTURED_CUBE_H
