#version 330 core
out vec4 FragColor;

in vec3 VertexBlendedColor; // Received from the vertex shader

void main()
{
    FragColor = vec4(VertexBlendedColor, 1.0);
}
