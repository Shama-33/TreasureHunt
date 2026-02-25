#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    vec3 ambient; // Material surface ambient color
    vec3 diffuseColor; // Material surface diffuse color
    vec3 specularColor; // Material surface specular color
    float shininess;
};

struct PointLight {
    vec3 position;
    
    float k_c;  // attenuation factors
    float k_l;  // attenuation factors
    float k_q;  // attenuation factors
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool isFlickering;    // Toggle for flickering
    float flickerIntensity;
};

#define NR_POINT_LIGHTS 2

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform bool pointLightEnabled[NR_POINT_LIGHTS]; // Light state toggles
uniform Material material;
uniform bool enableBlending; // New uniform to toggle blending
uniform float time; // Time for flickering effect


// Function prototypes
vec3 CalcPointLight(Material material, PointLight light, bool isEnabled, vec3 N, vec3 fragPos, vec3 V);


void main()
{
    // Normalize properties
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0); // Initialize lighting result

    // Add contributions from enabled point lights
    bool anyLightEnabled = false;
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        if (pointLightEnabled[i]) {
            anyLightEnabled = true;
            result += CalcPointLight(material, pointLights[i], pointLightEnabled[i], N, FragPos, V);
        }

    }
    

    // Sample texture color
    vec3 textureColor = vec3(texture(material.diffuse, TexCoords));

    if (!anyLightEnabled) {
        // If no lights are enabled, make everything black
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else if (enableBlending) {
        // Blend logic: combine texture with material's surface color and lighting
        vec3 surfaceColor = material.ambient + material.diffuseColor * result + material.specularColor;
        FragColor = vec4(textureColor * surfaceColor, 1.0); 
    } else {
        // Use plain texture multiplied by lighting result
        FragColor = vec4(textureColor , 1.0);
    }
}

// Calculate color contribution from a point light
vec3 CalcPointLight(Material material, PointLight light, bool isEnabled, vec3 N, vec3 fragPos, vec3 V)
{
    if (!isEnabled) return vec3(0.0); // Skip calculation if light is off

    vec3 L = normalize(light.position - fragPos);
    vec3 R = reflect(-L, N);
    
    // Attenuation
    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * (d * d));
    
    // Flickering logic
    float flickerFactor = 1.0; // Default no flicker
  if (light.isFlickering) {
    flickerFactor = 1.0f - light.flickerIntensity * (0.5f + 0.5f * sin(time * 20.0f  + sin(time * 5.0f) * 2.0f));
}

    // Lighting calculations
    vec3 ambient = material.ambient * light.ambient* flickerFactor;;
    vec3 diffuse = material.diffuseColor * max(dot(N, L), 0.0) * light.diffuse* flickerFactor;;
    vec3 specular = material.specularColor * pow(max(dot(V, R), 0.0), material.shininess) * light.specular* flickerFactor;;
    
    // Apply attenuation
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}
// Calculate lighting contribution from a directional light