//
//  pointLight.h
//  test

//

#ifndef pointLight_h
#define pointLight_h

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

class PointLight {
public:
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float k_c;
    float k_l;
    float k_q;
    int lightNumber;

    bool isFlickering;  // Toggle for flickering
    float flickerIntensity;

    PointLight(float posX, float posY, float posZ, float ambR, float ambG, float ambB, float diffR, float diffG, float diffB, float specR, float specG, float specB, float constant, float linear, float quadratic, int num, bool flickering = false, float intensity = 0.2f) {

        position = glm::vec3(posX, posY, posZ);
        ambient = glm::vec3(ambR, ambG, ambB);
        diffuse = glm::vec3(diffR, diffG, diffB);
        specular = glm::vec3(specR, specG, specB);
        k_c = constant;
        k_l = linear;
        k_q = quadratic;
        lightNumber = num;
        isFlickering = flickering;
        flickerIntensity = intensity;
    }
    void setUpPointLight(Shader& lightingShader, float time)
    {
        int l = lightNumber - 1;
        std::string index = std::to_string(l);
        lightingShader.use();

        // Calculate flicker effect if enabled
        float flickerFactor = 1.0f; // Default no flicker
        if (isFlickering) {
            flickerFactor = 1.0f - flickerIntensity * (0.5f + 0.5f * sin(time * 20.0f + l + sin(time * 5.0f) * 2.0f));
        }


        // Apply flicker factor to light components
        glm::vec3 flickerAmbient = ambientOn * ambient * flickerFactor;
        glm::vec3 flickerDiffuse = diffuseOn * diffuse * flickerFactor;
        glm::vec3 flickerSpecular = specularOn * specular * flickerFactor;

        if (lightNumber == 1) {
            lightingShader.setVec3("pointLights[0].position", position);
            lightingShader.setVec3("pointLights[0].ambient", ambientOn * flickerAmbient);
            lightingShader.setVec3("pointLights[0].diffuse", diffuseOn * flickerDiffuse);
            lightingShader.setVec3("pointLights[0].specular", specularOn * flickerSpecular);
            lightingShader.setFloat("pointLights[0].k_c", k_c);
            lightingShader.setFloat("pointLights[0].k_l", k_l);
            lightingShader.setFloat("pointLights[0].k_q", k_q);
        }

        else if (lightNumber == 2)
        {
            lightingShader.setVec3("pointLights[1].position", position);
            lightingShader.setVec3("pointLights[1].ambient", ambientOn * flickerSpecular);
            lightingShader.setVec3("pointLights[1].diffuse", diffuseOn * flickerSpecular);
            lightingShader.setVec3("pointLights[1].specular", specularOn * flickerSpecular);
            lightingShader.setFloat("pointLights[1].k_c", k_c);
            lightingShader.setFloat("pointLights[1].k_l", k_l);
            lightingShader.setFloat("pointLights[1].k_q", k_q);
        }

    }
    void setUpPointLight2(Shader& lightingShader, bool isEnabled, float time) {
        int l = lightNumber - 1;
        std::string index = std::to_string(l);
        lightingShader.use();
        float flickerFactor = 1.0f; // Default no flicker
        if (isFlickering) {
            flickerFactor = 1.0f - flickerIntensity * (0.5f + 0.5f * sin(time * 10.0f + l));
        }
        // Apply flicker factor to light components
        glm::vec3 flickerAmbient = ambientOn * ambient * flickerFactor;
        glm::vec3 flickerDiffuse = diffuseOn * diffuse * flickerFactor;
        glm::vec3 flickerSpecular = specularOn * specular * flickerFactor;
        std::string prefix = "pointLights[" + std::to_string(lightNumber - 1) + "]";

        lightingShader.setVec3(prefix + ".position", position);

        if (isEnabled) {
            lightingShader.setVec3(prefix + ".ambient", flickerAmbient);
            lightingShader.setVec3(prefix + ".diffuse", flickerDiffuse);
            lightingShader.setVec3(prefix + ".specular", flickerSpecular);
        }
        else {
            lightingShader.setVec3(prefix + ".ambient", glm::vec3(0.0f));
            lightingShader.setVec3(prefix + ".diffuse", glm::vec3(0.0f));
            lightingShader.setVec3(prefix + ".specular", glm::vec3(0.0f));
        }

        lightingShader.setFloat(prefix + ".k_c", k_c);
        lightingShader.setFloat(prefix + ".k_l", k_l);
        lightingShader.setFloat(prefix + ".k_q", k_q);
    }


    void turnOff()
    {
        ambientOn = 0.0;
        diffuseOn = 0.0;
        specularOn = 0.0;
    }
    void turnOn()
    {
        ambientOn = 1.0;
        diffuseOn = 1.0;
        specularOn = 1.0;
    }
    void turnAmbientOn() {
        ambientOn = 1.0;
    }

    void turnAmbientOff() {
        ambientOn = 0.0;
    }

    void turnDiffuseOn() {
        diffuseOn = 1.0;
    }

    void turnDiffuseOff() {
        diffuseOn = 0.0;
    }

    void turnSpecularOn() {
        specularOn = 1.0;
    }

    void turnSpecularOff() {
        specularOn = 0.0;
    }

private:
    float ambientOn = 1.0;
    float diffuseOn = 1.0;
    float specularOn = 1.0;
};

#endif /* pointLight_h */