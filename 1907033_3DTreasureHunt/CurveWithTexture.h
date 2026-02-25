#ifndef CURVE_WITH_TEXTURE_H
#define CURVE_WITH_TEXTURE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include "shader.h"

class CurveWithTexture {
public:
    // Constructor
    CurveWithTexture(const std::string& vertexFilePath, const std::string& indexFilePath, unsigned int diffuseTex, unsigned int specularTex, float shiny)
        : diffuseTexture(diffuseTex), specularTexture(specularTex), shininess(shiny) {
        loadVertices(vertexFilePath);
        loadIndices(indexFilePath);
        setupVAO();
    }

    // Destructor
    ~CurveWithTexture() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    // Render the curve
    void render(Shader& shader, const glm::mat4& model, const glm::vec3& viewPos) {
        shader.use();

        // Pass transformation matrices
        shader.setMat4("model", model);

        // Pass material properties
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);
        shader.setInt("material.diffuse", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularTexture);
        shader.setInt("material.specular", 1);

        shader.setFloat("material.shininess", shininess);

        // Pass view position for lighting calculations
        shader.setVec3("viewPos", viewPos);

        // Bind VAO and render
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int VAO, VBO, EBO;
    unsigned int diffuseTexture, specularTexture;
    float shininess;

    std::vector<float> vertices;  // x, y, z, nx, ny, nz, u, v
    std::vector<unsigned int> indices;

    // Load vertices from a file
    void loadVertices(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open vertices file: " << filePath << std::endl;
            return;
        }

        float x, y, z, nx, ny, nz, u, v;
        while (file >> x >> y >> z >> nx >> ny >> nz >> u >> v) {
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
            vertices.push_back(u);
            vertices.push_back(v);
        }
        file.close();
    }

    // Load indices from a file
    void loadIndices(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open indices file: " << filePath << std::endl;
            return;
        }

        unsigned int i1, i2, i3;
        while (file >> i1 >> i2 >> i3) {
            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }
        file.close();
    }

    // Set up VAO, VBO, and EBO
    void setupVAO() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Load vertex data into VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Load index data into EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Configure vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coordinates
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};

#endif // CURVE_WITH_TEXTURE_H
