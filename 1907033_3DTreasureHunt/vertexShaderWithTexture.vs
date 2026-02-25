#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 VertexBlendedColor; // Pass blended color to the fragment shader

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    vec3 diffuseColor; // Diffuse surface color
    vec3 specularColor; // Specular surface color
    float shininess;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float k_c;  // Attenuation factors
    float k_l;
    float k_q;
    bool enabled; // Toggle for light
};

#define NR_POINT_LIGHTS 2

uniform mat4 model;      // Model matrix
uniform mat4 view;       // View matrix
uniform mat4 projection; // Projection matrix

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

uniform bool enableVertexBlending; // Toggle for blending in vertex shader

vec3 CalcPointLight(PointLight light, vec3 N, vec3 V, vec3 fragPos);

void main()
{
    // Transformations
    FragPos = vec3(model * vec4(aPos, 1.0)); // Compute world position
    TexCoords = aTexCoords;

    // Normal transformation
    mat3 normalMatrix = mat3(transpose(inverse(model))); // Normal matrix for correct normal transformation
    Normal = normalize(normalMatrix * aNormal);

    vec3 lightingResult = vec3(0.0); // Initialize lighting result
    bool anyLightEnabled = false;   // Track if any light is enabled

    vec3 N = normalize(normalMatrix * aNormal);
    vec3 V = normalize(viewPos - FragPos);

    // Accumulate lighting contributions from all point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        if (pointLights[i].enabled) {
            anyLightEnabled = true;
            lightingResult += CalcPointLight(pointLights[i], N, V, FragPos);
        }
    }

    if (!anyLightEnabled) {
        // If no lights are enabled, set the object color to black
        lightingResult = vec3(0.0);
    }

    vec3 textureColor = vec3(texture(material.diffuse, TexCoords)); // Sample texture color

    // Blending logic
    if (enableVertexBlending) {
        VertexBlendedColor = mix(textureColor, lightingResult, 0.5); // Blend texture and lighting
    } else {
        VertexBlendedColor = textureColor * lightingResult; // Apply lighting to texture directly
    }

    gl_Position = projection * view * model * vec4(aPos, 1.0); // Final position in clip space
}

// Function to calculate lighting effect from a point light
vec3 CalcPointLight(PointLight light, vec3 N, vec3 V, vec3 fragPos)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 R = reflect(-L, N);

    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * (d * d));

    vec3 ambient = material.diffuseColor * light.ambient;
    vec3 diffuse = material.diffuseColor * max(dot(N, L), 0.0) * light.diffuse;
    vec3 specular = material.specularColor * pow(max(dot(V, R), 0.0), material.shininess) * light.specular;

    return (ambient + diffuse + specular) * attenuation;
}
