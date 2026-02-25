#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


#include "cube.h"
#include "chest.h"
#include "stb_image.h"
#include "sphereWithTexture.h"
#include "hemiWithTex.h"
#include "coneWithTexture.h"
#include "cylinderWithTexture.h"
#include "CurveWithTexture.h"
#include "LeftFaceTexturedCube.h"
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
#include "Halfcyl.h"




#include <iostream>

using namespace std;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b, float a);
void drawTableChair(Shader& shaderProgram, unsigned int VAO, glm::mat4 parentTrans);
void DrawRoom(Shader& shaderProgram, unsigned int VAO, glm::mat4 model);
void classroom(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 alTogether);
unsigned int loadTexture(const char* path, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseClick(double mouseX, double mouseY, int screenWidth, int screenHeight, glm::vec3 cameraPos, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
bool intersectObject(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 sphereCenter, float radius);
void renderLevel1(Shader& shader, GLFWwindow* window, glm::mat4 view, glm::mat4 projection);
void renderLevel2(Shader& shader, GLFWwindow* window, glm::mat4 view, glm::mat4 projection);
void MaterialSpec(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, float ra, float ga, float ba, float rd, float gd, float bd, float rs, float gs, float bs, float shininess, float re, float ge, float be);



glm::mat4 PerspectiveProjection(float fovRadians, float aspect, float near, float far) {
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
glm::mat4 OrthogonalProjection(float left, float right, float bottom, float top, float near, float far) {
    glm::mat4 projection(1.0f);

    projection[0][0] = 2.0f / (right - left);
    projection[1][1] = 2.0f / (top - bottom);
    projection[2][2] = -2.0f / (far - near);
    projection[3][0] = -(right + left) / (right - left);
    projection[3][1] = -(top + bottom) / (top - bottom);
    projection[3][2] = -(far + near) / (far - near);

    return projection;
}
glm::mat4 PerspectiveProjection6(float left, float right, float bottom, float top, float near, float far) {
    glm::mat4 projection(0.0f);

    projection[0][0] = (2.0f * near) / (right - left);
    projection[1][1] = (2.0f * near) / (top - bottom);
    projection[0][2] = (right + left) / (right - left);
    projection[1][2] = (top + bottom) / (top - bottom);
    projection[2][2] = -(far + near) / (far - near);
    projection[2][3] = -1.0f;
    projection[3][2] = -(2.0f * far * near) / (far - near);

    return projection;
}




//car

// Car parameters
float carPosition = 0.0f; // Current position of the car
float carSpeed = 0.5f;    // Speed of the car
float wheelAngle = 0.0f;  // Angle of the wheels (rotation)
float carRange = 2.0f;    // Range the car moves forward and backward
float carRange2 = -2.0f;
bool movingForward = true; // Direction of car movement

void renderCar(Cube& carBody, CylinderWithTexture& wheel, Shader& shader, float deltaTime) {
    // Update car position
    float distanceTraveled = carSpeed * deltaTime; // Distance the car moves in this frame
    float anglePerUnit = 360.0f / (2.0f * PI * 0.25f); // Degrees of wheel rotation per unit distance (wheel radius = 0.25)
    //glm::mat4 tog = 1.0f;
    //tog = glm::translate(tog, glm::vec3(0.0,0.0,5.0));

    if (!movingForward) {
        carPosition += distanceTraveled;
        wheelAngle += distanceTraveled * anglePerUnit; // Wheels rotate forward
        if (carPosition >= carRange) {
            movingForward = true;
        }
    }
    else {
        carPosition -= distanceTraveled;
        wheelAngle -= distanceTraveled * anglePerUnit; // Wheels rotate backward
        if (carPosition <= -carRange) {
            movingForward = false;
        }
    }

    // Render the car body
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(carPosition, -0.1f, -5.5f));
    model = glm::scale(model, glm::vec3(1.2, 0.1f, 0.8f));
    carBody.drawCubeWithTexture(shader, model);

    // Render the wheels
    glm::vec3 wheelOffsets1[] = {
        glm::vec3(0.45f, -0.25f, 0.4f),  // Front-right
        glm::vec3(-0.45f, -0.25f, 0.4f), // Front-left
        glm::vec3(0.45f, -0.25f, -0.4f), // Back-right
        glm::vec3(-0.45f, -0.25f, -0.4f) // Back-left
    };
    // Render the wheels
    glm::vec3 wheelOffsets[] = {
        glm::vec3(1.0f, -0.3f, 0.2f),  // Front-right
        glm::vec3(0.2f, -0.3f, 0.2f), // Front-left
        glm::vec3(1.0f, -0.3f, -0.2f), // Back-right
        glm::vec3(0.2f, -0.3f, -0.2f) // Back-left
    };

    for (const auto& offset : wheelOffsets) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(carPosition, 0.0f, -5.5f) + offset);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(wheelAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        wheel.drawCylinder(shader, model, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void renderCar1(Cube& carBody, CylinderWithTexture& wheel, Shader& shader, float deltaTime) {
    // Update car position
    if (!movingForward) {
        carPosition += carSpeed * deltaTime;
        wheelAngle += 360.0f * deltaTime; // Wheels rotate forward
        if (carPosition >= carRange) {
            movingForward = true;
        }
    }
    else {
        carPosition -= carSpeed * deltaTime;
        wheelAngle -= 360.0f * deltaTime; // Wheels rotate backward
        if (carPosition <= -carRange) {
            movingForward = false;
        }
    }

    // Render the car body
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(carPosition, 0.2f, 0.0f));
    model = glm::scale(model, glm::vec3(1.2f, 0.1f, 0.5f));
    carBody.drawCubeWithTexture(shader, model);

    // Render the wheels
    glm::vec3 wheelOffsets[] = {
        glm::vec3(0.45f, -0.5f, 0.4f),  // Front-right
        glm::vec3(-0.45f, -0.5f, 0.4f), // Front-left
        glm::vec3(0.45f, -0.5f, -0.6f), // Back-right
        glm::vec3(-0.45f, -0.5f, -0.6f) // Back-left
    };

    for (const auto& offset : wheelOffsets) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(carPosition+0.5f, 0.5f, 0.0f) + offset);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(wheelAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f,0.5f,0.5f));
        wheel.drawCylinder(shader, model, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}




// Door variables
float doorAngle = 0.0f;  // Current rotation angle of the door
bool isDoorOpening = false; // Tracks whether the door is in the process of opening or closing
bool isDoorOpen = false;    // Tracks whether the door is fully open

void handleDoorInput(GLFWwindow* window, float deltaTime, glm::mat4& doorTransform) {
    // Check for key press to toggle door state
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !isDoorOpening) {
        isDoorOpening = true;  // Start the transition
    }

    // Gradually open or close the door
    if (isDoorOpening) {
        if (!isDoorOpen) {
            // Open the door gradually
            doorAngle -= 50.0f * deltaTime; // Adjust speed with deltaTime
            if (doorAngle <= -90.0f) {      // Check if the door has fully opened
                doorAngle = -90.0f;         // Clamp to fully open
                isDoorOpening = false;     // Stop transitioning
                isDoorOpen = true;         // Mark door as fully open
            }
        }
        else {
            // Close the door gradually
            doorAngle += 50.0f * deltaTime;
            if (doorAngle >= 0.0f) {       // Check if the door has fully closed
                doorAngle = 0.0f;          // Clamp to fully closed
                isDoorOpening = false;    // Stop transitioning
                isDoorOpen = false;       // Mark door as fully closed
            }
        }
    }

    // Apply the transformation to the door
    doorTransform = glm::mat4(1.0f); // Reset the transformation
    doorTransform = glm::translate(doorTransform, glm::vec3(0.0f, -0.5f, -8.6f)); // Translate to door's position
    doorTransform = glm::translate(doorTransform, glm::vec3(0.0f, 0.0f, 0.0f));  // Move pivot to edge
    doorTransform = glm::rotate(doorTransform, glm::radians(doorAngle), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around pivot
    doorTransform = glm::translate(doorTransform, glm::vec3(0.0f, 0.0f, 0.0f));   // Move pivot back to original position
    doorTransform = glm::scale(doorTransform, glm::vec3(1.0f, 2.2f, 0.1f));       // Scale
}





enum ObjectType { SPHERETEX, CHEST, CYLINDERTEX, HEMISPHERETEX,CURVETEX,CONETEX };
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
    else if (type == CURVETEX) {
        static_cast<CurveWithTexture*>(renderable)->render(shader, model, viewPos);
    }
    else if (type == CONETEX) {
        static_cast<ConeWithTexture*>(renderable)->drawCone(shader, model, viewPos);
    }
}
void initializeObjects(SphereWithTexture& globe, CylinderWithTexture& stoneObstacle, Chest& chestCube, HemiWithTex& hemiSphere, CurveWithTexture& vase,ConeWithTexture& selcone) {
    /*objects.push_back({

        glm::vec3(-3.5f, 1.2f, -3.0f), // Position
        glm::vec3(-3.5f, 1.2, -3.0f), // Original position
        glm::vec3(0.4f, 0.4f, 0.4f),   // Scale
        glm::vec3(0.0f, 45.0f, 0.0f),  // Rotation
        0.4f,                          // Bounding radius (already scaled)
        false,                         // Does not have the key
        SPHERETEX,                     // Object type
        &globe                         // Pointer to the sphere object
        });*/

    objects.push_back({
        glm::vec3(1.5f, -0.5f, -2.3f), // Position
        glm::vec3(1.5f, -0.5f, -2.3f), // Original position
        glm::vec3(2.5f, 0.9f, 1.2f),   // Scale
        glm::vec3(0.0f, 180.0f, 0.0f), // Rotation
        1.0f,
        true,                          // This object has the key
        CHEST,                         // Object type
        &chestCube                     // Pointer to the chest object
        });

    objects.push_back({
        glm::vec3(-2.0f, 0.0f, -1.0f), // Position
        glm::vec3(-2.0f, 0.0f, -1.0f), // Original position
      //  glm::vec3(0.75f, 0.75f, 0.75f),// Scale
          glm::vec3(0.4f, 0.4f, 0.4f),// Scale
        glm::vec3(0.0f, 0.0f, 0.0f),  // Rotation
       // 1.06f , // Properly scaled bounding radius
        0.35f,
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
     glm::vec3(-2.0f, -0.7f, 1.5f), // New Position (ensured no overlap)
     glm::vec3(-2.0f, -0.7f, 1.5f), // Original position
     glm::vec3(0.40f, 0.50f, 0.40f),// Scale
     glm::vec3(0.0f, 0.0f, 0.0f),  // Rotation
     0.70f , // New bounding radius
     false,                         // Does not have the key
     CURVETEX,                      // Object type
     &vase                          // Pointer to the vase object
        });
    objects.push_back({
     glm::vec3(1.3f, -0.5f, 0.6f), // New Position (ensured no overlap)
     glm::vec3(1.3f, -0.5f, 0.6f), // Original position
     glm::vec3(0.5f, 0.7f, 0.5f), // Scale
     glm::vec3(0.0f, 0.0f, 0.0f), // Rotation
     0.43f,                       // Calculated bounding radius
     false,                       // Does not have the key
     CONETEX,                     // Object type
     &selcone                   // Pointer to the cone object
        });

}


glm::vec3 getTransformedPosition(const Object& obj);
void drawBoundingSphere(Shader& shader, Object& obj) {
    Sphere sphere;
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), obj.position), glm::vec3(obj.boundingRadius));
    shader.setMat4("model", model);
    //sphere.drawSphere(shader,model); // Assume you have a wireframe sphere drawing utility
}

glm::vec3 TraceRay(double mouseX, double mouseY, int screenWidth, int screenHeight, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
    // Convert mouse position to Normalized Device Coordinates (NDC)
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight; // Flip Y to match OpenGL's coordinate system
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f); // Ray in clip space

    // Transform to View space
    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Set w=0 for direction vector

    // Transform to world space
    glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
    rayWorld = glm::normalize(rayWorld);

    std::cout << "Mouse Ray: (" << rayWorld.x << ", " << rayWorld.y << ", " << rayWorld.z << ")" << std::endl;

    return rayWorld;
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
float initialEyeX = 0.0, initialEyeY = 2.0, initialEyeZ = 7.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);


// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(3.40f,  1.50f,  -0.5f),
    //glm::vec3(1.40f,  1.50f,  -0.5f)
    glm::vec3(-3.8f,  1.5f,  -0.5f)

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
    1,      // light number
    true
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

    -1.5, 2, -5.0,  //pos
    .2, .2, .2, //ambient
    .8f, .8f, .8f, //diffuse
    0.1f, 0.1f, 0.1f, //specular
    0, -4, 1.7, //direction
    glm::cos(glm::radians(15.5f)), //cos_theta
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
    0.0f, 5.0f, 0.0f //dir

);


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
float gameStartTime = glfwGetTime(); // Record the start time
bool gameOver = false;
bool allTargetsHit = false;
bool gameOver2 = false;
bool allTargetsHit2 = false;
bool firstFound = false; // Whether the key is found
float firstTime = 0.0f;
bool level1 = true; // Static targets level
bool level2 = false;


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




Object* selectedObject = nullptr; 

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
    model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f)); // Adjust arrow size
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    //model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0, 0.0, 1.0));

    shader.setMat4("model", model);

    // Updated: Added a generic texture for the arrow
    unsigned int arrowTexture = loadTexture("container2.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    CylinderWithTexture arrowTex(0.8f, 0.8f, 2.0f, 50, 20,
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
    glm::vec3 originalPosition;
    float radius;         // Radius of the target
    bool isHit;           // Whether the target is hit
    float speed;          // Speed of the target's movement
    unsigned int textureID;  // Texture ID for the target
    void draw(Shader& shader);  // Render the target
    void update(float deltaTime); // Move the target

    // Constructor
    // Constructor
    Target(glm::vec3 orpos, glm::vec3 pos, float rad, bool hit, float spd, unsigned int texID)
        : originalPosition(orpos), position(pos), radius(rad), isHit(hit), speed(spd), textureID(texID) {}
};
std::vector<Target> targets; // List of targets

// Updated initializeNextRoom Function
void initializeNextRoom() {
    // Arrow Initialization


    arrow.position = glm::vec3(0.0f, 1.1f, 5.2f);
    arrow.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    arrow.speed = 10.0f;
    arrow.isFlying = false;

    // Load textures for different targets
    unsigned int texture1 = loadTexture("shoot133.jpg", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texture2 = loadTexture("shoot133.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texture3 = loadTexture("shoot133.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Initialize targets with different textures
    targets.clear();
    targets.push_back(Target(glm::vec3(-5.0f, 0.0f, -20.0f), glm::vec3(-7.0f, 0.0f, -20.0f), 0.5f, false, 0.0f, texture1));
    targets.push_back(Target(glm::vec3(0.0f, 0.0f, -20.0f), glm::vec3(-2.0f, 0.0f, -20.0f), 0.5f, false, 0.0f, texture2));
    targets.push_back(Target(glm::vec3(5.0f, 0.0f, -20.0f), glm::vec3(3.0f, 0.0f, -20.0f), 0.5f, false, 0.0f, texture3));
}

void initializeNextRoom2() {
    // Arrow Initialization


    arrow.position = glm::vec3(0.0f, 1.1f, 5.2f);
    arrow.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    arrow.speed = 10.0f;
    arrow.isFlying = false;

    targets.clear();
    // Load textures for different targets
    unsigned int texture1 = loadTexture("shoot133.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texture2 = loadTexture("shoot133.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texture3 = loadTexture("shoot133.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);


    // Updated: Added a variety of target positions
    targets.push_back(Target(glm::vec3(-5.0f, 0.0f, -20.0f), glm::vec3(-7.0f, 0.0f, -20.0f), 0.5f, false, 1.0f, texture1));
    targets.push_back(Target(glm::vec3(0.0f, 0.0f, -20.0f), glm::vec3(-2.0f, 0.0f, -20.0f), 0.5f, false, 1.0f, texture2));
    targets.push_back(Target(glm::vec3(5.0f, 0.0f, -20.0f), glm::vec3(3.0f, 0.0f, -20.0f), 0.5f, false, 1.0f, texture3));
}



// Collision Check
void isTargetHit(Arrow& arrow) {
    for (Target& target : targets) {
        if (!target.isHit) { // Only check for targets that are not hit
            // Calculate the vector difference between the arrow and the target
            glm::vec3 diff = arrow.position - target.position;

            // Compute the squared distance between the arrow and the target center
            float distanceSquared = glm::dot(diff, diff);

            // Increase the effective radius for collision detection
            float expandedRadius = target.radius + 0.5f; // Increase the radius by 0.5 units
            float expandedRadiusSquared = expandedRadius * expandedRadius;

            // Check if the arrow is within the expanded radius of the target sphere
            if (distanceSquared <= expandedRadiusSquared) {
                target.isHit = true; // Mark target as hit
                arrow.isFlying = false; // Stop the arrow
                arrow.position = glm::vec3(0.0f, 1.1f, 5.2f); // Reset arrow position

                // Update score and print a message
                score += 10;
                std::cout << "Target hit! Score: " << score << std::endl;

                break; // Stop after hitting the first target
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
    // Add continuous rotation around the z-axis
    //float rotationAngle = glfwGetTime() * 50.0f; // Adjust speed by changing 50.0f
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 


    shader.setMat4("model", model);

    
    CylinderWithTexture targetSphere(1.0f,1.0f,0.2f, 36, 18,
        glm::vec3(0.1f, 0.1f, 0.8f),
        glm::vec3(0.2f, 0.2f, 0.9f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        32.0f,
        textureID,
        textureID
    );
    

    targetSphere.drawCylinder(shader, model, camera.Position);
}
void Target::update(float deltaTime) {
    if (!isHit) {
        position.x = originalPosition.x + sin(glfwGetTime() * speed) * 2.0f;
    }
}








void onArrowShoot(Arrow& arrow, double mouseX, double mouseY, int screenWidth, int screenHeight, glm::vec3 cameraPos, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
    if (!arrow.isFlying) {
        arrow.isFlying = true;

        // Calculate the direction of the arrow based on mouse click
        glm::vec3 rayDirection = TraceRay(mouseX, mouseY, screenWidth, screenHeight, viewMatrix, projectionMatrix);
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



void checkGameStatus() {
    if (!gameOver) {

        for (const Target& target : targets) {
            if (!target.isHit) {
                allTargetsHit = false;
                break;
            }
        }

        if (score == 30 && level1 ) {
            level1 = false;
            level2 = true;
            gameOver2 = false;
            //gameOver = true; // End game if all targets are hit
            std::cout << "All targets hit at first level! " << std::endl;
            allTargetsHit = true;
            initializeNextRoom2(); // Initialize moving targets
            std::cout << "All targets hit in Level 1! Moving to Level 2..." << std::endl;

        }
        else if (score == 40 && level2) {
            // End game if all moving targets are hit
            gameOver = true;
            std::cout << "All targets hit in Level 2! You win!" << std::endl;
            allTargetsHit = true;

        }


        // Check if the game has timed out
        float elapsedTime = glfwGetTime() - gameStartTime;
        //cout << elapsedTime;

        if (elapsedTime > 38.0f) { // 30 seconds timeout
            gameOver = true;
            allTargetsHit = false;
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


    // Assuming you have a shader initialized
   // Shader lightingShader("vertex_shader_path", "fragment_shader_path");

    // Texture loading
    unsigned int diffuseMapghost = loadTexture("ghostim1.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specularMapghost = loadTexture("ghostim1.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Create the curve
    CurveWithTexture curve("verticesTex.txt", "indicesTex.txt", diffuseMapghost, specularMapghost, 32.0f);


    // Texture loading
    diffuseMapghost = loadTexture("vasesplash.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    specularMapghost = loadTexture("vasesplash.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Create the curve
    CurveWithTexture vase("verticesVase.txt", "indicesVase.txt", diffuseMapghost, specularMapghost, 32.0f);


    //chest
    glm::vec3 ambient(0.5f, 0.3f, 0.2f);
    glm::vec3 diffuse(0.5f, 0.3f, 0.2f);
    glm::vec3 specular(0.5f, 0.5f, 0.5f);




    string frontTexturePath = "ss.jpg";  // Front face texture (e.g., with a lock)
    string otherFacesTexturePath = "shin1.jpg";  // Other faces texture (e.g., plain wood)
    unsigned int frontTexture = loadTexture(frontTexturePath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int otherTexture = loadTexture(otherFacesTexturePath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Chest chestCube = Chest(ambient, diffuse, specular, frontTexture, otherTexture, 32.0f);



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
    unsigned int globeDiffuseMap = loadTexture("crystal.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int globeSpecularMap = loadTexture("crystal.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);


    SphereWithTexture globe(globeRadius, globeSectorCount, globeStackCount,
        globeAmbient, globeDiffuse, globeSpecular,
        globeShininess, globeDiffuseMap, globeSpecularMap);

    


    unsigned int hemiDiffuseMap2 = loadTexture("gg.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    HemiWithTex gold(1.0f, 36, 18,
        glm::vec3(0.2f, 0.15f, 0.05f), // Ambient color (a soft golden base color)
        glm::vec3(0.8f, 0.6f, 0.2f),  // Diffuse color (the bright golden shine under light)
        glm::vec3(0.9f, 0.8f, 0.5f),  // Specular color (highlights with a golden tint)
        32.0f,                       // Shininess
        hemiDiffuseMap2,              // Diffuse texture
        hemiDiffuseMap2              // Specular texture
    );

    unsigned int coneMap = loadTexture("conetree.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    ConeWithTexture selcone(1.0f, 2.0, 20,
        glm::vec3(1.0f, 0.7f, 0.7f), // Ambient color 
        glm::vec3(1.0f, 0.7f, 0.7f),  // Diffuse color 
        glm::vec3(0.1f, 0.1f, 0.1f),  // Specular color 
        32.0f,                       // Shininess
        coneMap,              // Diffuse texture
        coneMap              // Specular texture
    );

    unsigned int stoneMap = loadTexture("crystal2.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    CylinderWithTexture stoneObstacle(1.0f, 1.0f, 2.0f, 50, 20, glm::vec3(0.8f, 0.4f, 0.6f), // Ambient color (soft pink)
        glm::vec3(1.0f, 0.5f, 0.7f), // Diffuse color (brighter pink)
        glm::vec3(0.3f, 0.1f, 0.2f), // Specular color (subtle pink reflection)
        //glm::vec3(0.5f, 0.5f, 0.5f), // Ambient color (medium gray)
        //glm::vec3(0.6f, 0.6f, 0.6f), 
        //glm::vec3(1.0f, 0.0f, 0.0f), // Ambient color (medium gray)
        //glm::vec3(0.8f, 0.0f, 0.0f),
        // // Diffuse color (slightly lighter gray)
        //glm::vec3(0.1f, 0.0f, 0.0f),  // Specular color (low specular reflection)
        32.0f,                       // Shininess
        stoneMap,              // Diffuse texture
        stoneMap              // Specular texture
    );

    // Example texture loading (adjust paths accordingly)
    unsigned int diffuseMap = loadTexture("container2.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specularMap = loadTexture("container2.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
   // unsigned int stoneMap = loadTexture("crystal2.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Create the half-cylinder
    halfCylinderWithTexture halfCylinder(1.0f, 1.0f, 2.0f, 20, 1,
        glm::vec3(0.2f, 0.2f, 0.2f), // Ambient color
        glm::vec3(0.8f, 0.8f, 0.8f), // Diffuse color
        glm::vec3(1.0f, 1.0f, 1.0f), // Specular color
        32.0f,                       // Shininess
        diffuseMap,                  // Diffuse texture
        specularMap                  // Specular texture
    );

    glm::vec3 amb = glm::vec3(1.0f);
    glm::vec3 diff = glm::vec3(0.8f);
    glm::vec3 spec = glm::vec3(0.8f);
    float shiny = 30.0f;
    // Load textures
    unsigned int diffuseMapMonalisa = loadTexture("vinci.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specularMapMonalisa = loadTexture("vinci.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

   

    // Create cube
    LeftFaceTexturedCube cubeVinci(diffuseMapMonalisa, specularMapMonalisa,amb,diff,spec,shiny);


    // Load textures
  diffuseMapMonalisa = loadTexture("vinci1.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
   specularMapMonalisa = loadTexture("vinci1.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);



    // Create cube
    LeftFaceTexturedCube cubeVinci1(diffuseMapMonalisa, specularMapMonalisa, amb, diff, spec, shiny);

    // Front Wall (with a door gap)


    // Texture IDs
    unsigned int diffuseMapW = loadTexture("concrete.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specularMapW = loadTexture("concrete.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Create a textured cube
    Cube backwall(diffuseMapW, specularMapW, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    //Cube side1(diffuseMapW, specularMapW, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    //Cube side2(diffuseMapW, specularMapW, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);


     // Door
    unsigned int diffuseMapDoor = loadTexture("container2.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specularMapDoor = loadTexture("container2.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Create a textured cube
    Cube Door(diffuseMapDoor, specularMapDoor, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Door behind
    diffuseMapDoor = loadTexture("nun.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    specularMapDoor = loadTexture("nun.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Create a textured cube
    Cube DoorB(diffuseMapDoor, specularMapDoor, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Load textures
    diffuseMap = loadTexture("car2.jpg",GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int wheelDiffuseMap = loadTexture("car.jpg",GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Create car body and wheels
    Cube carBody(diffuseMap, diffuseMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    CylinderWithTexture wheel(0.2f, 0.2f, 0.1f, 20, 1, glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(1.0f), 32.0f, wheelDiffuseMap, wheelDiffuseMap);

    float lastFrame = 0.0f;








    initializeObjects(globe, stoneObstacle, chestCube, gold, vase,selcone);









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
        float currentTime = glfwGetTime();
        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set background color to black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear screen and depth buffer




        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // point light 1
        pointlight1.setUpPointLight(lightingShader, currentTime);
        // point light 2
        pointlight2.setUpPointLight(lightingShader, currentTime);

        spotlight.setUpSpotLight(lightingShader);
        directionallight.setUpDirectionalLight(lightingShader);








        // activate shader
        lightingShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        // // Replace glm::perspective with manual calculation
        float fov = glm::radians(45.0f); // Convert degrees to radians
        //float fov = camera.Zoom;
        float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
        float near = 0.1f;
        float far = 100.0f;

        projection = PerspectiveProjection(fov, aspect, near, far);

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

            
          translateMatrix = glm::translate(model, glm::vec3(-2.8f, 1.2f, -8.5f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.150f, 0.150f, 0.150f));

            globe.drawSphere(lightingShader, scaleMatrix,camera.Position);

            //globe handle

            //glm::mat4 modelForCylinder = glm::mat4(1.0f);
            translateMatrix = glm::translate(model, glm::vec3(-3.5f, 0.7f, -3.0f));
            scaleMatrix = glm::scale(translateMatrix, glm::vec3(0.25f, 0.25f, 0.25f));

            //handle.drawCylinder(lightingShader, translateMatrix);

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

            //halfCylinder.drawCylinder(lightingShader, translateMatrix);





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
            pointlight1.setUpPointLight2(*activeShader, pointOn1, currentTime);
            pointlight2.setUpPointLight2(*activeShader, pointOn2, currentTime);
            //spotlight.setUpSpotLight(*activeShader);
            //directionallight.setUpDirectionalLight();

            glm::vec3 viewPos = camera.Position; // Assuming you have a camera object
            unsigned int textureDiffuse = globeDiffuseMap; // Replace with actual diffuse texture
            unsigned int textureSpecular = globeSpecularMap; // Replace with actual specular texture





            // Render objects using the active shader
            // 
            for (Object& obj : objects) {
                obj.draw(*activeShader, viewPos);
                drawBoundingSphere(*activeShader, obj);

            }
           
            //backwall+door

            //door img
            // Front Wall (with a door gap)
            glm::mat4 doorTransformB = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -8.6f));
            doorTransformB = glm::scale(doorTransformB, glm::vec3(1.0f, 2.0f, 0.05f));


            // Draw the wall
            DoorB.drawCubeWithTexture(*activeShader, doorTransformB);

            glm::mat4 doorTransform;
            handleDoorInput(window, deltaTime, doorTransform);

            // Render the door using the updated transform
            Door.drawCubeWithTexture(*activeShader, doorTransform);





           

            // Front Wall (with a door gap)
            glm::mat4 frontWallTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -0.51f, -8.7f));
            frontWallTransform = glm::scale(frontWallTransform, glm::vec3(7.5f, 2.5f, 0.1f));


            // Draw the wall
            backwall.drawCubeWithTexture(*activeShader, frontWallTransform);


            /*
            */
            // Front Wall (with a door gap)
            frontWallTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-4.1f, -.51f, -8.6f));
            frontWallTransform = glm::scale(frontWallTransform, glm::vec3(0.1, 2.5f, 10.0f));


            // Draw the wall
            //side1.drawCubeWithTexture(*activeShader, frontWallTransform);

            // Front Wall (with a door gap)
            frontWallTransform = glm::translate(glm::mat4(1.0f), glm::vec3(3.5f, -.51f, -8.6f));
            frontWallTransform = glm::scale(frontWallTransform, glm::vec3(0.1, 2.5f, 10.0f));


            // Draw the wall
            //side2.drawCubeWithTexture(*activeShader, frontWallTransform);

         


            //car wheel
            // Render the car
            renderCar(carBody, wheel, *activeShader, deltaTime);





            //ghost

            // Define animation parameters
            float amplitude = 0.5f;         // Range of vertical movement
            float speed = 2.0f;             // Speed of oscillation
            float rotationSpeed = 180.0f;   // Rotation speed in degrees per second

            // State variables
            static bool rotating = false;    // Is the object rotating?
            static float rotationAngle = 0.0f; // Current rotation angle
            static float lastTopTime = 0.0f; // Time when the object reached the top

            // In the rendering loop
            float currentTime = glfwGetTime(); // Get the elapsed time

            // Calculate the animated Y position
            float animatedY = amplitude * sin(speed * currentTime);

            // Initial position of the curve
            glm::vec3 basePosition = glm::vec3(2.5f, 0.0f, -7.5f);

            // Check if the object is at the top of the float
            if (!rotating && animatedY > amplitude - 0.01f) {
                rotating = true; // Start rotation
                lastTopTime = currentTime;
            }

            // Handle rotation at the top
            if (rotating) {
                // Increment rotation angle based on elapsed time
                rotationAngle += rotationSpeed * (currentTime - lastTopTime);
                lastTopTime = currentTime; // Update last rotation time

                // If the object completes a full rotation, stop rotating
                if (rotationAngle >= 360.0f) {
                    rotationAngle = 0.0f; // Reset rotation angle
                    rotating = false;     // Resume floating
                }
            }

            // Construct the model matrix
            glm::mat4 model4 = glm::mat4(1.0f);
            model4 = glm::translate(model4, basePosition);                // Place the curve at the base position
            model4 = glm::translate(model4, glm::vec3(0.0f, animatedY, 0.0f)); // Apply floating animation
            model4 = glm::rotate(model4, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around its center
            model4 = glm::scale(model4, glm::vec3(0.4f, 0.6f, 0.4f));     // Scale the object

            // Render the curve
            viewPos = camera.Position; // Replace with your actual camera position
            curve.render(*activeShader, model4, viewPos);


            /* // Define animation parameters
            float amplitude = 0.5f;   // Range of movement (y-axis)
            float speed = 2.0f;       // Speed of oscillation
            float rotationSpeed = 180.0f; // Rotation speed in degrees per second

            // State variables
            static bool rotating = false;    // Is the object rotating?
            static float rotationAngle = 0.0f; // Current rotation angle
            static float lastTopTime = 0.0f; // Time when the object reached the top

            // In the rendering loop
            float currentTime = glfwGetTime(); // Get the elapsed time

            // Calculate the animated Y position
            float animatedY = amplitude * sin(speed * currentTime);

            glm::mat4 model4 = glm::mat4(1.0f);

            // Check if the object is at the top of the float
            if (!rotating && animatedY > amplitude - 0.01f) {
                rotating = true; // Start rotation
                lastTopTime = currentTime;
            }

            // Handle rotation at the top
            if (rotating) {
                // Increment rotation angle based on elapsed time
                rotationAngle += rotationSpeed * (currentTime - lastTopTime);
                lastTopTime = currentTime; // Update last rotation time

                // If the object completes a full rotation, stop rotating
                if (rotationAngle >= 360.0f) {
                    rotationAngle = 0.0f; // Reset rotation angle
                    rotating = false;     // Resume floating
                }
            }
            else {
                // Standard floating animation
                model4 = glm::translate(model4, glm::vec3(2.5f, animatedY, -6.0f)); // Apply floating animation
            }

            // Apply rotation (if rotating or not)
            model4 = glm::rotate(model4, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis
            model4 = glm::scale(model4, glm::vec3(0.4f, 0.6f, 0.4f)); // Scale the object

            // Render the curve
            viewPos = camera.Position; // Replace with your actual camera position
            curve.render(*activeShader, model4, viewPos);*/

           


            /*
            *  // Define animation parameters
            float amplitude = 0.5f; // Range of movement (y-axis)
            float speed = 2.0f;     // Speed of oscillation

            // In the rendering loop
            float currentTime = glfwGetTime(); // Get the elapsed time

            glm::mat4 model4 = glm::mat4(1.0f);
            float animatedY = amplitude * sin(speed * currentTime); // Calculate animated y position

            model4 = glm::translate(model4, glm::vec3(2.5f, animatedY, -6.0f)); // Apply animation
            model4 = glm::scale(model4, glm::vec3(0.4f, 0.6f, 0.4f));

            viewPos = camera.Position; // Replace with your actual camera position

            curve.render(*activeShader, model4, viewPos);
            */
           

            /*
            *   // In the rendering loop
            glm::mat4 model4 = glm::mat4(1.0f);
            model4 = glm::translate(model4, glm::vec3(2.5f, 0.0f, -6.0f)); // Example translation
            model4 = glm::scale(model4, glm::vec3(0.4f, 0.6f, 0.4f));
            viewPos = camera.Position; // Replace with your actual camera position

            curve.render(*activeShader, model4, viewPos);*/

         



            

  


            // Render the vINCI
            glm::mat4 modelv = glm::mat4(1.0f);
            modelv = glm::translate(modelv, glm::vec3(3.4f, 0.5f, -3.0f));
            modelv = glm::scale(modelv, glm::vec3(0.03f, 1.0f, 1.5f));
            cubeVinci.drawCubeWithTexture(*activeShader, modelv);

            glm::mat4 modelvv = glm::mat4(1.0f);
            modelvv = glm::translate(modelvv, glm::vec3(-3.9f, 0.5f, -3.0f));
            modelvv = glm::rotate(modelvv, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelvv = glm::scale(modelvv, glm::vec3(0.03f, 1.f, 0.7f));
           
            cubeVinci1.drawCubeWithTexture(*activeShader, modelvv);


            // Model transformation for the half-cylinder
            glm::mat4 model1 = glm::mat4(1.0f);
            model = glm::translate(model1, glm::vec3(-2.0f, 0.0f, 0.0f)); // Move to desired position
            model = glm::rotate(model1, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate if needed
            model = glm::scale(model1, glm::vec3(0.4f, 0.5f, 0.4f));

           
            // Mouse click logic to verify the WC of the clicked point
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                // Get the mouse cursor position in screen space
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);

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

            // Check game status and render based on level
            checkGameStatus();
            if (level1) {
                renderLevel2(lightingShaderWithTexture, window, view, projection); // Level 1 rendering
            }
            else if (level2) {
                renderLevel1(lightingShaderWithTexture, window, view, projection); // Level 2 rendering
            }
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
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        eyeX = initialEyeX;
        eyeY = initialEyeY;
        eyeZ = initialEyeZ;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
        //std::cout << "Camera reset to initial position: ("
            //<< eyeX << ", " << eyeY << ", " << eyeZ << ")" << std::endl;
    }
    static bool key1Pressed = false;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!key1Pressed) { // Only toggle if the key was not already pressed
            key1Pressed = true; // Mark the key as pressed

            if (directionalOn) {
                directionallight.turnOff();
                directionalOn = false;
            }
            else {
                directionallight.turnOn();
                directionalOn = true;
            }
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
        key1Pressed = false; // Reset the key state when the key is released
    }


    // State variables for debouncing
    static bool key2Pressed = false;
    static bool key3Pressed = false;
    static bool key4Pressed = false;
    static bool key5Pressed = false;
    static bool key6Pressed = false;
    static bool key7Pressed = false;

    // Toggle pointlight1 (Key 2)
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!key2Pressed) {
            key2Pressed = true; // Mark key as pressed
            if (pointOn1) {
                pointlight1.turnOff();
                pointOn1 = false;
            }
            else {
                pointlight1.turnOn();
                pointOn1 = true;
            }
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
        key2Pressed = false; // Reset key state
    }

    // Toggle pointlight2 (Key 3)
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (!key3Pressed) {
            key3Pressed = true;
            if (pointOn2) {
                pointlight2.turnOff();
                pointOn2 = false;
            }
            else {
                pointlight2.turnOn();
                pointOn2 = true;
            }
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
        key3Pressed = false;
    }

    // Toggle spotlight (Key 4)
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        if (!key4Pressed) {
            key4Pressed = true;
            if (spotOn) {
                spotlight.turnOff();
                spotOn = false;
            }
            else {
                spotlight.turnOn();
                spotOn = true;
            }
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) {
        key4Pressed = false;
    }

    // Toggle ambient lighting (Key 5)
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        if (!key5Pressed) {
            key5Pressed = true;
            if (ambientToggle) {
                pointlight1.turnAmbientOff();
                pointlight2.turnAmbientOff();
                directionallight.turnAmbientOff();
                spotlight.turnAmbientOff();
                ambientToggle = false;
            }
            else {
                pointlight1.turnAmbientOn();
                pointlight2.turnAmbientOn();
                directionallight.turnAmbientOn();
                spotlight.turnAmbientOn();
                ambientToggle = true;
            }
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE) {
        key5Pressed = false;
    }

    // Toggle diffuse lighting (Key 6)
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        if (!key6Pressed) {
            key6Pressed = true;
            if (diffuseToggle) {
                pointlight1.turnDiffuseOff();
                pointlight2.turnDiffuseOff();
                directionallight.turnDiffuseOff();
                spotlight.turnDiffuseOff();
                diffuseToggle = false;
            }
            else {
                pointlight1.turnDiffuseOn();
                pointlight2.turnDiffuseOn();
                directionallight.turnDiffuseOn();
                spotlight.turnDiffuseOn();
                diffuseToggle = true;
            }
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_RELEASE) {
        key6Pressed = false;
    }

    // Toggle specular lighting (Key 7)
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        if (!key7Pressed) {
            key7Pressed = true;
            if (specularToggle) {
                pointlight1.turnSpecularOff();
                pointlight2.turnSpecularOff();
                directionallight.turnSpecularOff();
                spotlight.turnSpecularOff();
                specularToggle = false;
            }
            else {
                pointlight1.turnSpecularOn();
                pointlight2.turnSpecularOn();
                directionallight.turnSpecularOn();
                spotlight.turnSpecularOn();
                specularToggle = true;
            }
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_RELEASE) {
        key7Pressed = false;
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
        glm::vec3 rayDirection = TraceRay(mouseX, mouseY, SCR_WIDTH, SCR_HEIGHT, camera.GetViewMatrix(), projection);

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
void drawTableChair(Shader& shaderProgram, unsigned int VAO, glm::mat4 parentTrans)
{
    shaderProgram.use();
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, modelCentered;

    //table 
    model = parentTrans * glm::scale(identityMatrix, glm::vec3(3.5f, 0.2f, 2.0f));
    drawCube(VAO, shaderProgram, model, 0.9176f, 0.7020f, 0.0314f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    model = parentTrans * glm::scale(identityMatrix, glm::vec3(0.2f, -2.0f, 0.2f));
    drawCube(VAO, shaderProgram, model, 0.9176f, 0.7020f, 0.0314f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 0.9));
    model = parentTrans * glm::scale(translateMatrix, glm::vec3(0.2f, -2.0f, 0.2f));
    drawCube(VAO, shaderProgram, model, 0.9176f, 0.7020f, 0.0314f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.65, 0.0, 0.9));
    model = parentTrans * glm::scale(translateMatrix, glm::vec3(0.2f, -2.0f, 0.2f));
    drawCube(VAO, shaderProgram, model, 0.9176f, 0.7020f, 0.0314f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.65, 0.0, 0.0));
    model = parentTrans * glm::scale(translateMatrix, glm::vec3(0.2f, -2.0f, 0.2f));
    drawCube(VAO, shaderProgram, model, 0.9176f, 0.7020f, 0.0314f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.55f, 0.1f, 0.50f));
    model = parentTrans * glm::scale(translateMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
    drawCube(VAO, shaderProgram, model, 0.0f, 0.0f, 0.0f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.67f, 0.15f, 0.75f));
    model = parentTrans * glm::scale(translateMatrix, glm::vec3(0.5f, 0.1f, 0.50f));
    drawCube(VAO, shaderProgram, model, 1.0f, 1.0f, 1.0f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  


}
void MaterialSpec(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float ra = 1.0f, float ga = 1.0f, float ba = 1.0f, float rd = 1.0f, float gd = 1.0f, float bd = 1.0f, float rs = 1.0f, float gs = 1.0f, float bs = 1.0f, float shininess = 32.0f, float re = 0.0f, float ge = 0.0f, float be = 0.3f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(ra, ga, ba));
    lightingShader.setVec3("material.diffuse", glm::vec3(rd, gd, bd));
    lightingShader.setVec3("material.specular", glm::vec3(rs, gs, bs));
    lightingShader.setFloat("material.shininess", shininess);
    lightingShader.setVec3("material.emissive", glm::vec3(re, ge, be));

    lightingShader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

void DrawRoom(Shader& shaderProgram, unsigned int VAO, glm::mat4 model) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);


    glm::mat4 floorTransform = glm::translate(identityMatrix, glm::vec3(-4.0f, -.51f, -8.7f)); // Floor at y = -1.0f
    floorTransform = glm::scale(floorTransform, glm::vec3(15.0f, 0.1f, 20.5f)); // Large floor
    floorTransform = floorTransform * model;
    MaterialSpec(
        VAO,
        shaderProgram,
        floorTransform,
        0.2f, 0.1f, 0.05f, // Ambient: ra, ga, ba
        0.4f, 0.2f, 0.1f,  // Diffuse: rd, gd, bd
        0.2f, 0.1f, 0.05f, // Specular: rs, gs, bs
        16.0f,             // Shininess
        0.0f, 0.0f, 0.0f   // Emissive: re, ge, be
    );

   
    
   

    glm::mat4 translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.8f, 1.0f, -8.5f));
    glm::mat4 scaleMatrix = glm::scale(translateMatrix, glm::vec3(3.8f, 0.1f, 1.5f));


    MaterialSpec(
        VAO,
        shaderProgram,
        scaleMatrix,
        0.3f, 0.3f, 0.3f, // Ambient: ra, ga, ba
        0.5f, 0.5f, 0.5f, // Diffuse: rd, gd, bd
        0.2f, 0.2f, 0.2f, // Specular: rs, gs, bs
        16.0f,            // Shininess
        0.0f, 0.0f, 0.0f  // Emissive: re, ge, be
    );


   
    // Left Wall
    glm::mat4 leftWallTransform = glm::translate(identityMatrix, glm::vec3(-4.0f, -.51f, -8.7f));
    leftWallTransform = glm::scale(leftWallTransform, glm::vec3(0.1f, 5.0f, 20.5f));
    leftWallTransform = leftWallTransform * model;
    MaterialSpec(
        VAO,
        shaderProgram,
        leftWallTransform,
        0.8f, 0.8f, 0.8f, // Ambient: ra, ga, ba
        1.0f, 1.0f, 1.0f, // Diffuse: rd, gd, bd
        0.9f, 0.9f, 0.9f, // Specular: rs, gs, bs
        32.0f,            // Shininess
        0.0f, 0.0f, 0.0f  // Emissive: re, ge, be
    );



    // Right 
    glm::mat4 rightWallTransform = glm::translate(identityMatrix, glm::vec3(3.5f, -.51f, -8.7f)); // Position of the right wall
    rightWallTransform = glm::scale(rightWallTransform, glm::vec3(0.1f, 5.0f, 20.5)); // Wall dimensions
    rightWallTransform = rightWallTransform * model;
    MaterialSpec(
        VAO,
        shaderProgram,
        rightWallTransform,
        0.8f, 0.8f, 0.8f, // Ambient: ra, ga, ba
        1.0f, 1.0f, 1.0f, // Diffuse: rd, gd, bd
        0.9f, 0.9f, 0.9f, // Specular: rs, gs, bs
        32.0f,            // Shininess
        0.0f, 0.0f, 0.0f  // Emissive: re, ge, be
    );



}





void classroom(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix;


    DrawRoom(lightingShader, cubeVAO, model);

    

    glm::mat4 mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

    mat = mat * model;

 
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.8f, 0.3f, -8.3f));
    mat = translateMatrix * glm::scale(glm::mat4(0.8f), glm::vec3(1.0f,0.8f,1.0f));
    drawTableChair(lightingShader, cubeVAO, mat);
    mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    scaleMatrix = glm::scale(mat, glm::vec3(0.50f, 0.50f, 0.50f));

    mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));


    scaleMatrix = glm::scale(mat, glm::vec3(0.50f, 0.50f, 0.50f));


}




unsigned int loadTexture(const char* path, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter) {
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

    stbi_image_free(data); 
    return textureID;
}



glm::vec3 getTransformedPosition(const Object& obj) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, obj.position);
    model = glm::rotate(model, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    return glm::vec3(model[3]); // Extract the transformed position from the matrix
}

bool intersectObject(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 sphereCenter, float radius) {
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
    glm::vec3 rayDirection = TraceRay(mouseX, mouseY, screenWidth, screenHeight, viewMatrix, projectionMatrix);

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
        if (intersectObject(cameraPos, rayDirection, transformedPosition, testr)) {
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

void renderLevel2(Shader& shader, GLFWwindow* window, glm::mat4 view, glm::mat4 projection) {
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    if (gameOver2) {
        renderLevel1(shader, window, view, projection); // Transition to moving targets
    }
    else {
        for (Target& target : targets) {
            target.update(deltaTime); // Update targets (including motion)
        }

        arrow.update(deltaTime); // Update arrow position
        isTargetHit(arrow); // Check for collisions

        // Render targets
        for (Target& target : targets) {
            if (!target.isHit) {
                target.draw(shader);
            }
        }

        // Render arrow
        arrow.draw(shader);

        // Render trajectory if the arrow is not flying
        if (!arrow.isFlying) {
            glm::vec3 start = arrow.position;
            glm::vec3 direction = arrow.direction;
            std::vector<glm::vec3> trajectoryPoints = generateTrajectoryPoints(start, direction, 10, 0.5f);

            updateTrajectoryBuffer(trajectoryPoints);
            renderTrajectory(shader, trajectoryPoints);
        }
    }
}


void renderLevel1(Shader& shader, GLFWwindow* window, glm::mat4 view, glm::mat4 projection) {
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);



    if (gameOver) {

        // If the game is over, show the win/lose animation
        if (allTargetsHit) {
            // Animation parameters
            float elapsedTime = glfwGetTime();
            float rotationAngle = elapsedTime * 30.0f; // Rotate 30 degrees per second
            float pulsateFactor = 1.0f + 0.1f * sin(elapsedTime * 3.0f); // Pulsate effect

            // Base (Cylinder)
            glm::mat4 modelBase = glm::mat4(1.0f);
            modelBase = glm::translate(modelBase, glm::vec3(0.0f, -2.5f, -10.0f));
            modelBase = glm::scale(modelBase, glm::vec3(2.0f * pulsateFactor, 1.5f, 2.0f));
            shader.setMat4("model", modelBase);

            unsigned int baseTexture = loadTexture("gold.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
            SphereWithTexture base(1.0f, 36, 18,
                glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f), 32.0f,
                baseTexture, baseTexture);
            base.drawSphere(shader, modelBase, camera.Position);


            //body.drawCube(shader, modelBody);
            /*// Top (Cone)
            glm::mat4 modelTop = glm::mat4(1.0f);
            modelTop = glm::translate(modelTop, glm::vec3(0.0f, -0.9f, -10.0f));
            modelTop = glm::rotate(modelTop, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            modelTop = glm::scale(modelTop, glm::vec3(2.0f * pulsateFactor));
            shader.setMat4("model", modelTop);

            unsigned int topTexture = loadTexture("gold.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
            ConeWithTexture top(1.0f, 2.0f, 36,
                glm::vec3(0.2f, 0.4f, 0.9f), glm::vec3(0.3f, 0.6f, 1.0f), glm::vec3(1.0f), 64.0f,
                topTexture, topTexture);
            top.drawCone(shader, modelTop, camera.Position);*/

            
        }

        else {
            //std::cout << "Executing else block for losing condition." << std::endl;

            // Render the sad emoji sphere for losing with animation
            glm::mat4 model = glm::mat4(1.0f);

            // Get the elapsed time for animation effects
            float elapsedTime = glfwGetTime();

            // Pulsate the size of the sphere
            float pulsateFactor = 1.0f + 0.1f * sin(elapsedTime * 3.0f);

            // Rotate the sphere slowly
            float rotationAngle = elapsedTime * 20.0f; // 20 degrees per second

            string frontTexturePath2 = "shoot2.jpg";  // Front face texture (e.g., with a lock)
            string otherFacesTexturePath2 = "shoot2.jpg";  // Other faces texture (e.g., plain wood)
            unsigned int frontTexture2 = loadTexture(frontTexturePath2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
            unsigned int otherTexture2 = loadTexture(otherFacesTexturePath2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
            Chest failed = Chest(glm::vec3(1.0f, 1.0f, 1.0f),    // Ambient color
                glm::vec3(1.0f, 1.0f, 1.0f),    // Diffuse color
                glm::vec3(0.5f, 0.5f, 0.5f), frontTexture2, otherTexture2, 32.0f);

            // Apply transformations: translation, scaling, and rotation
            model = glm::translate(model, glm::vec3(0.0f, -2.0f, -10.0f));  // Position the sphere
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis
            model = glm::scale(model, glm::vec3(3.0f * pulsateFactor)); // Apply pulsation effect
            shader.setMat4("model", model);


            failed.drawCubeWithTwoTextures(shader, model);

   



        }
    }
    else {
        // Update all targets
        for (Target& target : targets) {
            target.update(deltaTime);
        }

        // Update and render the arrow
        arrow.update(deltaTime);
        isTargetHit(arrow);


        // Render targets
        for (Target& target : targets) {
            if (!target.isHit) {
                target.draw(shader);
            }
        }
        arrow.draw(shader);

        // Render trajectory if the arrow is not flying
        if (!arrow.isFlying) {
            glm::vec3 start = arrow.position;
            glm::vec3 direction = arrow.direction;
            std::vector<glm::vec3> trajectoryPoints = generateTrajectoryPoints(start, direction, 10, 0.5f);

            updateTrajectoryBuffer(trajectoryPoints);
            renderTrajectory(shader, trajectoryPoints);
        }

        // Optional: Print the score to the console
        //std::cout << "Score: " << score << std::endl;
    }

}









