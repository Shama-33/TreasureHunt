//
//  main.cpp
//  3D Object Drawing

//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "spotLight.h"
#include "directionalLight.h"
#include "sphere.h"
#include "cone.h"
#include "cylinder.h"
#include "hemisphere.h"
#include "cube.h"
#include "chest.h"
#include "stb_image.h"
#include "sphereWithTexture.h"
#include "hemiWithTex.h"
#include "coneWithTexture.h"
#include "cylinderWithTexture.h"

#include <iostream>

using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b, float a);
void bed(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 alTogether);
// draw object functions
void drawTabChairBook(Shader& shaderProgram, unsigned int VAO, glm::mat4 parentTrans);
void drawWallsAndFloorWithCarpet(Shader& shaderProgram, unsigned int VAO, glm::mat4 model);
void drawWallsAndFloorWithCarpet2(Shader& shaderProgram, unsigned int VAO);
void drawFan(Shader& shaderProgram, unsigned int VAO, glm::mat4 parentTrans, bool fanOn, float& r);
void classroom(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 alTogether);
glm::mat4 customPerspective(float fovRadians, float aspect, float near, float far);
void drawAlmirah(Shader& shaderProgram, unsigned int VAO, glm::mat4 model);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);
unsigned int loadTexture2(const char* path, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
bool isPointInsideSphere(glm::vec3 rayOrigin, glm::vec3 sphereCenter, float boundingRadius);
void onMouseClick(double mouseX, double mouseY, int screenWidth, int screenHeight, glm::vec3 cameraPos, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
bool intersectRayWithSphere(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 sphereCenter, float radius);
void renderNextRoom(Shader& shader);

enum ObjectType { SPHERETEX, CHEST, CYLINDERTEX, HEMISPHERETEX };
struct Object {
    glm::vec3 position;
    glm::vec3 originalPosition;
    glm::vec3 scale;
    glm::vec3 rotation;
    float boundingRadius;
    bool hasKey;
    ObjectType type;
    void* renderable;
    unsigned int textureDiffuse;
    unsigned int textureSpecular;

    void draw(Shader& shader, glm::vec3 viewPos);
};


std::vector<Object> objects; // Global vector for all objects
void Object::draw(Shader& shader, glm::vec3 viewPos) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    shader.setMat4("model", model);

    if (type == SPHERETEX) {
        static_cast<SphereWithTexture*>(renderable)->drawSphere(shader, model, viewPos);
    }
    else if (type == CHEST) {
        static_cast<Chest*>(renderable)->drawCubeWithTwoTextures(shader, model);
    }
    else if (type == CYLINDERTEX) {
        static_cast<CylinderWithTexture*>(renderable)->drawCylinder(shader, model, viewPos);
    }
    else if (type == HEMISPHERETEX) {
        static_cast<HemiWithTex*>(renderable)->drawSphere(shader, model, viewPos);
    }
}
void initializeObjects(SphereWithTexture& globe, CylinderWithTexture& stoneObstacle, Chest& chestCube, HemiWithTex& hemiSphere, HemiWithTex& ghostMatha) {
    objects.push_back({
        glm::vec3(-3.5f, 1.2f, -3.0f), // Position
        glm::vec3(-3.5f, 1.2f, -3.0f), // Original position
        glm::vec3(0.4f, 0.4f, 0.4f),   // Scale
        glm::vec3(0.0f, 45.0f, 0.0f),  // Rotation
        0.4f,                          // Bounding radius (already scaled)
        false,                         // Does not have the key
        SPHERETEX,                     // Object type
        &globe                         // Pointer to the sphere object
        });

    objects.push_back({
        glm::vec3(1.5f, -0.5f, -2.3f), // Position
        glm::vec3(1.5f, -0.5f, -2.3f), // Original position
        glm::vec3(2.5f, 0.9f, 1.2f),   // Scale
        glm::vec3(0.0f, 180.0f, 0.0f), // Rotation
        1.56f , // Properly scaled bounding radius
        true,                          // This object has the key
        CHEST,                         // Object type
        &chestCube                     // Pointer to the chest object
        });

    objects.push_back({
        glm::vec3(-2.0f, 0.3f, -1.0f), // Position
        glm::vec3(-2.0f, 0.3f, -1.0f), // Original position
        glm::vec3(0.75f, 0.75f, 0.75f),// Scale
        glm::vec3(90.0f, 0.0f, 0.0f),  // Rotation
        1.06f , // Properly scaled bounding radius
        false,                         // Does not have the key
        CYLINDERTEX,                   // Object type
        &stoneObstacle                 // Pointer to the cylinder object
        });
    objects.push_back({
        glm::vec3(-0.5f, -0.5f, -1.0f), // Position
        glm::vec3(-0.5f, -0.5f, -1.0f), // Original position
        glm::vec3(0.75f, 0.75f, 0.75f),// Scale
        glm::vec3(0.0f, 0.0f, 0.0f),  // Rotation
        0.75f , // Properly scaled bounding radius
        false,                         // Does not have the key
        HEMISPHERETEX,                   // Object type
        &hemiSphere                 // Pointer to the cylinder object
        });

    objects.push_back({
        glm::vec3(0.7f, 0.5f, 0.5f), // Position
        glm::vec3(0.7f, 0.5f, 0.5f), // Original position
        glm::vec3(0.50f, 0.80f, 0.50f),// Scale
        glm::vec3(0.0f, 0.0f, 0.0f),  // Rotation
        0.80f , // Properly scaled bounding radius
        false,                         // Does not have the key
        HEMISPHERETEX,                   // Object type
        &ghostMatha                 // Pointer to the cylinder object
        });
}

glm::vec3 getTransformedPosition(const Object& obj);
void drawBoundingSphere(Shader& shader, Object& obj) {
    Sphere sphere;
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), obj.position), glm::vec3(obj.boundingRadius));
    shader.setMat4("model", model);
    //sphere.drawSphere(shader,model); // Assume you have a wireframe sphere drawing utility
}

glm::vec3 calculateRayFromMouse(double mouseX, double mouseY, int screenWidth, int screenHeight, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
    // Convert mouse position to Normalized Device Coordinates (NDC)
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight; // Flip Y to match OpenGL's coordinate system
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f); // Ray in clip space

    // Transform to eye space
    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Set w=0 for direction vector

    // Transform to world space
    glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
    rayWorld = glm::normalize(rayWorld);

    std::cout << "Mouse Ray: (" << rayWorld.x << ", " << rayWorld.y << ", " << rayWorld.z << ")" << std::endl;

    return rayWorld;
}

bool intersectRayWithObject(glm::vec3 rayOrigin, glm::vec3 rayDirection, Object& object) {
    glm::vec3 toSphere = object.position - rayOrigin;
    float t = glm::dot(toSphere, rayDirection);
    glm::vec3 closestPoint = rayOrigin + t * rayDirection;
    float distanceSquared = glm::dot(closestPoint - object.position, closestPoint - object.position);
    return distanceSquared <= object.boundingRadius * object.boundingRadius;
}





// settings
const unsigned int SCR_WIDTH = 2000;
const unsigned int SCR_HEIGHT = 1200;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;

// camera
Camera camera(glm::vec3(0.0f, 1.1f, 5.2f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 2.0, eyeZ = 7.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);


// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(1.40f,  1.50f,  -0.5f),
    //glm::vec3(1.40f,  1.50f,  -0.5f)
    glm::vec3(-4.0f,  1.5f,  -0.5f)

    //glm::vec3(-4.0f,  1.5f,  -0.5f)

};

//glm::vec3 spotLightPos = glm::vec3(-0.5, 1, -0.5);
//glm::vec3 spotLightDir = glm::vec3(0, -1, 0);


//glm::vec3(-0.5, 1, -0.5)


PointLight pointlight1(

    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    0.3f, 0.3f, 0.3f,     // ambient
    1.0f, 1.0f, 1.0f,     // diffuse
    0.1f, 0.1f, 0.1f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1       // light number
);
PointLight pointlight2(

    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,  // position
    0.3f, 0.3f, 0.3f,       // ambient
    1.0f, 1.0f, 1.0f,     // diffuse
    0.1f, 0.1f, 0.1f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    2       // light number
);

SpotLight spotlight(

    -1.5, 2, -1.0,  //pos
    .2, .2, .2, //ambient
    .8f, .8f, .8f, //diffuse
    0.1f, 0.1f, 0.1f, //specular
    0, -4, 1.7, //direction
    glm::cos(glm::radians(8.5f)), //cos_theta
    1.0f, //k_c
    0.09f, //k_l
    0.032f //k_q

);

DirectionalLight directionallight(


    //.2, .2, .2, //amb
    //.8f, .8f, .8f, //diff
    //1.0f, 1.0f, 1.0f //spec
    0.2f, 0.2f, 0.2f,     // ambient
    1.0f, 1.0f, 1.0f,     // diffuse
    0.1f, 0.1f, 0.1f,       // specular
    3.0f, 3.0f, 0.0f //dir

);


// light settings
bool onOffToggle = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;

bool pointOn1 = true;
bool pointOn2 = true;
bool directionalOn = true;
bool spotOn = true;
bool blendingEnabled = false;
bool vertexBlendingEnabled = false;
// State variables to track key presses
bool bKeyPressed = false; // Track the state of the B key
bool vKeyPressed = false; // Track the state of the V key
bool key3Pressed = false;
bool key2Pressed = false;

float r1 = 0.0, r2 = 0.0, r3 = 0.0, r4 = 0.0;

bool fanOn1 = false, fanOn2 = false, fanOn3 = false, fanOn4 = false;


// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;
glm::mat4 projection;
bool mouseClickDetected = false; // Detects if a mouse click has occurred
double mouseX = 0.0;             // Stores the x-coordinate of the mouse
double mouseY = 0.0;             // Stores the y-coordinate of the mouse
bool doorOpened = false;     // Tracks if the door is opened
int currentRoom = 0;         // Tracks the current room (0 = initial room, 1 = next room)
bool keyFound = false; // Whether the key is found
float keyFoundTime = 0.0f; // Time when the key was found
bool transitioning = false; // Whether we're in the middle of transitioning to the next room
int score;

Object* selectedObject = nullptr; // Pointer to the currently selected object

//arrow shooting game arrangement
struct Arrow {
    glm::vec3 position;     // Current position of the arrow
    glm::vec3 direction;    // Direction of the arrow
    float speed;            // Speed of the arrow
    bool isFlying;          // If the arrow is currently flying

    void draw(Shader& shader);  // Function to render the arrow
    void update(float deltaTime); // Update arrow position
};
Arrow arrow; // Single arrow object
unsigned int trajectoryVAO, trajectoryVBO;
// Arrow Rendering Function
void Arrow::draw(Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 1.0f)); // Adjust arrow size
    //model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));

    shader.setMat4("model", model);

    // Updated: Added a generic texture for the arrow
    unsigned int arrowTexture = loadTexture2("stone.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    CylinderWithTexture arrowTex(1.0f, 1.0f, 2.0f, 50, 20,
        glm::vec3(1.0f, 1.0f, 1.0f),  // Ambient color
        glm::vec3(1.0f, 1.0f, 1.0f),  // Diffuse color
        glm::vec3(0.2f, 0.2f, 0.2f),  // Specular color
        32.0f,                        // Shininess
        arrowTexture,                 // Diffuse texture
        arrowTexture                  // Specular texture
    );


    // Draw the arrow cylinder
    arrowTex.drawCylinder(shader, model, camera.Position);

}

// Arrow Update Function
void Arrow::update(float deltaTime) {
    if (isFlying) {
        // Updated: Improved out-of-bounds check for better resetting
        position += direction * speed * deltaTime;

        if (position.z < -50.0f || position.y < -5.0f || position.x < -50.0f || position.x > 50.0f) {
            isFlying = false; // Reset arrow if out of bounds
            position = glm::vec3(0.0f, 1.1f, 5.2f); // Reset arrow to its initial position
            std::cout << "Arrow missed. Resetting to initial position." << std::endl;
        }
    }
}

// Target Structure
struct Target {
    glm::vec3 position;   // Position of the target
    float radius;         // Radius of the target
    bool isHit;           // Whether the target is hit
    float speed;          // Speed of the target's movement
    void draw(Shader& shader);  // Render the target
    void update(float deltaTime); // Move the target

    // Constructor
    Target(glm::vec3 pos, float rad, bool hit, float spd)
        : position(pos), radius(rad), isHit(hit), speed(spd) {}
};
std::vector<Target> targets; // List of targets

// Updated initializeNextRoom Function
void initializeNextRoom() {
    // Arrow Initialization


    arrow.position = glm::vec3(0.0f, 1.1f, 5.2f);
    arrow.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    arrow.speed = 10.0f;
    arrow.isFlying = false;

    // Updated: Added a variety of target positions
    targets.push_back(Target(glm::vec3(-7.0f, 0.0f, -20.0f), 0.5f, false, 0.0f));
    targets.push_back(Target(glm::vec3(-2.0f, 0.0f, -20.0f), 0.5f, false, 0.0f));
    targets.push_back(Target(glm::vec3(3.0f, 0.0f, -20.0f), 0.5f, false, 0.0f));
}

// Collision Check
void checkCollisions(Arrow& arrow) {
    for (Target& target : targets) {
        if (!target.isHit) {
            glm::vec3 diff = arrow.position - target.position;
            diff = diff + 0.3f;
            float distanceSquared = glm::dot(diff, diff);

            if (distanceSquared <= target.radius * target.radius) { // Check if arrow touches the target
                target.isHit = true;
                arrow.isFlying = false; // Stop the arrow
                arrow.position = glm::vec3(0.0f, 1.1f, 5.2f); // Reset arrow position
                score += 10;
                std::cout << "Target hit! Score: " << score << std::endl;
                break; // Exit after the first hit
            }
        }
    }
}




// Rendering the Trajectory
void renderTrajectory(Shader& shader, const std::vector<glm::vec3>& trajectoryPoints) {
    shader.use();
    glBindVertexArray(trajectoryVAO);
    glDrawArrays(GL_LINE_STRIP, 0, trajectoryPoints.size());
    glBindVertexArray(0);
}

// Updated Target Drawing Function
void Target::draw(Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius * 2.0f));

    shader.setMat4("model", model);

    unsigned int targetTexture = loadTexture2("stone.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    SphereWithTexture targetSphere(1.0f, 36, 18,
        glm::vec3(0.1f, 0.1f, 0.8f),
        glm::vec3(0.2f, 0.2f, 0.9f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        32.0f,
        targetTexture,
        targetTexture
    );

    targetSphere.drawSphere(shader, model, camera.Position);
}
void Target::update(float deltaTime) {
    // Simple back-and-forth movement
    position.x += sin(glfwGetTime()) * speed * deltaTime;
    //position.x = position.x;
}



void onArrowShoot(Arrow& arrow, double mouseX, double mouseY, int screenWidth, int screenHeight, glm::vec3 cameraPos, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
    if (!arrow.isFlying) {
        arrow.isFlying = true;

        // Calculate the direction of the arrow based on mouse click
        glm::vec3 rayDirection = calculateRayFromMouse(mouseX, mouseY, screenWidth, screenHeight, viewMatrix, projectionMatrix);
        arrow.direction = glm::normalize(rayDirection);

        // Start the arrow from the camera position
        arrow.position = cameraPos;
    }
}


void renderScore(Shader& shader) {
    std::cout << "Score: " << score << std::endl;

}
//show trajectory
std::vector<glm::vec3> generateTrajectoryPoints(glm::vec3 start, glm::vec3 direction, int numPoints, float stepSize) {
    std::vector<glm::vec3> points;
    for (int i = 0; i < numPoints; ++i) {
        points.push_back(start + direction * (float)i * stepSize);
    }
    return points;
}


void setupTrajectory() {
    glGenVertexArrays(1, &trajectoryVAO);
    glGenBuffers(1, &trajectoryVBO);

    glBindVertexArray(trajectoryVAO);
    glBindBuffer(GL_ARRAY_BUFFER, trajectoryVBO);

    // Enable vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void updateTrajectoryBuffer(const std::vector<glm::vec3>& trajectoryPoints) {
    glBindBuffer(GL_ARRAY_BUFFER, trajectoryVBO); // trajectoryVBO is the VBO ID for trajectory
    glBufferData(GL_ARRAY_BUFFER, trajectoryPoints.size() * sizeof(glm::vec3), trajectoryPoints.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
float gameStartTime = glfwGetTime(); // Record the start time
bool gameOver = false;
bool allTargetsHit = false;

void checkGameStatus() {
    if (!gameOver) {

        for (const Target& target : targets) {
            if (!target.isHit) {
                allTargetsHit = false;
                break;
            }
        }

        if (score == 30) {
            gameOver = true; // End game if all targets are hit
            std::cout << "All targets hit! You win!" << std::endl;
            allTargetsHit = true;
        }

        // Check if the game has timed out
        float elapsedTime = glfwGetTime() - gameStartTime;
        if (elapsedTime > 30.0f && !allTargetsHit) { // 30 seconds timeout
            gameOver = true;
            std::cout << "Game over! You ran out of time!" << std::endl;
        }
    }
}













int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    //Shader lightingShader("vertexShaderForGouraudShading.vs", "fragmentShaderForGouraudShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float cube_vertices[] = {
        // Positions          // Normals
        // Front face
        0.0f, 0.0f, 0.5f,     0.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f,     0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f,     0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.5f,     0.0f, 0.0f, 1.0f,

        // Back face
        0.0f, 0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
        0.5f, 0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, 0.0f,     0.0f, 0.0f, -1.0f,
        0.0f, 0.5f, 0.0f,     0.0f, 0.0f, -1.0f,

        // Left face
        0.0f, 0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f,     -1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f,     -1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f,     -1.0f, 0.0f, 0.0f,

        // Right face
        0.5f, 0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.5f,     1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f,     1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f,     1.0f, 0.0f, 0.0f,

        // Top face
        0.0f, 0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,

        // Bottom face
        0.0f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
        0.5f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
        0.5f, 0.0f, 0.5f,     0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 0.5f,     0.0f, -1.0f, 0.0f,
    };


    /*float cube_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };*/
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    Sphere sphere = Sphere();

    Hemisphere hemi = Hemisphere();

    Cone cone = Cone();
    Cylinder cylinder = Cylinder();
    // Create a cylinder (as the handle)
    float baseRadius = 0.06f;   // Smaller base radius for the handle
    float topRadius = 0.06f;    // Same as base to make it cylindrical
    float height = 0.25f;       // Length of the handle
    int sectorCount = 20;      // Smoothness of the cylinder
    int stackCount = 10;       // Stack divisions
    glm::vec3 ambientColor(0.2f, 0.5f, 0.7f); // Blueish ambient color
    glm::vec3 diffuseColor(0.2f, 0.5f, 0.7f); // Blueish diffuse color
    glm::vec3 specularColor(0.1f, 0.1f, 0.1f); // Bright specular highlights
    float shininess = 32.0f;   // Shininess factor for the handle

    Cylinder handle(baseRadius, topRadius, height, sectorCount, stackCount,
        ambientColor, diffuseColor, specularColor, shininess);

    //declare shader for texture
    Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    //Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    Shader fragmentBlendingShader("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    Shader vertexBlendingShader("vertexShaderWithTexture.vs", "fragmentShaderWithoutTexture.fs");



    //chest
    glm::vec3 ambient(0.5f, 0.3f, 0.2f);
    glm::vec3 diffuse(0.5f, 0.3f, 0.2f);
    glm::vec3 specular(0.5f, 0.5f, 0.5f);




    string frontTexturePath = "front_face.png";  // Front face texture (e.g., with a lock)
    string otherFacesTexturePath = "container2.png";  // Other faces texture (e.g., plain wood)
    unsigned int frontTexture = loadTexture(frontTexturePath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int otherTexture = loadTexture(otherFacesTexturePath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Chest chestCube = Chest(ambient, diffuse, specular, frontTexture, otherTexture, 32.0f);


    // string globee = "globe.jpg";


    float globeRadius = 1.0f;                 // Radius of the globe
    int globeSectorCount = 36;               // Smoothness of the sphere
    int globeStackCount = 18;
    glm::vec3 globeAmbient(0.1f, 0.1f, 0.8f); // Blueish ambient color
    glm::vec3 globeDiffuse(0.2f, 0.2f, 0.9f); // Slightly brighter diffuse blue
    glm::vec3 globeSpecular(0.1f, 0.1f, 0.1f); // Bright white specular highlights
    float globeShininess = 32.0f;            // Shininess for the globe

    //Sphere globe(globeRadius, globeSectorCount, globeStackCount,
                 //globeAmbient, globeDiffuse, globeSpecular, globeShininess);


    // Load textures for the globe
    unsigned int globeDiffuseMap = loadTexture2("globe.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int globeSpecularMap = loadTexture2("earth_specular.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);


    SphereWithTexture globe(globeRadius, globeSectorCount, globeStackCount,
        globeAmbient, globeDiffuse, globeSpecular,
        globeShininess, globeDiffuseMap, globeSpecularMap);

    //ghost

    // Load hemisphere textures
    unsigned int hemiDiffuseMap = loadTexture2("ghost.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    //unsigned int hemiSpecularMap = loadTexture2("hemi_specular.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Create the hemisphere object
    HemiWithTex ghostmatha(1.0f, 36, 18,
        glm::vec3(1.0f, 0.0f, 0.0f), // Ambient color
        glm::vec3(1.0f, 0.0f, 0.0f), // Diffuse color
        glm::vec3(0.1f, 0.1f, 0.1f), // Specular color
        32.0f,                       // Shininess
        hemiDiffuseMap,              // Diffuse texture
        hemiDiffuseMap              // Specular texture
    );

    unsigned int hemiDiffuseMap2 = loadTexture2("gold.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    HemiWithTex gold(1.0f, 36, 18,
        glm::vec3(0.2f, 0.15f, 0.05f), // Ambient color (a soft golden base color)
        glm::vec3(0.8f, 0.6f, 0.2f),  // Diffuse color (the bright golden shine under light)
        glm::vec3(0.9f, 0.8f, 0.5f),  // Specular color (highlights with a golden tint)
        32.0f,                       // Shininess
        hemiDiffuseMap2,              // Diffuse texture
        hemiDiffuseMap2              // Specular texture
    );

    unsigned int coneMap = loadTexture2("ghosttail.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    ConeWithTexture ghostTail(1.0f, 2.0, 20,
        glm::vec3(1.0f, 0.7f, 0.7f), // Ambient color 
        glm::vec3(1.0f, 0.7f, 0.7f),  // Diffuse color 
        glm::vec3(0.1f, 0.1f, 0.1f),  // Specular color 
        32.0f,                       // Shininess
        coneMap,              // Diffuse texture
        coneMap              // Specular texture
    );

    unsigned int stoneMap = loadTexture2("stone.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    CylinderWithTexture stoneObstacle(1.0f, 1.0f, 2.0f, 50, 20,
        //glm::vec3(0.5f, 0.5f, 0.5f), // Ambient color (medium gray)
        //glm::vec3(0.6f, 0.6f, 0.6f), 
        glm::vec3(1.0f, 1.0f, 1.0f), // Ambient color (medium gray)
        glm::vec3(1.0f, 1.0f, 1.0f),
        // // Diffuse color (slightly lighter gray)
        glm::vec3(0.2f, 0.2f, 0.2f),  // Specular color (low specular reflection)

        32.0f,                       // Shininess
        stoneMap,              // Diffuse texture
        stoneMap              // Specular texture
    );
    initializeObjects(globe, stoneObstacle, chestCube, gold, ghostmatha);









    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    //ourShader.use();
    //lightingShader.use();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set background color to black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear screen and depth buffer




        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // point light 1
        pointlight1.setUpPointLight(lightingShader);
        // point light 2
        pointlight2.setUpPointLight(lightingShader);

        spotlight.setUpSpotLight(lightingShader);
        directionallight.setUpDirectionalLight(lightingShader);








        // activate shader
        lightingShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        // // Replace glm::perspective with manual calculation
        float fov = glm::radians(45.0f); // Convert degrees to radians
        float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
        float near = 0.1f;
        float far = 100.0f;

        projection = customPerspective(fov, aspect, near, far);

        lightingShader.setMat4("projection", projection);

        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        lightingShader.setMat4("projection", projection);

        // camera/view transformation
        //glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 view = basic_camera.createViewMatrix();
        lightingShader.setMat4("view", view);
        if (currentRoom == 0) {

            // Modelling Transformation
            glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            //glBindVertexArray(cubeVAO);
            //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            //glDrawArrays(GL_TRIANGLES, 0, 36);

            classroom(cubeVAO, lightingShader, model);

            //bed(cubeVAO, lightingShader, model);
            //glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix;

            //sphere
            //glm::mat4 modelForSphere = glm::mat4(1.0f);

            //glm::mat4 modelForSphere = glm::mat4(1.0f);

            //globe
           /* translateMatrix = glm::translate(model, glm::vec3(-3.68f, 0.95f, -3.0f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.20f, 0.20f, 0.20f));

            globe.drawSphere(lightingShader, scaleMatrix);*/

            //globe handle

            //glm::mat4 modelForCylinder = glm::mat4(1.0f);
            translateMatrix = glm::translate(model, glm::vec3(-3.5f, 0.7f, -3.0f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.25f, 0.25f, 0.25f));

            handle.drawCylinder(lightingShader, translateMatrix);

            //cylinder
            translateMatrix = glm::translate(model, glm::vec3(-1.80f, 0.1f, -1.8f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.55f, 0.55f, 0.55f));
            rotateYMatrix = glm::rotate(scaleMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            //cylinder.drawCylinder(lightingShader, rotateYMatrix);

            //hemisphere
            /*translateMatrix = glm::translate(model, glm::vec3(0.0f, -0.5f, -1.0f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.75f, 0.75f, 0.75f));

            hemi.drawHemiSphere(lightingShader, scaleMatrix);*/



            glm::mat4 modelForCone = glm::mat4(1.0f);
            translateMatrix = glm::translate(model, glm::vec3(3.5f, 1.25f, 0.5f));
            //scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.25f, 0.25f, 0.25f));

            //cone.drawCone(lightingShader, translateMatrix);

            glm::mat4 modelForCylinder = glm::mat4(1.0f);
            translateMatrix = glm::translate(model, glm::vec3(2.0f, 1.0f, 0.5f));
            //scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.25f, 0.25f, 0.25f));

            //cylinder.drawCylinder(lightingShader, translateMatrix);





            // also draw the lamp object(s)
            ourShader.use();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);

            // we now draw as many light bulbs as we have point lights.
            glBindVertexArray(lightCubeVAO);
            for (unsigned int i = 0; i < 2; i++)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                ourShader.setMat4("model", model);
                ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                //glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // be sure to activate shader when setting uniforms/drawing objects
           // Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
            lightingShaderWithTexture.use();
            /*if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {

                blendingEnabled = !blendingEnabled;
                lightingShaderWithTexture.setBool("enableBlending", blendingEnabled);
            }*/
            // Check for key presses and update blending modes
            // Handle toggling for fragment blending (B key)
            if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
                if (!bKeyPressed) { // If B was not previously pressed
                    blendingEnabled = !blendingEnabled; // Toggle blending
                    vertexBlendingEnabled = false; // Disable vertex blending
                    bKeyPressed = true; // Mark B as pressed
                }
            }
            else {
                bKeyPressed = false; // Reset when B is released
            }

            // Handle toggling for vertex blending (V key)
            if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
                if (!vKeyPressed) { // If V was not previously pressed
                    vertexBlendingEnabled = !vertexBlendingEnabled; // Toggle vertex blending
                    blendingEnabled = false; // Disable fragment blending
                    vKeyPressed = true; // Mark V as pressed
                }
            }
            else {
                vKeyPressed = false; // Reset when V is released
            }

            // Determine which shader to use based on blending mode
            Shader* activeShader;


            if (blendingEnabled) {
                activeShader = &fragmentBlendingShader;
                fragmentBlendingShader.use();
                fragmentBlendingShader.setBool("enableBlending", true);


            }
            else if (vertexBlendingEnabled) {
                activeShader = &vertexBlendingShader;
                vertexBlendingShader.use();
                vertexBlendingShader.setBool("enableVertexBlending", true);
            }
            else {
                activeShader = &lightingShaderWithTexture;
                lightingShaderWithTexture.use();

            }
            activeShader->setBool("pointLightEnabled[0]", pointOn1);
            activeShader->setBool("pointLightEnabled[1]", pointOn2);
            //activeShader->setVec3("globalAmbientLight", glm::vec3(0.8f, 0.8f, 0.8f));

            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !key2Pressed) {

                pointOn1 = !pointOn1; // Toggle light state
                key2Pressed = true;

            }

            /*if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
                key2Pressed = false;
            }*/

            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !key3Pressed) {
                pointOn2 = !pointOn2;


                key3Pressed = true; // Prevent multiple presses

            }

            /*if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
                key3Pressed = false;
            }*/


            // Set common uniforms for the active shader
            activeShader->setVec3("viewPos", camera.Position);
            activeShader->setMat4("projection", projection);
            activeShader->setMat4("view", view);

            // Configure lights for the active shader
            pointlight1.setUpPointLight2(*activeShader, pointOn1);
            pointlight2.setUpPointLight2(*activeShader, pointOn2);

            glm::vec3 viewPos = camera.Position; // Assuming you have a camera object
            unsigned int textureDiffuse = globeDiffuseMap; // Replace with actual diffuse texture
            unsigned int textureSpecular = globeSpecularMap; // Replace with actual specular texture


            // Render objects using the active shader
            // 
            for (Object& obj : objects) {
                obj.draw(*activeShader, viewPos);
                drawBoundingSphere(*activeShader, obj);

            }
            // Chest
           /* glm::mat4 modelMatrixForChest = glm::mat4(1.0f);
            modelMatrixForChest = glm::translate(modelMatrixForChest, glm::vec3(1.5f, -0.5f, -2.3f));
            modelMatrixForChest = glm::scale(modelMatrixForChest, glm::vec3(2.5f, 0.9f, 1.2f));
            modelMatrixForChest = glm::rotate(modelMatrixForChest, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            chestCube.drawCubeWithTwoTextures(*activeShader, modelMatrixForChest);

            // Globe
            glm::mat4 modelMatrixForGlobe = glm::mat4(1.0f);
            modelMatrixForGlobe = glm::translate(modelMatrixForGlobe, glm::vec3(-3.5f, 1.2f, -3.0f));
            modelMatrixForGlobe = glm::scale(modelMatrixForGlobe, glm::vec3(0.4f, 0.4f, 0.4f));
            globe.drawSphere(*activeShader, modelMatrixForGlobe, camera.Position);

            // Stone Obstacle
            glm::mat4 modelMatrixForStone = glm::mat4(1.0f);
            modelMatrixForStone = glm::translate(modelMatrixForStone, glm::vec3(-2.0f, 0.3f, -1.0f));
            modelMatrixForStone = glm::scale(modelMatrixForStone, glm::vec3(0.75f, 0.75f, 0.75f));
            modelMatrixForStone = glm::rotate(modelMatrixForStone, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            stoneObstacle.drawCylinder(*activeShader, modelMatrixForStone, camera.Position);

            // Hemisphere
            glm::mat4 modelMatrixForHemisphere2 = glm::mat4(1.0f);
            modelMatrixForHemisphere2 = glm::translate(modelMatrixForHemisphere2, glm::vec3(0.7f, 0.5f, 0.5f));
            modelMatrixForHemisphere2 = glm::scale(modelMatrixForHemisphere2, glm::vec3(0.5f, 0.8f, 0.5f));
            hemisphere.drawSphere(*activeShader, modelMatrixForHemisphere2, camera.Position);*/

            // Ghost Tail
            glm::mat4 modelMatrixForGhostTail = glm::mat4(1.0f);
            modelMatrixForGhostTail = glm::translate(modelMatrixForGhostTail, glm::vec3(0.7f, 0.5f, 0.5f));
            modelMatrixForGhostTail = glm::scale(modelMatrixForGhostTail, glm::vec3(0.5f, 0.5f, 0.5f));
            modelMatrixForGhostTail = glm::rotate(modelMatrixForGhostTail, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            ghostTail.drawCone(*activeShader, modelMatrixForGhostTail, camera.Position);

            // Gold Obstacle
           /* glm::mat4 modelMatrixForHemisphere3 = glm::mat4(1.0f);
            modelMatrixForHemisphere3 = glm::translate(modelMatrixForHemisphere3, glm::vec3(-0.5f, -0.5f, -1.0f));
            modelMatrixForHemisphere3 = glm::scale(modelMatrixForHemisphere3, glm::vec3(0.75f, 0.75f, 0.75f));
            gold.drawSphere(*activeShader, modelMatrixForHemisphere3, hemiDiffuseMap2, hemiDiffuseMap2, camera.Position);*/
            // Mouse click logic to verify the WC of the clicked point
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                // Get the mouse cursor position in screen space
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);

                // Calculate the ray from the camera through the mouse position
               /* glm::mat4 viewMatrix = camera.GetViewMatrix();
                glm::mat4 projectionMatrix = projection;
                glm::vec3 rayOrigin = camera.Position;
                glm::vec3 rayDirection = calculateRayFromMouse(mouseX, mouseY, SCR_WIDTH, SCR_HEIGHT, viewMatrix, projectionMatrix);

                std::cout << "Mouse clicked at screen coordinates: (" << mouseX << ", " << mouseY << ")" << std::endl;

                bool objectHit = false;

                // Check intersection with objects
                for (Object& obj : objects) {
                    if (intersectRayWithObject(rayOrigin, rayDirection, obj)) {
                        objectHit = true;

                        // Print object position (WC) to verify
                        std::cout << "Clicked object at WC: ("
                            << obj.position.x << ", "
                            << obj.position.y << ", "
                            << obj.position.z << ")" << std::endl;

                        // Lift the object up as feedback
                        obj.position.y += 1.0f;

                        // Check for the key
                        if (obj.hasKey) {
                            std::cout << "Key found under this object!" << std::endl;
                        }
                        else {
                            std::cout << "No key under this object." << std::endl;
                        }
                        break; // Stop checking after finding the first hit
                    }

                }

                if (!objectHit) {
                    std::cout << "No object hit at this mouse position." << std::endl;
                }*/
            }
            if (mouseClickDetected) {
                glm::vec3 rayOrigin = camera.Position;
                onMouseClick(mouseX, mouseY, SCR_WIDTH, SCR_HEIGHT, camera.Position, view, projection);
                mouseClickDetected = false; // Reset the mouse click flag
            }
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
                std::cout << "Resetting all objects to original positions." << std::endl;
                for (Object& obj : objects) {
                    obj.position = obj.originalPosition; // Reset position to original
                }
            }

            // Check if transitioning
            if (keyFound && !transitioning) {
                if (glfwGetTime() - keyFoundTime > 1.0f) { // 1-second delay
                    transitioning = true;
                    currentRoom = 1; // Switch to the next room
                    initializeNextRoom();
                    std::cout << "Transitioning to the next room..." << std::endl;
                }
            }



        }
        if (currentRoom == 1) {
            lightingShaderWithTexture.use();
            lightingShaderWithTexture.setMat4("projection", projection);
            lightingShaderWithTexture.setMat4("view", view);
            // Check for mouse click to shoot arrow
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                onArrowShoot(arrow, mouseX, mouseY, SCR_WIDTH, SCR_HEIGHT, camera.Position, view, projection);
            }
            checkGameStatus(); // Check game status (win/lose)

            // Render the next room
            renderNextRoom(lightingShaderWithTexture);
        }




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec4(r, g, b, a));
    lightingShader.setVec3("material.diffuse", glm::vec4(r, g, b, a));
    lightingShader.setVec3("material.specular", glm::vec3(0.1, 0.1, 0.1));
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
void drawCube2(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f, float e_r = 0.0f, float e_g = 0.0f, float e_b = 0.0f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec4(r, g, b, a));
    lightingShader.setVec3("material.diffuse", glm::vec4(r, g, b, a));
    lightingShader.setVec3("material.specular", glm::vec3(0, 0, 0));
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void bed(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 alTogether)
{
    float baseHeight = 0.3;
    float width = 1;
    float length = 2;
    float pillowWidth = 0.3;
    float pillowLength = 0.15;
    float blanketWidth = 0.8;
    float blanketLength = 0.7;
    float headHeight = 0.6;

    //base
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    glm::mat4 translate2 = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);
    scale = glm::scale(model, glm::vec3(width, baseHeight, length));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 0.545, 0.271, 0.075);

    //foam
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3(0, baseHeight, 0));
    scale = glm::scale(model, glm::vec3(width, 0.06, length));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 0.804, 0.361, 0.361);

    //pillow 1
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3((width / 2) - (0.1 + pillowWidth / 2), baseHeight + 1 * 0.06, (length / 2) - (0.025 + pillowWidth / 2)));
    scale = glm::scale(model, glm::vec3(pillowWidth, 0.04, pillowLength));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 1, 0.647, 0);

    //pillow 2
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3((-width / 2) + (0.1 + pillowWidth / 2), baseHeight + 1 * 0.06, (length / 2) - (0.025 + pillowWidth / 2)));
    scale = glm::scale(model, glm::vec3(pillowWidth, 0.04, pillowLength));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 1, 0.647, 0);

    //blanket
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3(0, baseHeight + 1 * 0.06, -(length / 2 - 0.025) + blanketLength / 2));
    scale = glm::scale(model, glm::vec3(blanketWidth, 0.015, blanketLength));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 0.541, 0.169, 0.886);

    //head
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3(0, 0, (length / 2 - 0.02 / 2) + 0.02));
    scale = glm::scale(model, glm::vec3(width, headHeight, 0.02));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 0.545, 0.271, 0.075);

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    /*if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if (rotateAxis_X) rotateAngle_X -= 0.1;
        else if (rotateAxis_Y) rotateAngle_Y -= 0.1;
        else rotateAngle_Z -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += 0.001;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += 0.001;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) translate_Z += 0.001;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) scale_X += 0.001;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) scale_X -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) scale_Y += 0.001;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += 0.001;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z -= 0.001;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        rotateAngle_X += 0.1;
        rotateAxis_X = 1.0;
        rotateAxis_Y = 0.0;
        rotateAxis_Z = 0.0;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        rotateAngle_Y += 0.1;
        rotateAxis_X = 0.0;
        rotateAxis_Y = 1.0;
        rotateAxis_Z = 0.0;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        rotateAngle_Z += 0.1;
        rotateAxis_X = 0.0;
        rotateAxis_Y = 0.0;
        rotateAxis_Z = 1.0;
    }*/

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        eyeX += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        eyeX -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        eyeZ += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        eyeZ -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        eyeY += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        eyeY -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        if (directionalOn) {
            directionallight.turnOff();
            directionalOn = false;
        }
        else {
            directionallight.turnOn();
            directionalOn = true;

        }


    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (pointOn1) {
            pointlight1.turnOff();

            pointOn1 = false;
        }
        else {
            pointlight1.turnOn();

            pointOn1 = true;

        }


    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        if (pointOn2) {

            pointlight2.turnOff();
            pointOn2 = false;
        }
        else {

            pointlight2.turnOn();
            pointOn2 = true;

        }


    }


    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        if (spotOn) {
            spotlight.turnOff();
            spotOn = false;
        }
        else {
            spotlight.turnOn();
            spotOn = true;

        }


    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        if (ambientToggle)
        {

            pointlight1.turnAmbientOff();
            pointlight2.turnAmbientOff();
            directionallight.turnAmbientOff();
            spotlight.turnAmbientOff();

            ambientToggle = false;
        }
        else
        {

            pointlight1.turnAmbientOn();
            pointlight2.turnAmbientOn();
            directionallight.turnAmbientOn();
            spotlight.turnAmbientOn();

            ambientToggle = true;
        }

    }


    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        if (diffuseToggle)
        {

            pointlight1.turnDiffuseOff();
            pointlight2.turnDiffuseOff();
            directionallight.turnDiffuseOff();
            spotlight.turnDiffuseOff();

            diffuseToggle = false;
        }
        else
        {

            pointlight1.turnDiffuseOn();
            pointlight2.turnDiffuseOn();
            directionallight.turnDiffuseOn();
            spotlight.turnDiffuseOn();

            diffuseToggle = true;
        }

    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        if (specularToggle)
        {

            pointlight1.turnSpecularOff();
            pointlight2.turnSpecularOff();
            directionallight.turnSpecularOff();
            spotlight.turnSpecularOff();

            specularToggle = false;
        }
        else
        {

            pointlight1.turnSpecularOn();
            pointlight2.turnSpecularOn();
            directionallight.turnSpecularOn();
            spotlight.turnSpecularOn();

            specularToggle = true;
        }

    }




}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    /*if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Get cursor position
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        std::cout << "Mouse clicked at screen position: ("
            << mouseX << ", " << mouseY << ")"
            << std::endl;

        // Convert to ray
        glm::vec3 rayOrigin = camera.Position; // Assuming you have a camera object
        glm::vec3 rayDirection = calculateRayFromMouse(mouseX, mouseY, SCR_WIDTH, SCR_HEIGHT, camera.GetViewMatrix(), projection);

        // Check for intersections
        for (Object& obj : objects) {
            if (intersectRayWithObject(rayOrigin, rayDirection, obj)) {
                selectedObject = &obj; // Set the selected object
                std::cout << "Object selected at position: ("
                    << obj.position.x << ", "
                    << obj.position.y << ", "
                    << obj.position.z << ")"
                    << std::endl;
                break;
            }
        }
    }*/
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouseClickDetected = true;

        // Get the current mouse position
        glfwGetCursorPos(window, &mouseX, &mouseY);
    }

}




// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
void drawTabChairBook(Shader& shaderProgram, unsigned int VAO, glm::mat4 parentTrans)
{
    shaderProgram.use();
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, modelCentered;

    //translateMatrix = glm::translate(parentTrans, glm::vec3(posX, posY, posZ));
    //rotateXMatrix = glm::rotate(translateMatrix, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
    //rotateYMatrix = glm::rotate(rotateXMatrix, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
    //rotateZMatrix = glm::rotate(rotateYMatrix, glm::radians(rotZ), glm::vec3(0.0f, 0.0f, 1.0f));

    //table 
    model = parentTrans * glm::scale(identityMatrix, glm::vec3(3.5f, 0.2f, 2.0f));
    //modelCentered = glm::translate(model, glm::vec3(-0.25, -0.25, -0.25));
    drawCube(VAO, shaderProgram, model, 0.9176f, 0.7020f, 0.0314f);
    //drawCube(VAO, shaderProgram, model, 1.0f, 0.0f, 0.0f);
    //shaderProgram.setMat4("model", model);
    //shaderProgram.setVec4("color", glm::vec4(0.9176f, 0.7020f, 0.0314f, 0.89f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //table er pa
    model = parentTrans * glm::scale(identityMatrix, glm::vec3(0.2f, -2.0f, 0.2f));
    //modelCentered = glm::translate(model, glm::vec3(-0.25, -0.25, -0.25));
    drawCube(VAO, shaderProgram, model, 0.9176f, 0.7020f, 0.0314f);
    //shaderProgram.setMat4("model", model);
    //shaderProgram.setVec4("color", glm::vec4(0.9176f, 0.7020f, 0.0314f, 0.89f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 0.9));
    model = parentTrans * glm::scale(translateMatrix, glm::vec3(0.2f, -2.0f, 0.2f));
    //modelCentered = glm::translate(model, glm::vec3(-0.25, -0.25, -0.25));
    drawCube(VAO, shaderProgram, model, 0.9176f, 0.7020f, 0.0314f);
    //shaderProgram.setMat4("model", model);
    //shaderProgram.setVec4("color", glm::vec4(0.9176f, 0.7020f, 0.0314f, 0.89f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.65, 0.0, 0.9));
    model = parentTrans * glm::scale(translateMatrix, glm::vec3(0.2f, -2.0f, 0.2f));
    //modelCentered = glm::translate(model, glm::vec3(-0.25, -0.25, -0.25));
    drawCube(VAO, shaderProgram, model, 0.9176f, 0.7020f, 0.0314f);
    //shaderProgram.setMat4("model", model);
    //shaderProgram.setVec4("color", glm::vec4(0.9176f, 0.7020f, 0.0314f, 0.89f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.65, 0.0, 0.0));
    model = parentTrans * glm::scale(translateMatrix, glm::vec3(0.2f, -2.0f, 0.2f));
    //modelCentered = glm::translate(model, glm::vec3(-0.25, -0.25, -0.25));
    drawCube(VAO, shaderProgram, model, 0.9176f, 0.7020f, 0.0314f);
    // shaderProgram.setMat4("model", model);
    // shaderProgram.setVec4("color", glm::vec4(0.9176f, 0.7020f, 0.0314f, 0.89f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //lower book
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.55f, 0.1f, 0.50f));
    model = parentTrans * glm::scale(translateMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
    //modelCentered = glm::translate(model, glm::vec3(-0.25, -0.25, -0.25));
    drawCube(VAO, shaderProgram, model, 0.0f, 0.0f, 0.0f);
    //shaderProgram.setMat4("model", model);
    //shaderProgram.setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 0.89f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //upper book
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.67f, 0.15f, 0.75f));
    model = parentTrans * glm::scale(translateMatrix, glm::vec3(0.5f, 0.1f, 0.50f));
    //modelCentered = glm::translate(model, glm::vec3(-0.25, -0.25, -0.25));
    drawCube(VAO, shaderProgram, model, 1.0f, 1.0f, 1.0f);
    //shaderProgram.setMat4("model", model);
    //shaderProgram.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 0.89f));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Chair Seat
    glm::mat4 chairSeatTranslate = glm::translate(identityMatrix, glm::vec3(0.5f, -0.5f, 0.6f)); // Position in front of the books
    glm::mat4 chairSeat = parentTrans * glm::scale(chairSeatTranslate, glm::vec3(1.2f, 0.2f, 1.0f)); // Proper seat size
    drawCube(VAO, shaderProgram, chairSeat, 0.5f, 0.25f, 0.0f);
    //shaderProgram.setMat4("model", chairSeat);
    //shaderProgram.setVec4("color", glm::vec4(0.5f, 0.25f, 0.0f, 1.0f)); // Brown color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Chair Backrest
    glm::mat4 chairBackrestTranslate = glm::translate(chairSeatTranslate, glm::vec3(0.0f, 0.1f, 0.4f)); // Position backrest behind the seat
    glm::mat4 chairBackrest = parentTrans * glm::scale(chairBackrestTranslate, glm::vec3(1.2f, 1.4f, 0.2f)); // Backrest size
    drawCube(VAO, shaderProgram, chairBackrest, 0.3f, 0.2f, 0.1f);
    //shaderProgram.setMat4("model", chairBackrest);
    //shaderProgram.setVec4("color", glm::vec4(0.3f, 0.2f, 0.1f, 1.0f)); // Brown color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Chair Leg 1
    glm::mat4 leg1Translate = glm::translate(chairSeatTranslate, glm::vec3(0.0f, -0.50f, 0.4f)); // Bottom-left leg
    glm::mat4 chairLeg1 = parentTrans * glm::scale(leg1Translate, glm::vec3(0.2f, 1.0f, 0.2f)); // Leg size
    drawCube(VAO, shaderProgram, chairLeg1, 0.3f, 0.2f, 0.1f);
    //shaderProgram.setMat4("model", chairLeg1);
    //shaderProgram.setVec4("color", glm::vec4(0.3f, 0.2f, 0.1f, 1.0f)); // Dark brown color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Chair Leg 2
    glm::mat4 leg2Translate = glm::translate(chairSeatTranslate, glm::vec3(0.5f, -0.50f, 0.4f)); // Bottom-right leg
    glm::mat4 chairLeg2 = parentTrans * glm::scale(leg2Translate, glm::vec3(0.2f, 1.0f, 0.2f)); // Leg size
    drawCube(VAO, shaderProgram, chairLeg2, 0.3f, 0.2f, 0.1f);
    //shaderProgram.setMat4("model", chairLeg2);
    //shaderProgram.setVec4("color", glm::vec4(0.3f, 0.2f, 0.1f, 1.0f)); // Dark brown color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Chair Leg 3
    glm::mat4 leg3Translate = glm::translate(chairSeatTranslate, glm::vec3(0.5f, -0.50f, 0.0f)); // Bottom-right leg
    glm::mat4 chairLeg3 = parentTrans * glm::scale(leg3Translate, glm::vec3(0.2f, 1.0f, 0.2f)); // Leg size
    drawCube(VAO, shaderProgram, chairLeg3, 0.3f, 0.2f, 0.1f);
    //shaderProgram.setMat4("model", chairLeg3);
    //shaderProgram.setVec4("color", glm::vec4(0.3f, 0.2f, 0.1f, 1.0f)); // Dark brown color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Chair Leg 4
    glm::mat4 leg4Translate = glm::translate(chairSeatTranslate, glm::vec3(0.0f, -0.50f, 0.0f)); // Bottom-right leg
    glm::mat4 chairLeg4 = parentTrans * glm::scale(leg4Translate, glm::vec3(0.2f, 1.0f, 0.2f)); // Leg size
    drawCube(VAO, shaderProgram, chairLeg4, 0.3f, 0.2f, 0.1f);
    //shaderProgram.setMat4("model", chairLeg4);
    //shaderProgram.setVec4("color", glm::vec4(0.3f, 0.2f, 0.1f, 1.0f)); // Dark brown color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


}
// Function to draw walls and floor with red carpet
void drawWallsAndFloorWithCarpet(Shader& shaderProgram, unsigned int VAO, glm::mat4 model) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    // Floor
    glm::mat4 floorTransform = glm::translate(identityMatrix, glm::vec3(-4.0f, -.51f, -3.7f)); // Floor at y = -1.0f
    floorTransform = glm::scale(floorTransform, glm::vec3(11.0f, 0.1f, 9.5f)); // Large floor
    floorTransform = floorTransform * model;
    drawCube(VAO, shaderProgram, floorTransform, 0.6353f, 0.0314f, 0.1373f); // Floor color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Front Wall (with a door gap)
    glm::mat4 frontWallTransform = glm::translate(identityMatrix, glm::vec3(-4.0f, -.51f, -3.7f));
    frontWallTransform = glm::scale(frontWallTransform, glm::vec3(11.0f, 5.0f, 0.1f));
    frontWallTransform = frontWallTransform * model;
    drawCube(VAO, shaderProgram, frontWallTransform, 0.6f, 0.3f, 0.1f); // Wall color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



    // Back Wall
    /*glm::mat4 backWallTransform = glm::translate(identityMatrix, glm::vec3(-4.0f, -.51f, 1.0f));
    backWallTransform = glm::scale(backWallTransform, glm::vec3(11.0f, 5.0f, 0.1f));
    backWallTransform = backWallTransform * model;
    drawCube(VAO, shaderProgram, backWallTransform, 0.9f, 0.9f, 0.9f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/

    // Left Wall
    glm::mat4 leftWallTransform = glm::translate(identityMatrix, glm::vec3(-4.0f, -.51f, -3.7f));
    leftWallTransform = glm::scale(leftWallTransform, glm::vec3(0.1f, 5.0f, 9.5f));
    leftWallTransform = leftWallTransform * model;
    drawCube(VAO, shaderProgram, leftWallTransform, 0.6f, 0.3f, 0.1f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Right Wall (Positioned at middle)
    glm::mat4 rightWallTransform = glm::translate(identityMatrix, glm::vec3(1.5f, -.51f, -3.7f)); // Position of the right wall
    rightWallTransform = glm::scale(rightWallTransform, glm::vec3(0.1f, 5.0f, 9.5f)); // Wall dimensions
    rightWallTransform = rightWallTransform * model;
    drawCube(VAO, shaderProgram, rightWallTransform, 0.6f, 0.3f, 0.1f); // Wall color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Creating a gap for the door at the middle of the right wall
    glm::mat4 doorTransform = glm::translate(identityMatrix, glm::vec3(1.5f, -0.5f, -1.6f)); // Middle of the wall on x and z axis
    doorTransform = glm::scale(doorTransform, glm::vec3(0.1f, 4.0f, 2.0f)); // Door gap size (2 units wide, 4 units tall, thin depth)
    doorTransform = doorTransform * model;
    drawCube(VAO, shaderProgram, doorTransform, 0.0f, 0.0f, 0.0f); // Black to simulate "subtraction"
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Actual Door (Placed slightly inside the gap)
    glm::mat4 actualDoorTransform = glm::translate(identityMatrix, glm::vec3(1.45f, -0.5f, -1.6f)); // Slightly inside the gap on the z-axis
    actualDoorTransform = glm::scale(actualDoorTransform, glm::vec3(0.1f, 4.0f, 2.0f)); // Door dimensions
    drawCube(VAO, shaderProgram, actualDoorTransform, 0.5f, 0.3f, 0.1f); // Wooden door color
    actualDoorTransform = actualDoorTransform * model;
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);




    // Adding a Window (with transparent glass)
    glm::mat4 windowTransform = glm::translate(identityMatrix, glm::vec3(-2.0f, 0.40f, -3.65f)); // Position for window
    windowTransform = glm::scale(windowTransform, glm::vec3(3.0f, 2.5f, 0.1f)); // Window size
    drawCube(VAO, shaderProgram, windowTransform, 0.0f, 0.0f, 0.0f); // Simulating subtraction for window gap
    windowTransform = windowTransform * model;
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Adding transparent glass inside the window
    glm::mat4 glassTransform = glm::translate(identityMatrix, glm::vec3(-2.0f, 0.40f, -3.6f)); // Slightly inside position for glass
    glassTransform = glm::scale(glassTransform, glm::vec3(2.8f, 2.3f, 0.05f)); // Thin glass for the window
    glassTransform = glassTransform * model;
    drawCube(VAO, shaderProgram, glassTransform, 0.5f, 0.7f, 1.0f, 0.5f); // Light blue transparent glass (RGBA with alpha)
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}
void drawWallsAndFloorWithCarpet2(Shader& shaderProgram, unsigned int VAO) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    // Floor
    glm::mat4 floorTransform = glm::translate(identityMatrix, glm::vec3(-4.0f, -.51f, -3.7f)); // Floor at y = -1.0f
    floorTransform = glm::scale(floorTransform, glm::vec3(11.0f, 0.1f, 9.5f)); // Large floor
    drawCube(VAO, shaderProgram, floorTransform, 0.6353f, 0.0314f, 0.1373f);
    //shaderProgram.setMat4("model", floorTransform);
    //shaderProgram.setVec4("color", glm::vec4(0.6353f, 0.0314f, 0.1373f, 0.8f)); // Light gray floor
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Front Wall
    glm::mat4 frontWallTransform = glm::translate(identityMatrix, glm::vec3(-4.0f, -.51f, -3.7f)); // Positioned along Z-axis
    frontWallTransform = glm::scale(frontWallTransform, glm::vec3(11.0f, 5.0f, 0.1f)); // Large vertical plane
    drawCube(VAO, shaderProgram, floorTransform, 0.9f, 0.9f, 0.9f);
    //shaderProgram.setMat4("model", frontWallTransform);
    //shaderProgram.setVec4("color", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)); // Off-white color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Back Wall
    glm::mat4 backWallTransform = glm::translate(identityMatrix, glm::vec3(-4.0f, -.51f, 1.0f)); // Positioned along Z-axis
    backWallTransform = glm::scale(backWallTransform, glm::vec3(11.0f, 5.0f, 0.1f)); // Large vertical plane
    drawCube(VAO, shaderProgram, backWallTransform, 0.9f, 0.9f, 0.9f);
    //shaderProgram.setMat4("model", backWallTransform);
    //shaderProgram.setVec4("color", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)); // Off-white color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Left Wall
    glm::mat4 leftWallTransform = glm::translate(identityMatrix, glm::vec3(-4.0f, -.51f, -3.7f)); // Positioned along X-axis
    leftWallTransform = glm::scale(leftWallTransform, glm::vec3(0.1f, 5.0f, 9.5f)); // Large vertical plane
    drawCube(VAO, shaderProgram, leftWallTransform, 0.9f, 0.9f, 0.9f);
    //shaderProgram.setMat4("model", leftWallTransform);
    //shaderProgram.setVec4("color", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)); // Off-white color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //AC1
    glm::mat4 acTransform = glm::translate(identityMatrix, glm::vec3(-4.0f, 0.4f, -2.0f)); // Positioned along X-axis
    acTransform = glm::scale(acTransform, glm::vec3(0.9f, 1.2f, 2.0f)); // Large vertical plane
    drawCube(VAO, shaderProgram, acTransform, 0.9647f, 0.8235f, 0.3725f);
    //shaderProgram.setMat4("model", acTransform);
    //shaderProgram.setVec4("color", glm::vec4(0.9647f, 0.8235f, 0.3725f, 0.8f)); // Off-white color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //AC2
    glm::mat4 acTransform2 = glm::translate(identityMatrix, glm::vec3(-4.0f, 0.4f, -0.4f)); // Positioned along X-axis
    acTransform2 = glm::scale(acTransform2, glm::vec3(0.9f, 1.2f, 2.0f)); // Large vertical plane
    drawCube(VAO, shaderProgram, acTransform2, 0.9647f, 0.8235f, 0.3725f);
    //shaderProgram.setMat4("model", acTransform2);
    //shaderProgram.setVec4("color", glm::vec4(0.9647f, 0.8235f, 0.3725f, 0.8f)); // Off-white color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Right Wall
    glm::mat4 rightWallTransform = glm::translate(identityMatrix, glm::vec3(1.5f, -.51f, -3.7f)); // Positioned along X-axis
    rightWallTransform = glm::scale(rightWallTransform, glm::vec3(0.1f, 5.0f, 9.5f)); // Large vertical plane
    drawCube(VAO, shaderProgram, rightWallTransform, 0.9f, 0.9f, 0.9f);
    //shaderProgram.setMat4("model", rightWallTransform);
    //shaderProgram.setVec4("color", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)); // Off-white color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //AC3
    glm::mat4 acTransform3 = glm::translate(identityMatrix, glm::vec3(1.1f, 0.4f, -2.0f)); // Positioned along X-axis
    acTransform3 = glm::scale(acTransform3, glm::vec3(0.9f, 1.2f, 2.0f)); // Large vertical plane
    drawCube(VAO, shaderProgram, acTransform3, 0.9647f, 0.8235f, 0.3725f);
    //saderProgram.setMat4("model", acTransform3);
   // shaderProgram.setVec4("color", glm::vec4(0.9647f, 0.8235f, 0.3725f, 0.8f)); // Off-white color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //AC4
    glm::mat4 acTransform4 = glm::translate(identityMatrix, glm::vec3(1.1f, 0.4f, -0.4f)); // Positioned along X-axis
    acTransform4 = glm::scale(acTransform4, glm::vec3(0.9f, 1.2f, 2.0f)); // Large vertical plane
    drawCube(VAO, shaderProgram, acTransform4, 0.9647f, 0.8235f, 0.3725f);
    //shaderProgram.setMat4("model", acTransform4);
    //shaderProgram.setVec4("color", glm::vec4(0.9647f, 0.8235f, 0.3725f, 0.8f)); // Off-white color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void drawFan(Shader& shaderProgram, unsigned int VAO, glm::mat4 parentTrans, bool fanOn, float& r) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, translateMatrix2, translateMatrix3, scaleMatrix, model;

    if (fanOn) {
        // Blade 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f, 0.5f, 0.125f));
        translateMatrix2 = glm::translate(identityMatrix, glm::vec3(-0.5f, 0.5f, -0.125f));
        translateMatrix3 = glm::translate(parentTrans, glm::vec3(-1.5f, 0.0f, -1.7f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.1f, 0.5f));
        model = translateMatrix3 * translateMatrix * glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f)) * translateMatrix2 * scaleMatrix;
        drawCube(VAO, shaderProgram, model, 0.1451f, 0.2039f, 0.5725f);
        //shaderProgram.setMat4("model", model);
        //shaderProgram.setVec4("color", glm::vec4(0.1451f, 0.2039f, 0.5725f, 0.8f));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Blade 2
        model = translateMatrix3 * translateMatrix * glm::rotate(identityMatrix, glm::radians(r + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * translateMatrix2 * scaleMatrix;
        drawCube(VAO, shaderProgram, model, 0.1451f, 0.2039f, 0.5725f);
        //shaderProgram.setMat4("model", model);
        //shaderProgram.setVec4("color", glm::vec4(0.1451f, 0.2039f, 0.5725f, 0.8f));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Stand
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.6f, 0.1f));
        translateMatrix3 = glm::translate(parentTrans, glm::vec3(-1.02f, 1.0f, -1.58f));
        model = translateMatrix3 * scaleMatrix;
        drawCube(VAO, shaderProgram, model, 0.0, 0.0f, 0.0f);
        // shaderProgram.setMat4("model", model);
        // shaderProgram.setVec4("color", glm::vec4(0.0, 0.0f, 0.0f, 1.0f));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Update rotation
        r = (r + 5.0f);
    }
    else {
        // Blade 1 (Static)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f, 0.5f, 0.125f));
        translateMatrix2 = glm::translate(identityMatrix, glm::vec3(-0.5f, 0.5f, -0.125f));
        translateMatrix3 = glm::translate(parentTrans, glm::vec3(-1.5f, 0.0f, -1.7f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.1f, 0.5f));
        model = translateMatrix3 * translateMatrix * glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f)) * translateMatrix2 * scaleMatrix;
        drawCube(VAO, shaderProgram, model, 0.1451f, 0.2039f, 0.5725f);
        //shaderProgram.setMat4("model", model);
        //shaderProgram.setVec4("color", glm::vec4(0.1451f, 0.2039f, 0.5725f, 0.8f));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Blade 2 (Static)
        model = translateMatrix3 * translateMatrix * glm::rotate(identityMatrix, glm::radians(r + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * translateMatrix2 * scaleMatrix;
        drawCube(VAO, shaderProgram, model, 0.1451f, 0.2039f, 0.5725f);
        //shaderProgram.setMat4("model", model);
        //shaderProgram.setVec4("color", glm::vec4(0.1451f, 0.2039f, 0.5725f, 0.8f));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Stand
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.6f, 0.1f));
        translateMatrix3 = glm::translate(parentTrans, glm::vec3(-1.02f, 1.0f, -1.58f));
        model = translateMatrix3 * scaleMatrix;
        drawCube(VAO, shaderProgram, model, 0.0, 0.0f, 0.0f);
        //shaderProgram.setMat4("model", model);
        //shaderProgram.setVec4("color", glm::vec4(0.0, 0.0f, 0.0f, 1.0f));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

// Function to draw a ghost sculpture
// Function to draw a ghost sculpture
// Function to draw a ghost sculpture
void drawGhostSculpture(Shader& shaderProgram, unsigned int VAO, glm::mat4 model) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    // Create a sphere (Body of the ghost)
   /* Hemisphere ghostBody(1.0f, 36, 18, glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.1f, 0.1f, 0.3f), 2.0f);                      // shininess
          // emissive color (this makes it glow)

    glm::mat4 bodyTransform = glm::translate(identityMatrix, glm::vec3(0.7f, 0.5f, -2.0f)); // Positioning the ghost's body
    bodyTransform = glm::scale(bodyTransform, glm::vec3(1.0f, 1.5f, 1.0f)); // Scaling to form an oval
    bodyTransform = bodyTransform * model; // Apply model transformations (translation and scaling)
    ghostBody.drawHemiSphere(shaderProgram, bodyTransform); // Draw the sphere as the ghost's body*/

    // Create a cone (Lower part of the ghost)
    Cone ghostTail(1.0f, 2.0f, 36, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 32.0f); // Light purple color
    glm::mat4 tailTransform = glm::translate(identityMatrix, glm::vec3(0.7f, 0.5f, -2.0f)); // Position beneath the body
    tailTransform = glm::scale(tailTransform, glm::vec3(1.0f, 1.0f, 1.0f)); // Making the cone wider
    tailTransform = glm::rotate(tailTransform, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Flip the cone upside down
    tailTransform = tailTransform * model; // Apply model transformations
    ghostTail.drawCone(shaderProgram, tailTransform); // Draw the cone as the ghost's tail

    // Adding transparency to the ghost
    shaderProgram.use();
    shaderProgram.setFloat("material.transparency", 0.3f); // Adjust transparency level (0.0 - 1.0)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enable alpha blending
}

void drawGhostSculpture2(Shader& shaderProgram, unsigned int VAO, glm::mat4 model) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    // Create a sphere (Body of the ghost)
   /* Hemisphere ghostBody(1.0f, 36, 18, glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.1f, 0.1f, 0.3f), 2.0f);                      // shininess
    // emissive color (this makes it glow)

    glm::mat4 bodyTransform = glm::translate(identityMatrix, glm::vec3(0.7f, 0.5f, 0.5f)); // Positioning the ghost's body
    bodyTransform = glm::scale(bodyTransform, glm::vec3(1.0f, 1.5f, 1.0f)); // Scaling to form an oval
    bodyTransform = bodyTransform * model; // Apply model transformations (translation and scaling)
    ghostBody.drawHemiSphere(shaderProgram, bodyTransform); // Draw the sphere as the ghost's body*/

    // Create a cone (Lower part of the ghost)
    Cone ghostTail(1.0f, 2.0f, 36, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 32.0f); // Light purple color
    glm::mat4 tailTransform = glm::translate(identityMatrix, glm::vec3(0.7f, 0.5f, 0.5f)); // Position beneath the body
    tailTransform = glm::scale(tailTransform, glm::vec3(1.0f, 1.0f, 1.0f)); // Making the cone wider
    tailTransform = glm::rotate(tailTransform, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Flip the cone upside down
    tailTransform = tailTransform * model; // Apply model transformations
    ghostTail.drawCone(shaderProgram, tailTransform); // Draw the cone as the ghost's tail

    // Adding transparency to the ghost
    shaderProgram.use();
    shaderProgram.setFloat("material.transparency", 0.3f); // Adjust transparency level (0.0 - 1.0)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enable alpha blending
}

// Function to draw a sphere (used for the body of the ghost)
void drawSphere(unsigned int VAO, Shader& shaderProgram, glm::mat4 model, float r, float g, float b, float alpha) {
    shaderProgram.use();
    shaderProgram.setMat4("model", model);
    shaderProgram.setVec4("color", glm::vec4(r, g, b, alpha)); // Set the color and transparency
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// Function to draw a cone (used for the lower part of the ghost)
void drawCone(unsigned int VAO, Shader& shaderProgram, glm::mat4 model, float r, float g, float b, float alpha) {
    shaderProgram.use();
    shaderProgram.setMat4("model", model);
    shaderProgram.setVec4("color", glm::vec4(r, g, b, alpha)); // Set the color and transparency
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}





void classroom(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix;
    //WALL 

    drawWallsAndFloorWithCarpet(lightingShader, cubeVAO, model);

    //almirah
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.00f, -0.5f, -1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 4.0f, 2.0f));
    model = translateMatrix * scaleMatrix;
    drawCube(cubeVAO, lightingShader, model, 0.5f, 0.4f, 0.2f, 1.0f);
    //ourShader.setMat4("model", model);
    //ourShader.setVec4("color", glm::vec4(0.3f, 0.2f, 0.1f, 1.0f));
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //almirah door
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.00f, -0.5f, -1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 4.0f, 1.0f));
    model = translateMatrix * scaleMatrix;
    drawCube(cubeVAO, lightingShader, model, 0.9f, 0.4f, 0.2f, 1.0f);
    //ourShader.setMat4("model", model);
    //ourShader.setVec4("color", glm::vec4(0.3f, 0.2f, 0.1f, 1.0f));
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    /*
    //shinduk
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, -0.5f, -3.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.5f, 1.2f, 1.5f));
    model = translateMatrix * scaleMatrix;
    drawCube(cubeVAO, lightingShader, model, 0.3f, 0.2f, 0.1f, 1.0f);
    //ourShader.setMat4("model", model);
    //ourShader.setVec4("color", glm::vec4(0.3f, 0.2f, 0.1f, 1.0f));
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //shinduk er upor
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 0.1f, -3.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.5f, 0.4f, 1.5f));
    model = translateMatrix * scaleMatrix;
    drawCube(cubeVAO, lightingShader, model, 0.6f, 0.3f, 0.6f, 1.0f);
    //ourShader.setMat4("model", model);
    //ourShader.setVec4("color", glm::vec4(0.3f, 0.2f, 0.1f, 1.0f));
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/











    //fan
    //drawFan(ourShader, VAO, identityMatrix, fanOn, r);
    /*translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.2f, 0.0f, 0.2f));

    drawFan(lightingShader, cubeVAO, translateMatrix, fanOn1, r1);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, 0.0f, 0.2f));
    drawFan(lightingShader, cubeVAO, translateMatrix, fanOn2, r2);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.2f, 0.0f, 1.5f));
    drawFan(lightingShader, cubeVAO, translateMatrix, fanOn3, r3);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, 0.0f, 1.5f));
    drawFan(lightingShader, cubeVAO, translateMatrix, fanOn4, r4);*/

    glm::mat4 mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    //sm = glm::translate(identityMatrix, glm::vec3(-1.2f, 0.0f, 1.4f)) * glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

    //drawTabChairBook(ourShader, VAO, sm);
    mat = mat * model;

    //mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    //drawTabChairBook(lightingShader, cubeVAO, mat);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, 0.5f, -3.5f));
    mat = translateMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    drawTabChairBook(lightingShader, cubeVAO, mat);
    mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    //translateMatrix= glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -3.5f));

    scaleMatrix = glm::scale(mat, glm::vec3(0.50f, 0.50f, 0.50f));

    //drawGhostSculpture(lightingShader, cubeVAO, scaleMatrix);

    mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    //translateMatrix= glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -3.5f));

    scaleMatrix = glm::scale(mat, glm::vec3(0.50f, 0.50f, 0.50f));

    //drawGhostSculpture2(lightingShader, cubeVAO, scaleMatrix);






}
void drawAlmirah(Shader& shaderProgram, unsigned int VAO, glm::mat4 model) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);

    // Main Body of the Almira
    glm::mat4 almiraBodyTransform = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f, 0.0f)); // Positioning the almira
    almiraBodyTransform = glm::scale(almiraBodyTransform, glm::vec3(4.0f, 8.0f, 2.0f)); // Almira body size
    almiraBodyTransform = almiraBodyTransform * model;
    drawCube(VAO, shaderProgram, almiraBodyTransform, 0.5f, 0.3f, 0.1f); // Wood color for almira body
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Left Door of the Almira
    glm::mat4 leftDoorTransform = glm::translate(identityMatrix, glm::vec3(-1.8f, -1.0f, 1.0f)); // Left door position
    leftDoorTransform = glm::scale(leftDoorTransform, glm::vec3(0.1f, 6.0f, 2.0f)); // Door dimensions
    leftDoorTransform = leftDoorTransform * model;
    drawCube(VAO, shaderProgram, leftDoorTransform, 0.4f, 0.2f, 0.1f); // Darker wood color for door
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Right Door of the Almira
    glm::mat4 rightDoorTransform = glm::translate(identityMatrix, glm::vec3(1.8f, -1.0f, 1.0f)); // Right door position
    rightDoorTransform = glm::scale(rightDoorTransform, glm::vec3(0.1f, 6.0f, 2.0f)); // Door dimensions
    rightDoorTransform = rightDoorTransform * model;
    drawCube(VAO, shaderProgram, rightDoorTransform, 0.4f, 0.2f, 0.1f); // Darker wood color for door
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Shelf 1 (middle shelf inside almira)
    glm::mat4 shelf1Transform = glm::translate(identityMatrix, glm::vec3(0.0f, 0.5f, 0.0f)); // Shelf position (middle)
    shelf1Transform = glm::scale(shelf1Transform, glm::vec3(4.0f, 0.1f, 2.0f)); // Shelf dimensions
    shelf1Transform = shelf1Transform * model;
    drawCube(VAO, shaderProgram, shelf1Transform, 0.3f, 0.2f, 0.1f); // Shelf color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Shelf 2 (bottom shelf inside almira)
    glm::mat4 shelf2Transform = glm::translate(identityMatrix, glm::vec3(0.0f, -0.5f, 0.0f)); // Shelf position (bottom)
    shelf2Transform = glm::scale(shelf2Transform, glm::vec3(4.0f, 0.1f, 2.0f)); // Shelf dimensions
    shelf2Transform = shelf2Transform * model;
    drawCube(VAO, shaderProgram, shelf2Transform, 0.3f, 0.2f, 0.1f); // Shelf color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Shelf 3 (top shelf inside almira)
    glm::mat4 shelf3Transform = glm::translate(identityMatrix, glm::vec3(0.0f, 1.5f, 0.0f)); // Shelf position (top)
    shelf3Transform = glm::scale(shelf3Transform, glm::vec3(4.0f, 0.1f, 2.0f)); // Shelf dimensions
    shelf3Transform = shelf3Transform * model;
    drawCube(VAO, shaderProgram, shelf3Transform, 0.3f, 0.2f, 0.1f); // Shelf color
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Back Panel of the Almira
    glm::mat4 backPanelTransform = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f, 2.0f)); // Back panel position
    backPanelTransform = glm::scale(backPanelTransform, glm::vec3(4.1f, 8.0f, 0.1f)); // Panel size
    backPanelTransform = backPanelTransform * model;
    drawCube(VAO, shaderProgram, backPanelTransform, 0.3f, 0.3f, 0.3f); // Dark gray color for back panel
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Optional: Knobs for the doors (small spheres or cubes) could be added to the door if needed
}




glm::mat4 customPerspective(float fovRadians, float aspect, float near, float far) {
    glm::mat4 projection(0.0f);

    float tanHalfFov = tan(fovRadians / 2.0f);

    // Fill in the matrix values
    projection[0][0] = 1.0f / (aspect * tanHalfFov);
    projection[1][1] = 1.0f / tanHalfFov;
    projection[2][2] = -(far + near) / (far - near);
    projection[2][3] = -1.0f;
    projection[3][2] = -(2.0f * far * near) / (far - near);

    return projection;
}
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadTexture2(const char* path, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    // Load image using stb_image
    stbi_set_flip_vertically_on_load(true); // Flip the texture vertically if needed
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    }
    else {
        std::cerr << "Failed to load texture at path: " << path << std::endl;
    }

    stbi_image_free(data); // Free image memory
    return textureID;
}

bool isPointInSphere(glm::vec3 rayOrigin, glm::vec3 rayDirection, Object& obj) {
    // Center of the bounding sphere
    glm::vec3 sphereCenter = obj.position;

    // Adjust for chest center offset
    if (obj.type == CHEST) {
        glm::vec3 centerOffset(0.5f * obj.scale.x, 0.5f * obj.scale.y, 0.5f * obj.scale.z);
        sphereCenter += centerOffset;
    }

    // Ray-sphere intersection test
    glm::vec3 oc = rayOrigin - sphereCenter; // Vector from ray origin to sphere center
    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * glm::dot(oc, rayDirection);
    float c = glm::dot(oc, oc) - obj.boundingRadius * obj.boundingRadius;
    float discriminant = b * b - 4.0f * a * c;

    return discriminant >= 0.0f; // Intersection if discriminant is non-negative
}

glm::vec3 getTransformedPosition(const Object& obj) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, obj.position);
    model = glm::rotate(model, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    return glm::vec3(model[3]); // Extract the transformed position from the matrix
}
bool isPointInsideSphere(glm::vec3 rayOrigin, glm::vec3 sphereCenter, float boundingRadius) {
    glm::vec3 difference = rayOrigin - sphereCenter;
    float distanceSquared = glm::dot(difference, difference); // Squared distance
    return distanceSquared <= (boundingRadius * boundingRadius);
}
bool intersectRayWithSphere(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 sphereCenter, float radius) {
    glm::vec3 L = rayOrigin - sphereCenter; // Vector from sphere center to ray origin
    float a = glm::dot(rayDirection, rayDirection); // Always 1 for normalized rays
    float b = 2.0f * glm::dot(rayDirection, L);
    float c = glm::dot(L, L) - radius * radius;

    // Solve the quadratic equation: t² + b * t + c = 0
    float discriminant = b * b - 4 * a * c;

    // Check if the ray intersects the sphere
    if (discriminant < 0) {
        std::cout << "No intersection: Discriminant < 0" << std::endl;
        return false; // No intersection
    }

    // Calculate the two intersection points (t0 and t1)
    float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
    float t1 = (-b + sqrt(discriminant)) / (2.0f * a);

    // Check if either intersection point is in front of the ray origin
    if (t0 >= 0 || t1 >= 0) {
        std::cout << "Intersection found! t0: " << t0 << ", t1: " << t1 << std::endl;
        return true;
    }

    std::cout << "No valid intersection: Both t0 and t1 are behind the ray origin" << std::endl;
    return false;
}





void onMouseClick(double mouseX, double mouseY, int screenWidth, int screenHeight, glm::vec3 cameraPos, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
    // Calculate ray direction from mouse position
    glm::vec3 rayDirection = calculateRayFromMouse(mouseX, mouseY, screenWidth, screenHeight, viewMatrix, projectionMatrix);

    std::cout << "Mouse Ray: (" << rayDirection.x << ", " << rayDirection.y << ", " << rayDirection.z << ")" << std::endl;
    std::cout << "Ray Origin: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;

    // Check each object for intersection
    for (Object& obj : objects) {
        // Calculate the transformed position of the object
        glm::vec3 transformedPosition = getTransformedPosition(obj);
        std::cout << "Testing Object - Transformed Position: ("
            << transformedPosition.x << ", "
            << transformedPosition.y << ", "
            << transformedPosition.z << ")" << std::endl;
        std::cout << "Bounding Radius: " << obj.boundingRadius << std::endl;
        float testr = obj.boundingRadius;

        // Perform ray-sphere intersection
        if (intersectRayWithSphere(cameraPos, rayDirection, transformedPosition, testr)) {
            glm::vec3 closestPoint = cameraPos + glm::dot(transformedPosition - cameraPos, rayDirection) * rayDirection;
            std::cout << "Object Selected at Position: ("
                << obj.position.x << ", "
                << obj.position.y << ", "
                << obj.position.z << ")" << std::endl;

            std::cout << "Closest Point on Ray: ("
                << closestPoint.x << ", "
                << closestPoint.y << ", "
                << closestPoint.z << ")" << std::endl;

            // Lift the object as feedback
            obj.position.y += 1.0f;

            // Print if the object has a key
            if (obj.hasKey) {
                std::cout << "You found the key! The door is opening." << std::endl;
                keyFound = true; // Set key found
                keyFoundTime = glfwGetTime(); // Record the time
            }
            else {
                std::cout << "No key under this object." << std::endl;
            }

            return; // Stop after finding the first intersected object
        }
    }

    // If no object was selected
    std::cout << "No object was clicked." << std::endl;
}
/*void renderNextRoom(Shader& shader, glm::vec3 viewPos, unsigned int cubeVAO) {


    //arrow initialization

    arrow.position = glm::vec3(0.0f, 1.0f, -5.0f); // Starting position
    arrow.direction = glm::vec3(0.0f, 0.0f, -1.0f); // Default forward direction
    arrow.speed = 10.0f; // Adjust speed as needed
    arrow.isFlying = false;

    // Example objects for the next room
    /*glm::mat4 model = glm::mat4(1.0f);

    // Render a door to indicate transition
    glm::mat4 doorModel = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
    doorModel = glm::scale(doorModel, glm::vec3(2.0f, 4.0f, 0.1f));
    shader.setMat4("model", doorModel);
    drawCube(cubeVAO, shader, model, 0.545, 0.271, 0.075); // Assume drawCube renders a cube
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Render additional objects for the next room
    glm::mat4 tableModel = glm::translate(model, glm::vec3(2.0f, 0.0f, -3.0f));
    shader.setMat4("model", tableModel);
    drawCube(cubeVAO, shader, model, 0.545, 0.271, 0.075); // Assume drawCube renders a cube
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/
    // Update targets
    /*for (Target& target : targets) {
        target.update(deltaTime);
    }

    // Update arrow
    arrow.update(deltaTime);

    // Check for collisions
    checkCollisions(arrow);

    // Render objects
    for (Target& target : targets) {
        if (!target.isHit) {
            target.draw(shader);
        }
    }
    arrow.draw(shader);
    glm::vec3 start = arrow.position; // Arrow start position
    glm::vec3 direction = arrow.direction; // Arrow direction
    //std::vector<glm::vec3> trajectoryPoints = generateTrajectoryPoints(start, direction, 10, 0.5f);

    updateTrajectoryBuffer(trajectoryPoints); // Update VBO with trajectory points
    renderTrajectory(shader); // Render the trajectory
    // Render score
    //renderScore(shader);
}*/
void renderNextRoom(Shader& shader) {
    if (gameOver) {
        if (allTargetsHit) {
            // Render the golden sphere for winning
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 1.0f, -10.0f));
            model = glm::scale(model, glm::vec3(2.0f));
            shader.setMat4("model", model);

            // Load textures for the golden sphere
            unsigned int goldDiffuseMap = loadTexture2("gold.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
            unsigned int goldSpecularMap = loadTexture2("gold.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

            SphereWithTexture goldSphere(1.0f, 36, 18,
                glm::vec3(1.0f, 0.84f, 0.0f),
                glm::vec3(1.0f, 0.9f, 0.0f),
                glm::vec3(1.0f, 1.0f, 1.0f),
                32.0f, goldDiffuseMap, goldSpecularMap);
            goldSphere.drawSphere(shader, model, camera.Position);
        }
        else {
            // Render the sad emoji sphere for losing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 1.0f, -10.0f));
            model = glm::scale(model, glm::vec3(2.0f));
            shader.setMat4("model", model);

            // Load textures for the sad emoji
            unsigned int sadDiffuseMap = loadTexture2("sad_emo.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
            unsigned int sadSpecularMap = loadTexture2("sad_emo.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

            SphereWithTexture sadSphere(1.0f, 36, 18,
                glm::vec3(1.0f, 1.0f, 1.0f),
                glm::vec3(1.0f, 1.0f, 1.0f),
                glm::vec3(0.5f, 0.5f, 0.5f),
                32.0f, sadDiffuseMap, sadSpecularMap);
            sadSphere.drawSphere(shader, model, camera.Position);
        }
    }
    else {
        // Update targets
        for (Target& target : targets) {
            target.update(deltaTime);
        }

        // Update arrow
        arrow.update(deltaTime);

        // Check for collisions
       // Check for collisions
        checkCollisions(arrow); // Call here to check for any collisions

        // Render targets
        for (Target& target : targets) {
            if (!target.isHit) {
                target.draw(shader);
            }
        }

        // Render arrow
        arrow.draw(shader);

        // Render trajectory
        if (!arrow.isFlying) { // Only show trajectory when arrow is not flying
            glm::vec3 start = arrow.position;
            glm::vec3 direction = arrow.direction;
            std::vector<glm::vec3> trajectoryPoints = generateTrajectoryPoints(start, direction, 10, 0.5f);

            updateTrajectoryBuffer(trajectoryPoints);
            renderTrajectory(shader, trajectoryPoints);



        }

        // Render score
        //renderScore(shader);
    }
}/*

void renderNextRoom(Shader & shader) {
    // Update targets
    for (Target& target : targets) {
        target.update(deltaTime);
    }

    // Update arrow
    arrow.update(deltaTime);

    // Check for collisions
   // Check for collisions
    checkCollisions(arrow); // Call here to check for any collisions

    // Render targets
    for (Target& target : targets) {
        if (!target.isHit) {
            target.draw(shader);
        }
    }

    // Render arrow
    arrow.draw(shader);

    // Render trajectory
    if (!arrow.isFlying) { // Only show trajectory when arrow is not flying
        glm::vec3 start = arrow.position;
        glm::vec3 direction = arrow.direction;
        std::vector<glm::vec3> trajectoryPoints = generateTrajectoryPoints(start, direction, 10, 0.5f);

        updateTrajectoryBuffer(trajectoryPoints);
        renderTrajectory(shader, trajectoryPoints);



    }

    // Render score
    //renderScore(shader);
}

// Render Next Room
/*void renderNextRoom(Shader& shader) {
    for (Target& target : targets) {
        if (!target.isHit) {
            target.draw(shader);
        }
    }
    arrow.draw(shader);
    if (!arrow.isFlying) {
        std::vector<glm::vec3> trajectoryPoints = generateTrajectoryPoints(arrow.position, arrow.direction, 10, 0.5f);
        updateTrajectoryBuffer(trajectoryPoints);
        renderTrajectory(shader, trajectoryPoints);
    }
}*/





