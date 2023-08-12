#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Shader class from learnopengl.com:
// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h
#include <COMP371_assignment1/shader_s.h>
#include <COMP371_assignment1/stb_image.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void renderScene(Shader shader, glm::mat4 baseMat, unsigned int VAOs[6]);
void renderGrid(Shader shader, glm::mat4 baseMat, unsigned int VAOs[6]);
void renderRacket(Shader shader, glm::mat4 baseMat, unsigned int VAOs[6], int miniController);

// Settings (for easy adjustability)
int SCR_WIDTH = 1024;
int SCR_HEIGHT = 768;
//const unsigned int SHADOW_WIDTH = 1024;
//const unsigned int SHADOW_HEIGHT = 1024;


// camera
// angle
float pitch = -45.0f;
float yaw = 0.0f;
// position
glm::vec3 position = glm::vec3(-15.0f, 30.0f, 0.0f);
// relative directions (relative to the camera itself and where it is looking)
glm::vec3 front;
glm::vec3 right;
glm::vec3 up;
// constants
const float cameraMoveSpeed = 0.5f;
const float cameraAngleSpeed = 0.95f;

// name order within the position/rotation arrays
// Craig, Sergio Afaf Kelly, Jordan

// world rotations
float rotationx = 0.0f;
float rotationy = 0.0f;
float rotationz = 0.0f;
// world rotation speed
const float rotationFactor = 2.5f;

// complete model scaling
float scaleFactor[5] = { 0.5f, 0.5f, 1.0f, 1.0f, 1.0f };
const float scaleFactorModifier = 0.005f;

// complete model movement
float racketposx[5] = { 0.0f, 0.0f, -10.0f, 10.0f, 10.0f };
float racketposy[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float racketposz[5] = { 10.0f, -10.0f, 10.0f, -10.0f, 10.0f };
const float racketMoveSpeed = 0.5f;

// lower arm rotations
float larmrotx[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float larmroty[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float larmrotz[5] = { 315.0f, 315.0f, 0.0f, 340.0f, 45.0f };

// upper arm rotations
float uarmrotx[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float uarmroty[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float uarmrotz[5] = { 45.0f, 45.0f, 0.0f, 30.0f, 45.0f };

// racket rotations
float racketrotx[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float racketroty[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float racketrotz[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

// switch var to determine which part of the model should be rotated
int subject = 0;
int controller = 0;
bool canChangeController = true;

bool canSelect = true;
bool canTeleport = true;

// shadow toggle
bool shadowActive = true;
bool canToggleShadow = true;

// texture toggle
bool textureToggle = true;
bool canTextureToggle = true;
// Texture Declaration
unsigned int glossy;
unsigned int clay;
unsigned int triColor;
unsigned int tennisBall;
unsigned int white;
// lighting
glm::vec3 lightPos(0.0f, 30.0f, 0.0f);

// sphere/collision system
glm::mat4 sphereModelMat = glm::mat4(1.0f);
bool sphereDir = true;
glm::vec3 sphereMovement(0.0f, 0.0f, 0.1f);
const glm::vec3 DIST(5.0f, 5.0f, 1.0f);
int intersector;
glm::mat4 finalRacketModelMat[2] = {glm::mat4(1.0f), glm::mat4(1.0f)};
glm::mat4 finalRacketRotations[2] = { glm::mat4(1.0f), glm::mat4(1.0f) };
const float gravity = 0.005f;
const float racketBounce = 0.1f;
const float groundBounce = 0.3f;
int groundBounceTimer = 0;
int racketBounceTimer = 0;

// collision system - hitbox
glm::vec3 Xvector = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 Yvector = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 Zvector = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 hitbox = glm::vec3(1.0f, 1.0f, 1.0f);

const int sphereStacks = 128; // Number of horizontal cuts
const int sphereSectors = 256; // Number of vertical cuts
glm::vec3 sphereVertexArray[3 * (sphereStacks * sphereSectors + 2)]; // List of all vertices
unsigned int sphereIndexArray[6 * sphereSectors * sphereStacks]; // Order of vertices for EBO


// Afaf's global variables 
// starting points of model
float modelPositionX = 0.267f;
float modelPositionY = 0.01f;
float modelPositionZ = -0.5f;

// Instantiates all position and texture position values for the sphereVertexArray for given stacks and sectors
void defineSphereVertexArray() {
    float phi = -180.0f, theta = 0.f; // Spherical coordinates (radius rho assumed to be 1)
    int index = 1;

    // Bottom of sphere
    sphereVertexArray[0] = glm::vec3(0.0f, -1.0f, 0.0f); // Position
    sphereVertexArray[1] = glm::vec3(0.0f, 0.0f, 0.0f); // Texture UV

    // Top of sphere
    sphereVertexArray[3 * (sphereStacks * sphereSectors + 1)] = glm::vec3(0.0f, 1.0f, 0.0f); // Position
    sphereVertexArray[3 * (sphereStacks * sphereSectors + 1) + 1] = glm::vec3(0.0f, 1.0f, 0.0f); // Texture UV


    for (int i = 1; i < sphereStacks + 1; i++) {
        phi = -180.0f + 180.0f * ((float)i / (sphereStacks + 1)); // Rotate counter-clockwise around x-axis

        for (int j = 0; j < sphereSectors; j++) {
            theta = 360.0f * ((float)j / sphereSectors); // Rotate couter-clockwise around y-axis

            glm::vec3 vector = glm::vec3(glm::sin(glm::radians(phi)) * glm::cos(glm::radians(theta)),
                glm::cos(glm::radians(phi)),
                glm::sin(glm::radians(phi)) * glm::sin(glm::radians(theta))); // Define rectangular coordinates in terms of spherical coordinates
            sphereVertexArray[3 * index] = vector;

            sphereVertexArray[3 * index + 1] = glm::vec3(theta / 360.0, (phi + 180.0f) / 180.0f, 0.0f); // UV positions texture
            index++;
        }
    }
}

// Computes the normal vector of a face with three given vectors, and adds said normal vector to the vertex normal
void computeNormals(unsigned int p1, unsigned int p2, unsigned int p3) {
    glm::vec3 p1Pos = sphereVertexArray[3 * p1], p2Pos = sphereVertexArray[3 * p2], p3Pos = sphereVertexArray[3 * p3]; // Positions of relevant vertices
    glm::vec3 faceNormal = glm::normalize(glm::cross(p2Pos - p1Pos, p3Pos - p1Pos)); // Normal vector of the resulting plane

    sphereVertexArray[3 * p1 + 2] += faceNormal;
    sphereVertexArray[3 * p2 + 2] += faceNormal;
    sphereVertexArray[3 * p3 + 2] += faceNormal;
}

//Instantiates all values for the sphereIndexArray for given stacks and sectors
void defineSphereIndexArray() {
    int currentStack = 1, currentSector = 0, positionInVertexArray = 1, positionInIndexArray = 0;

    // Bottom of sphere
    for (currentSector = 0; currentSector < sphereSectors; currentSector++) {
        int nextSector = (currentSector == sphereSectors - 1) ? 0 : currentSector + 1; // Lowest layer: indices [1, sphereSectors + 1]
        int p1 = currentSector + 1, p2 = nextSector + 1; // indices of relevant vertices in vertex array

        // Triangle facing down
        sphereIndexArray[positionInIndexArray++] = 0; // Lowest point
        sphereIndexArray[positionInIndexArray++] = p1;
        sphereIndexArray[positionInIndexArray++] = p2;
        computeNormals(0, p1, p2);
    }

    // Middle of sphere
    for (currentStack = 0; currentStack < sphereStacks - 1; currentStack++) {
        int nextStack = currentStack + 1;

        for (currentSector = 0; currentSector < sphereSectors; currentSector++) {
            int nextSector = (currentSector == sphereSectors - 1) ? 0 : currentSector + 1; // This layer

            // Array positions of corners of current rectangle
            int p1 = currentStack * sphereSectors + currentSector + 1; // Bottom left corner
            int p2 = currentStack * sphereSectors + nextSector + 1; // Bottom right corner
            int p3 = nextStack * sphereSectors + currentSector + 1; // Top left corner
            int p4 = nextStack * sphereSectors + nextSector + 1; // Top right corner

            // First triangle
            sphereIndexArray[positionInIndexArray++] = p3;
            sphereIndexArray[positionInIndexArray++] = p2;
            sphereIndexArray[positionInIndexArray++] = p1;
            computeNormals(p3, p2, p1);

            // Second triangle
            sphereIndexArray[positionInIndexArray++] = p2;
            sphereIndexArray[positionInIndexArray++] = p3;
            sphereIndexArray[positionInIndexArray++] = p4;
            computeNormals(p2, p3, p4);
        }
    }

    // Top of sphere
    for (currentSector = 0; currentSector < sphereSectors; currentSector++) {
        int nextSector = (currentSector == sphereSectors - 1) ? 0 : currentSector + 1; // Highest layer
        // sphereStacks - 1 = stack position highest layer
        // (sphereStacks - 1) * sphereSectors + 1 = array position of first sector of highest layer
        // (sphereStacks - 1) * sphereSectors + currentSector + 1 = array position of current sector in highest layer
        int p1 = (sphereStacks - 1) * sphereSectors + currentSector + 1, p2 = (sphereStacks - 1) * sphereSectors + nextSector + 1; // indices of relevant vertices in vertex array

        // Triange facing up
        sphereIndexArray[positionInIndexArray++] = sphereStacks * sphereSectors + 1; // Highest point
        sphereIndexArray[positionInIndexArray++] = p2;
        sphereIndexArray[positionInIndexArray++] = p1;
        computeNormals(sphereStacks * sphereSectors + 1, p2, p1);
    }
}

struct SphereData { GLuint VAO, VBO, EBO; } bindSphereVAO() {
    
    SphereData data;
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create a vertex array
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertexArray), sphereVertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)0); // attribute 0 = aPos
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)sizeof(glm::vec3)); // attribute 2 = aTexCoords
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)(2 * sizeof(glm::vec3))); // attribute 1 = aNormal
    glEnableVertexAttribArray(2);

    // Upload Index Buffer to the GPU
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphereIndexArray), sphereIndexArray, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Return three values for VAO, VBO and EAO
    data.VBO = VBO;
    data.VAO = VAO;
    data.EBO = EBO;

    return data;
}


int Assignment2(GLFWwindow* window)
{

    Shader firstPass("firstPass.vs", "firstPass.fs");
    Shader shadow("shadow.vs", "shadow.fs");


    // 0 - x gridlines
    float xgridline[] =
    {
        -50.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         50.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };

    // 1 - z gridlines
    float zgridline[] =
    {
        0.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f,  50.0f, 0.0f, 1.0f, 0.0f
    };

    // 3 - vertical net bits
    float netLines[] =
    {
        0.0f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f,
        0.0f, 10.0f, 0.0f, 0.0f, 0.8f, 0.0f
    };

    // 4 - unit cube (base building block for model)
    float unitCubeRepeat[] =
    {
        // XY primary (back face)
       -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f,  2.0f, -1.0f,  0.0f,  0.0f, -1.0f, 3.0f, 3.0f, // top-right
        1.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f, 3.0f, 0.0f, // bottom-right
        1.0f,  2.0f, -1.0f,  0.0f,  0.0f, -1.0f, 3.0f, 3.0f, // top-right
       -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
       -1.0f,  2.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 3.0f, // top-left

       // ZY primary (left face)
       -1.0f,  2.0f,  1.0f, -1.0f,  0.0f,  0.0f, 3.0f, 3.0f, // top-right
       -1.0f,  2.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 3.0f, // top-left
       -1.0f,  0.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
       -1.0f,  0.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
       -1.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f, 3.0f, 0.0f, // bottom-right
       -1.0f,  2.0f,  1.0f, -1.0f,  0.0f,  0.0f, 3.0f, 3.0f, // top-right

       // XZ primary (bottom face)
       -1.0f,  0.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 3.0f, // top-right
        1.0f,  0.0f, -1.0f,  0.0f, -1.0f,  0.0f, 3.0f, 3.0f, // top-left
        1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f, 3.0f, 0.0f, // bottom-left
        1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f, 3.0f, 0.0f, // bottom-left
       -1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
       -1.0f,  0.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 3.0f, // top-right

       // XY secondary (front face)
       -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 3.0f, 0.0f, // bottom-right
        1.0f,  2.0f,  1.0f,  0.0f,  0.0f,  1.0f, 3.0f, 3.0f, // top-right
        1.0f,  2.0f,  1.0f,  0.0f,  0.0f,  1.0f, 3.0f, 3.0f, // top-right
       -1.0f,  2.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 3.0f, // top-left
       -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left

       // ZY secondary (right face)
        1.0f,  2.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 3.0f, // top-left
        1.0f,  0.0f, -1.0f,  1.0f,  0.0f,  0.0f, 3.0f, 0.0f, // bottom-right
        1.0f,  2.0f, -1.0f,  1.0f,  0.0f,  0.0f, 3.0f, 3.0f, // top-right
        1.0f,  0.0f, -1.0f,  1.0f,  0.0f,  0.0f, 3.0f, 0.0f, // bottom-right
        1.0f,  2.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 3.0f, // top-left
        1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left

       // XZ secondary (top face)
       -1.0f,  2.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 3.0f, // top-left
        1.0f,  2.0f , 1.0f,  0.0f,  1.0f,  0.0f, 3.0f, 0.0f, // bottom-right
        1.0f,  2.0f, -1.0f,  0.0f,  1.0f,  0.0f, 3.0f, 3.0f, // top-right  
        1.0f,  2.0f,  1.0f,  0.0f,  1.0f,  0.0f, 3.0f, 0.0f, // bottom-right
       -1.0f,  2.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 3.0f, // top-left
       -1.0f,  2.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
    };

    // 5 - unit cube (base building block for model)
    float unitCube2[] =
    {
        // XY primary (back face)
        -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  2.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  2.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  2.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left

        // ZY primary (left face)
        -1.0f,  2.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  2.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f,  0.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f,  0.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  2.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right

        // XZ primary (bottom face)
        -1.0f,  0.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f,  0.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  0.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right

        // XY secondary (front face)
        -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  2.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  2.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  2.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left

        // ZY secondary (right face)
         1.0f,  2.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f,  0.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  2.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
         1.0f,  0.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  2.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left

        // XZ secondary (top face)
        -1.0f,  2.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  2.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  2.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right  
         1.0f,  2.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  2.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  2.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
    };


    float vertexArray[] = {

        // back face
        // position				//normals			//texture coords
      -0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  	0.0f, 0.0f, -1.0f,	1.0f, 1.0f,
       0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	1.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

      // front face
      -0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
       0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,	    0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,

      //left face
      -0.5f,  0.5f,  0.5f,		-1.0f, 0.0f, 0.0f,	 1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,	 1.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,	 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,	 0.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,		-1.0f, 0.0f, 0.0f,	 0.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,		-1.0f, 0.0f, 0.0f,	 1.0f, 1.0f,

      // right face
       0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
       0.5f,  0.5f, -0.5f,	    1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
       0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
       0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
       0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,

       // bottom face 
      -0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,	1.0f, 1.0f,
       0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,	1.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 0.0f,

      // top face
      -0.5f,  0.5f, -0.5f,   	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
       0.5f,  0.5f, -0.5f,	    0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f


    };


    unsigned int VBOs[6], VAOs[6];
    glGenVertexArrays(6, VAOs);
    glGenBuffers(6, VBOs);

    // x gridlines
    glBindVertexArray(VAOs[0]); //1
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); //2
    glBufferData(GL_ARRAY_BUFFER, sizeof(xgridline), xgridline, GL_STATIC_DRAW); //3
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //4
    glEnableVertexAttribArray(0); //5
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //6
    glEnableVertexAttribArray(1); //7

    // z gridlines
    glBindVertexArray(VAOs[1]); //1
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]); //2
    glBufferData(GL_ARRAY_BUFFER, sizeof(zgridline), zgridline, GL_STATIC_DRAW); //3
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //4
    glEnableVertexAttribArray(0); //5
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //6
    glEnableVertexAttribArray(1); //7

    // vertical net lines
    glBindVertexArray(VAOs[3]); //1
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]); //2
    glBufferData(GL_ARRAY_BUFFER, sizeof(netLines), netLines, GL_STATIC_DRAW); //3
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //4
    glEnableVertexAttribArray(0); //5
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //6
    glEnableVertexAttribArray(1); //7

    // unit cube - model building block
    glBindVertexArray(VAOs[4]); //1
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[4]); //2
    glBufferData(GL_ARRAY_BUFFER, sizeof(unitCubeRepeat), unitCubeRepeat, GL_STATIC_DRAW); //3
    glEnableVertexAttribArray(0); //5
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //4
    glEnableVertexAttribArray(1); //7
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //6
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


    // unit cube - model building block
    glBindVertexArray(VAOs[5]); //1
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[5]); //2
    glBufferData(GL_ARRAY_BUFFER, sizeof(unitCube2), unitCube2, GL_STATIC_DRAW); //3
    glEnableVertexAttribArray(0); //5
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //4
    glEnableVertexAttribArray(1); //7
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //6
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));



    //unsigned int afafVAO;
    //unsigned int afafVBO;




    //glGenVertexArrays(1, &afafVAO);
    //glBindVertexArray(afafVAO);


    //glGenBuffers(1, &afafVBO);
    //glBindBuffer(GL_ARRAY_BUFFER, afafVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);


    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);


    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    //glEnableVertexAttribArray(1);


    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    //glEnableVertexAttribArray(2);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);


    // Sphere
    defineSphereVertexArray();
    defineSphereIndexArray();
    SphereData sphere = bindSphereVAO();

    glEnable(GL_DEPTH_TEST);

    // texture
    //unsigned int clay;
    glGenTextures(1, &clay);

    glBindTexture(GL_TEXTURE_2D, clay);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("clay2.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //unsigned int triColor;

    glGenTextures(1, &triColor);
    glBindTexture(GL_TEXTURE_2D, triColor);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // flip all textures vertically so that they appear upright
    stbi_set_flip_vertically_on_load(true);

    // load and generate the textures
    data = stbi_load("color.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //unsigned int white;
    glGenTextures(1, &white);
    glBindTexture(GL_TEXTURE_2D, white);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    data = stbi_load("white2.png", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //unsigned int glossy;
    glGenTextures(1, &glossy);
    glBindTexture(GL_TEXTURE_2D, glossy);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    data = stbi_load("glossy.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    //unsigned int tennisBall;
    glGenTextures(1, &tennisBall);
    glBindTexture(GL_TEXTURE_2D, tennisBall);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    data = stbi_load("tennisBall.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    // shadow mapping
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    // define world up - used for camera and lighting
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // view matrix
    glm::mat4 viewMat = glm::mat4(1.0f);

    // projection matrix
    glm::mat4 projectionMat = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // model matrix => use for non-scaling things (camera, coord markers)
    // used for global rotations
    glm::mat4 baseModelMat = glm::mat4(1.0f);

    // mode matrix for scaling things -> will be redifined every render loop to adjust scale of things
    glm::mat4 modelMatScale = glm::mat4(1.0f);

    // I initilize all the matrices that I will use in the render loop. 
    // This is to avoid needing to initialize the variables every time the render loop runs
    glm::mat4 modelx = glm::mat4(1.0f);
    glm::mat4 modelz = glm::mat4(1.0f);
    glm::mat4 netModelMat = glm::mat4(1.0f);
    glm::mat4 terrainModelMat = glm::mat4(1.0f);
    glm::mat4 modelMatLowerArm = glm::mat4(1.0f);
    glm::mat4 modelMatUpperArm = glm::mat4(1.0f);
    glm::mat4 baseRacketModelMat = glm::mat4(1.0f);
    glm::mat4 racketCompModelMat = glm::mat4(1.0f);
    glm::mat4 vertMesh = glm::mat4(1.0f);
    glm::mat4 trueVertMesh = glm::mat4(1.0f);
    glm::mat4 horiMesh = glm::mat4(1.0f);
    glm::mat4 trueHoriMesh = glm::mat4(1.0f);
    glm::mat4 axisModel = glm::mat4(1.0f);
    glm::mat4 safeBaseModelMat = glm::mat4(1.0f);

    
    sphereModelMat = glm::translate(sphereModelMat, glm::vec3(3.0f, 11.0f, 0.0f));

    // set up the shadow shader with default values
    shadow.use();
    shadow.setInt("diffuseTexture", 0);
    shadow.setInt("shadowMap", 1);


    lightPos = glm::vec3(0.0f, 5.0f, 5.0f);

    // Afaf
    glm::mat4 modelHierarchyMatrix = glm::mat4(1.0f);
    glm::mat4 modelWorldMatrix = glm::mat4(1.0f);

    // Kelly
    glm::mat4 kBaseModelMat;
    glm::mat4 shoulderWorldMat;
    glm::mat4 uarmWorldMatrix;
    glm::mat4 elbowWorldMatrix;
    glm::mat4 larmWorldMatrix;
    glm::mat4 wristWorldMatrix;
    glm::mat4 racketWorldMatrix;
    glm::mat4 batWorldMatrix;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // yaw: look left/right
        // pitch: look up/down
        // setup - initial camera
        // determining camera front, right, and up
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        right = glm::normalize(glm::cross(WorldUp, front));
        up = glm::normalize(glm::cross(front, right));

        viewMat = glm::lookAt(position, position + front, up);

        // input
        processInput(window);

        // clear buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // first pass: generate depth map

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 100.0f;
        float  test = sin(glfwGetTime());
        // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        lightProjection = glm::perspective(glm::radians(135.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
        //lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        firstPass.use();
        firstPass.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, white);

        // render first passes
        // the 'if (true)' statements are there just so that the code can be collapsed

        // world rotation matrix - root of hierarchical model for EVERYONE
        baseModelMat = glm::mat4(1.0f);
        baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationx), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate on true X
        baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationy), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate on current Y -> I'll look into this more later, but I wasn't able to find a way to make the third rotation be on the true axis, so I decided Y should rotate on current
        baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationz), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate on true Z
        safeBaseModelMat = baseModelMat;

        // gridlines, coordinate markers, terrain
        renderScene(firstPass,baseModelMat,VAOs);

        //sphereModelMat = safeBaseModelMat;
        // Sphere

        // sphere movement brain
        /*if (sphereModelMat[3][2] >= 30)
        {
            sphereDir = false;
        }
       
        if (sphereModelMat[3][2] <= -30)
        {
            sphereDir = true;
        }*/

        //finalRacketModelMat[0] = glm::translate(finalRacketModelMat[0], glm::vec3(0.0f, 0.0f, 1.0f));
        //std::cout << "X: " << finalRacketModelMat[0][3][0] << ", Y: " << finalRacketModelMat[0][3][1] << ", Z: " << finalRacketModelMat[0][3][2] << std::endl;
        //finalRacketModelMat[0] = glm::translate(finalRacketModelMat[0], glm::vec3(0.0f, 0.0f, -1.0f));

        intersector = -1;
        for (int i = 0; i < 2; i++) 
        {
            std::cout << finalRacketRotations[i][3][0] << std::endl;
            std::cout << finalRacketRotations[i][3][1] << std::endl;
            std::cout << finalRacketRotations[i][3][2] << std::endl;
            // calculate the X, Y, and Z vectors for each racket before doing the collision check
            finalRacketRotations[i] = glm::translate(finalRacketRotations[i], glm::vec3(1.0f, 0.0f, 0.0f));
            Xvector = glm::normalize(glm::vec3(finalRacketRotations[i][3][0], finalRacketRotations[i][3][1], finalRacketRotations[i][3][2]));
            finalRacketRotations[i] = glm::translate(finalRacketRotations[i], glm::vec3(-1.0f, 0.0f, 0.0f));

            finalRacketRotations[i] = glm::translate(finalRacketRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));
            Yvector = glm::normalize(glm::vec3(finalRacketRotations[i][3][0], finalRacketRotations[i][3][1], finalRacketRotations[i][3][2]));
            finalRacketRotations[i] = glm::translate(finalRacketRotations[i], glm::vec3(0.0f, -1.0f, 0.0f));
            
            finalRacketRotations[i] = glm::translate(finalRacketRotations[i], glm::vec3(0.0f, 0.0f, 1.0f));
            Zvector = glm::normalize(glm::vec3(finalRacketRotations[i][3][0], finalRacketRotations[i][3][1], finalRacketRotations[i][3][2]));
            finalRacketRotations[i] = glm::translate(finalRacketRotations[i], glm::vec3(0.0f, 0.0f, -1.0f));

            std::cout << "X: " << Xvector.x << ", " << Xvector.y << ", " << Xvector.z << std::endl;
            std::cout << "Y: " << Yvector.x << ", " << Yvector.y << ", " << Yvector.z << std::endl;
            std::cout << "Z: " << Zvector.x << ", " << Zvector.y << ", " << Zvector.z << std::endl;

            Xvector *= 3;
            Yvector *= 5;

            hitbox = Xvector + Yvector + Zvector;

            std::cout << "hitbox: " << hitbox.x << ", " << hitbox.y << ", " << hitbox.z << std::endl;

            //hitbox.x *= 3;
            //hitbox.y *= 5;
            
            //std::cout << racketposy[i] + 3.67157f << std::endl;
            //std::cout << (finalRacketModelMat[i][3][1] + 3.67157f) << std::endl;
            //std::cout << abs(sphereModelMat[3][1] - (finalRacketModelMat[i][3][1] + 3.67157f)) << std::endl;
            //hitbox.z *= 1;

            std::cout << i << std::endl;
            std::cout << "X: " << abs(sphereModelMat[3][0] - (finalRacketModelMat[i][3][0])) << ", Y: " << abs(sphereModelMat[3][1] - (finalRacketModelMat[i][3][1] + 3.67157f)) << ", Z: " << abs(sphereModelMat[3][2] - finalRacketModelMat[i][3][2]) << std::endl;
            //finalRacketModelMat[i] = glm::translate(finalRacketModelMat[i], glm::vec3(0.0f, 0.0f, 1.0f));
            //std::cout << "X: " << finalRacketModelMat[i][3][0] << ", Y: " << finalRacketModelMat[i][3][1] + 3.67157f << ", Z: " << finalRacketModelMat[i][3][2] << std::endl;
            //finalRacketModelMat[i] = glm::translate(finalRacketModelMat[i], glm::vec3(0.0f, 0.0f, -1.0f));

            //std::cout << i << ": X: " << abs(sphereModelMat[3][0] - racketposx[i]) << ", Y: " << abs(sphereModelMat[3][1] - (racketposy[i] - 11.0f)) << ", Z: " << abs(sphereModelMat[3][2] - racketposz[i]) << std::endl;
            if ((abs(sphereModelMat[3][0] - (finalRacketModelMat[i][3][0])) <= abs(hitbox.x)) && (abs(sphereModelMat[3][1] - (finalRacketModelMat[i][3][1])) <= abs(hitbox.y)) && (abs(sphereModelMat[3][2] - finalRacketModelMat[i][3][2]) <= abs(hitbox.z)))
            {
                //bool intersect = true
                intersector = i;
                //std::cout << intersector << std::endl;
                //break; // optional, minor optimization - ball can only intersect with 1 racket at any given time (technically not true)
            }
        }


        if (intersector != -1 && racketBounceTimer <= 0)
        {
            finalRacketModelMat[intersector][3][0] = 0.0f;
            finalRacketModelMat[intersector][3][1] = 0.0f;
            finalRacketModelMat[intersector][3][2] = 0.0f;
            racketBounceTimer = 30;
            std::cout << "TARGET" << std::endl;
            if (racketposz[intersector] < 0)
            {
                finalRacketModelMat[intersector] = glm::translate(finalRacketModelMat[intersector], glm::vec3(0.0f, 0.0f, 1.0f));
                sphereMovement = glm::normalize(glm::vec3(finalRacketModelMat[intersector][3][0], finalRacketModelMat[intersector][3][1], finalRacketModelMat[intersector][3][2]));
                sphereMovement = glm::vec3(sphereMovement.x / 10, sphereMovement.y / 10 + 0.1f, sphereMovement.z / 10);
            }
            else
            {
                finalRacketModelMat[intersector] = glm::translate(finalRacketModelMat[intersector], glm::vec3(0.0f, 0.0f, -1.0f));
                sphereMovement = glm::normalize(glm::vec3(finalRacketModelMat[intersector][3][0], finalRacketModelMat[intersector][3][1], finalRacketModelMat[intersector][3][2]));
                sphereMovement = glm::vec3(sphereMovement.x / 10, sphereMovement.y / 10 + racketBounce, sphereMovement.z / 10);
            }
            std::cout << "Movement vector: X: " << sphereMovement.x << ", Y: " << sphereMovement.y << ", Z: " << sphereMovement.z << std::endl;
        }
        else
            racketBounceTimer--;

        


        /*
        if (sphereDir)
            sphereModelMat = glm::translate(sphereModelMat, glm::vec3(0.0f, 0.0f, 0.1f));
        else
            sphereModelMat = glm::translate(sphereModelMat, glm::vec3(0.0f, 0.0f, -0.1f));
        */
        sphereModelMat = glm::translate(sphereModelMat, sphereMovement);

        // gravity control
        if (sphereModelMat[3][1] <= 0.01 && groundBounceTimer <= 0)
        {
            sphereMovement.y = groundBounce;
            groundBounceTimer = 5;
        }
        else
        {
            sphereMovement.y -= gravity;
            groundBounceTimer--;
        }

        firstPass.setMat4("model", sphereModelMat);
        // Sphere VAO, VBO and EBO
        glBindVertexArray(sphere.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.EBO);
        // Texture
        // TODO: apply texture on tennis ball
        glDrawElements(GL_TRIANGLES, sizeof(sphereIndexArray) / sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

        glBindVertexArray(VAOs[5]);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // second pass: actually render the scene
        shadow.use();

        //shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

        // shader settings: camera postion, projection matrix, view matrix
        shadow.setVec3("viewPos", position); // camPos

        shadow.setMat4("projection", projectionMat);
        shadow.setMat4("view", viewMat);
        shadow.setBool("shadowActive", shadowActive);


        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, white);

        // render second passes
        // lighting, gridlines, coordinate markers, terrain
        baseModelMat = glm::mat4(1.0f);
        baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationx), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate on true X
        baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationy), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate on current Y -> I'll look into this more later, but I wasn't able to find a way to make the third rotation be on the true axis, so I decided Y should rotate on current
        baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationz), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate on true Z



        // set the light source to 30 units 'up', whatever that up may be relative to the world orientation
        baseModelMat = glm::translate(baseModelMat, glm::vec3(0.1f, 25.0f, 5.0f));
        lightPos.x = baseModelMat[3][0];
        lightPos.y = baseModelMat[3][1];
        lightPos.z = baseModelMat[3][2];
        // light settings for the shader
        shadow.setVec3("lightPos", lightPos);
        shadow.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // light source
        glBindVertexArray(VAOs[5]);

        //visual representation of the light source location
        shadow.setMat4("model", baseModelMat);
        shadow.setVec3("trueColor", glm::vec3(1.0f, 0.0f, 0.0f));
        shadow.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        baseModelMat = glm::translate(baseModelMat, glm::vec3(-0.1f, -25.0f, -5.0f));



        //shader.use();

        shadow.setMat4("model", baseModelMat);

        renderScene(shadow, baseModelMat, VAOs);
       
        /*
        if (sphereDir)
            sphereModelMat = glm::translate(sphereModelMat, glm::vec3(0.0f, 0.0f, 0.1f));
        else
            sphereModelMat = glm::translate(sphereModelMat, glm::vec3(0.0f, 0.0f, -0.1f));
        */
        sphereModelMat = glm::translate(sphereModelMat, sphereMovement);

        shadow.setMat4("model", sphereModelMat);
        shadow.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
        if (textureToggle)
        {
            glBindTexture(GL_TEXTURE_2D, tennisBall);
        }

        // Sphere
        // Sphere VAO, VBO and EBO
        glBindVertexArray(sphere.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.EBO);
        // Texture
        // TODO: apply texture on tennis ball
        glDrawElements(GL_TRIANGLES, sizeof(sphereIndexArray) / sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

        glBindVertexArray(VAOs[5]);

        baseModelMat = safeBaseModelMat;
        glBindTexture(GL_TEXTURE_2D, white);

        // check and call events (poll IO) and swap the buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // |-> memory optimization/leak prevetion
    //glDeleteVertexArrays(6, VAOs);
    //glDeleteBuffers(6, VBOs);

    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // update scrren width and height variables when the window is resized
    glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
}

void processInput(GLFWwindow* window)
{
    // if ESC is pushed, close the OGL window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    // camera controls
    // camera position
    // UP move (default: positive y)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += up * cameraMoveSpeed;
    }
    // DOWN move (default: negative y)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        position -= up * cameraMoveSpeed;
    }
    // RIGHT move (default: positive x)
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        position -= right * cameraMoveSpeed;
    }
    // LEFT move (default: negative x)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        position += right * cameraMoveSpeed;
    }
    // FORWARD move (default: positive z)
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        position += front * cameraMoveSpeed;
    }
    // BACKWARD move (deafult: negative z)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        position -= front * cameraMoveSpeed;
    }

    //camera angle
    // pitch up (positive pitch)
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        pitch += cameraAngleSpeed;
        if (pitch >= 89.0f)
            pitch = 89.0f;
    }
    // pitch down (negative pitch)
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        pitch -= cameraAngleSpeed;
        if (pitch <= -89.0f)
            pitch = -89.0f;
    }
    // yaw right (positive yaw)
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        yaw += cameraAngleSpeed;
        if (yaw >= 360)
            yaw = 0.0f;
    }
    // yaw left (negative yaw)
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        yaw -= cameraAngleSpeed;
        if (yaw <= 0)
            yaw = 360.0f;
    }

    // upscale
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        scaleFactor[controller] += scaleFactorModifier;
        if (scaleFactor[controller] >= 10)
            scaleFactor[controller] = 10;
    }
    // downscale
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        scaleFactor[controller] -= scaleFactorModifier;
        if (scaleFactor[controller] <= 0.05)
            scaleFactor[controller] = 0.05;
    }

    // world rotations
    // positive x axis rot
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        rotationx += rotationFactor;
        if (rotationx >= 360)
            rotationx = 0.0f;
    }
    // negative x axis rot
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        rotationx -= rotationFactor;
        if (rotationx <= 0)
            rotationx = 360.0f;
    }
    // positive y axis rot
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        rotationy += rotationFactor;
        if (rotationy >= 360)
            rotationy = 0.0f;
    }
    // negative y axis rot
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        rotationy -= rotationFactor;
        if (rotationy <= 0)
            rotationy = 360.0f;
    }
    // positive z axis rot
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        rotationz += rotationFactor;
        if (rotationz >= 360)
            rotationz = 0.0f;
    }
    // negative z axis rot
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        rotationz -= rotationFactor;
        if (rotationz <= 0)
            rotationz = 360.0f;
    }

    // reset world
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        // world rotation
        rotationx = 0.0f;
        rotationy = 0.0f;
        rotationz = 0.0f;

        for (int i = 0; i < 5; i++)
        {
            // model scale
            scaleFactor[i] = 1.0f;

            // model position
            //racketposx[i] = -20 + i * 10;
            racketposy[i] = 0.0f;
            //racketposz[i] = -20 + i * 10;


            // model rotation
            // lower arm rotations
            larmrotx[i] = 0.0f;
            larmroty[i] = 0.0f;
            larmrotz[i] = 0.0f;

            // upper arm rotations
            uarmrotx[i] = 0.0f;
            uarmroty[i] = 0.0f;
            uarmrotz[i] = 0.0f;

            // racket rotation
            racketrotx[i] = 0.0f;
            racketroty[i] = 0.0f;
            racketrotz[i] = 0.0f;
        }

        larmrotz[0] = 315.0f;
        uarmrotz[0] = 45.0f;

        larmrotz[1] = 315.0f;
        uarmrotz[1] = 45.0f;

        larmrotz[4] = 45.0f;
        uarmrotz[4] = 45.0f;

        // model position
        racketposx[0] = 0.0f;
        racketposx[1] = 0.0f;
        racketposx[2] = -10.0f;
        racketposx[3] = 10.0f;
        racketposx[4] = 10.0f;

        racketposz[0] = 10.0f;
        racketposz[1] = -10.0f;
        racketposz[2] = 10.0f;
        racketposz[3] = -10.0f;
        racketposz[4] = 10.0f;

        // custom initial scales
        scaleFactor[0] = 0.5f;
        scaleFactor[1] = 0.5f;


        // camera position
        position = glm::vec3(-15.0f, 30.0f, 0.0f);

        // camera angle
        yaw = 0.0f;
        pitch = -45.0f;

        // model component selector
        subject = 0;

        // model selector
        controller = 0;

        // sphere/collision system
        sphereModelMat = glm::mat4(1.0f);
        sphereModelMat = glm::translate(sphereModelMat, glm::vec3(3.0f, 11.0f, 0.0f));
        sphereMovement = glm::vec3(0.0f, 0.0f, 0.1f);
        groundBounceTimer = 0;
        racketBounceTimer = 0;

    }

    // racket movement controls
    // up
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        racketposy[controller] += racketMoveSpeed;
        if (racketposy[controller] >= 50)
            racketposy[controller] = 50;
    }
    // down
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        racketposy[controller] -= racketMoveSpeed;
        if (racketposy[controller] <= -30)
            racketposy[controller] = -30;
    }
    // right
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        racketposx[controller] += racketMoveSpeed;
        if (racketposx[controller] >= 40)
            racketposx[controller] = 40;
    }
    // left
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        racketposx[controller] -= racketMoveSpeed;
        if (racketposx[controller] <= -40)
            racketposx[controller] = -40;
    }
    // forward
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        racketposz[controller] += racketMoveSpeed;
        if (racketposz[controller] >= 40)
            racketposz[controller] = 40;
    }
    // backwards
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        racketposz[controller] -= racketMoveSpeed;
        if (racketposz[controller] <= -40)
            racketposz[controller] = -40;
    }

    // racket roation controls
    // positive x axis rot
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        switch (subject)
        {
        case 0:
            larmrotx[controller] += rotationFactor;
            if (larmrotx[controller] >= 360)
                larmrotx[controller] = 0.0f;
            break;
        case 1:
            uarmrotx[controller] += rotationFactor;
            if (uarmrotx[controller] >= 360)
                uarmrotx[controller] = 0.0f;
            break;
        case 2:
            racketrotx[controller] += rotationFactor;
            if (racketrotx[controller] >= 360)
                racketrotx[controller] = 0.0f;
            break;
        }
    }
    // negative x axis rot
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        switch (subject)
        {
        case 0:
            larmrotx[controller] -= rotationFactor;
            if (larmrotx[controller] <= 0)
                larmrotx[controller] = 360.0f;
            break;
        case 1:
            uarmrotx[controller] -= rotationFactor;
            if (uarmrotx[controller] <= 0)
                uarmrotx[controller] = 360.0f;
            break;
        case 2:
            racketrotx[controller] -= rotationFactor;
            if (racketrotx[controller] <= 0)
                racketrotx[controller] = 360.0f;
            break;
        }
    }
    // positive y axis rot
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        switch (subject)
        {
        case 0:
            larmroty[controller] += rotationFactor;
            if (larmroty[controller] >= 360)
                larmroty[controller] = 0.0f;
            break;
        case 1:
            uarmroty[controller] += rotationFactor;
            if (uarmroty[controller] >= 360)
                uarmroty[controller] = 0.0f;
            break;
        case 2:
            racketroty[controller] += rotationFactor;
            if (racketroty[controller] >= 360)
                racketroty[controller] = 0.0f;
            break;
        }
    }
    // negative y axis rot
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        switch (subject)
        {
        case 0:
            larmroty[controller] -= rotationFactor;
            if (larmroty[controller] <= 0)
                larmroty[controller] = 360.0f;
            break;
        case 1:
            uarmroty[controller] -= rotationFactor;
            if (uarmroty[controller] <= 0)
                uarmroty[controller] = 360.0f;
            break;
        case 2:
            racketroty[controller] -= rotationFactor;
            if (racketroty[controller] <= 0)
                racketroty[controller] = 360.0f;
            break;
        }
    }
    // positive z axis rot
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        switch (subject)
        {
        case 0:
            larmrotz[controller] += rotationFactor;
            if (larmrotz[controller] >= 360)
                larmrotz[controller] = 0.0f;
            break;
        case 1:
            uarmrotz[controller] += rotationFactor;
            if (uarmrotz[controller] >= 360)
                uarmrotz[controller] = 0.0f;
            break;
        case 2:
            racketrotz[controller] += rotationFactor;
            if (racketrotz[controller] >= 360)
                racketrotz[controller] = 0.0f;
            break;
        }
    }
    // negative z axis rot
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        switch (subject)
        {
        case 0:
            larmrotz[controller] -= rotationFactor;
            if (larmrotz[controller] <= 0)
                larmrotz[controller] = 360.0f;
            break;
        case 1:
            uarmrotz[controller] -= rotationFactor;
            if (uarmrotz[controller] <= 0)
                uarmrotz[controller] = 360.0f;
            break;
        case 2:
            racketrotz[controller] -= rotationFactor;
            if (racketrotz[controller] <= 0)
                racketrotz[controller] = 360.0f;
            break;
        }
    }

    // change selected component
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if (canSelect)
        {
            subject++;
            if (subject == 3)
                subject = 0;

            switch (subject)
            {
            case 0:
                std::cout << "lower arm selected" << std::endl;
                break;
            case 1:
                std::cout << "upper arm selected" << std::endl;
                break;
            case 2:
                std::cout << "racket selected" << std::endl;
                break;
            }
            canSelect = false;
        }
    }
    else
    {
        canSelect = true;
    }

    // random teleport
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (canTeleport)
        {
            racketposx[controller] = rand() % 101 - 50;
            racketposy[controller] = rand() % 61 - 30;
            racketposz[controller] = rand() % 101 - 50;
            canTeleport = false;
        }
    }
    else
    {
        canTeleport = true;
    }

    // standard display mode - full faces
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // change to wireframe mode
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // points mode
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    // toggle shadow
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        if (canToggleShadow)
        {
            shadowActive = !shadowActive;
            canToggleShadow = false;
        }
    }
    else
    {
        canToggleShadow = true;
    }

    // cycle through controller
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        if (canChangeController)
        {
            controller++;
            if (controller == 2)
            {
                controller = 0;
            }
            canChangeController = false;

            switch (controller)
            {
            case 0:
                std::cout << "Player 1 selected!" << std::endl;
                break;
            case 1:
                std::cout << "Player 2 selected!" << std::endl;
                break;
            //case 2:
                //std::cout << "Afaf's model selected!" << std::endl;
                //break;
            //case 3:
                //std::cout << "Kelly's model selected!" << std::endl;
                //break;
            }

        }
    }
    else
    {
        canChangeController = true;
    }

    // toggle textures
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        if (canTextureToggle)
        {
            textureToggle = !textureToggle;
            canTextureToggle = false;
        }
    }
    else
    {
        canTextureToggle = true;
    }

}

/*
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = (xpos - lastX)/10;
    float yoffset = (lastY - ypos)/10; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // mose scroll wheel code here
}
*/


unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void renderScene(Shader shader, glm::mat4 baseModelMat, unsigned int VAOs[6]) {



    //glm::mat4 safeBaseModelMat = glm::mat4(1.0f);

    renderGrid(shader, baseModelMat, VAOs);
    for (size_t i = 0; i < 2; i++)
    {
        renderRacket(shader, baseModelMat, VAOs, i);
    }
    

}

void renderGrid(Shader shader, glm::mat4 baseModelMat, unsigned int VAOs[6]) {

    shader.setMat4("model", baseModelMat);

    glm::mat4 modelx = glm::mat4(1.0f);
    glm::mat4 modelz = glm::mat4(1.0f);
    glm::mat4 axisModel = glm::mat4(1.0f);
    glm::mat4 netModelMat = glm::mat4(1.0f);
    glm::mat4 terrainModelMat = glm::mat4(1.0f);

    shader.setMat4("model", baseModelMat);

    // x coord marker
    shader.setVec3("trueColor", glm::vec3(1.0f, 0.0f, 0.0f)); // set color for x coord marker
    glBindVertexArray(VAOs[5]);
    axisModel = glm::translate(baseModelMat, glm::vec3(5.0f, 0.0f, 0.0f));
    axisModel = glm::scale(axisModel, glm::vec3(5.0f, 0.5f, 0.5f));
    shader.setMat4("model", axisModel);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // z coord marker
    shader.setVec3("trueColor", glm::vec3(0.0f, 0.0f, 1.0f)); // set color for z coord marker
    glBindVertexArray(VAOs[5]);
    axisModel = glm::translate(baseModelMat, glm::vec3(0.0f, 0.0f, 5.0f));
    axisModel = glm::scale(axisModel, glm::vec3(0.5f, 0.5f, 5.0f));
    shader.setMat4("model", axisModel);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // y coord marker
    shader.setVec3("trueColor", glm::vec3(0.0f, 1.0f, 0.0f)); // set color for y coord marker
    glBindVertexArray(VAOs[5]);
    axisModel = glm::scale(baseModelMat, glm::vec3(0.5f, 5.0f, 0.5f));
    shader.setMat4("model", axisModel);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    shader.setVec3("trueColor", glm::vec3(0.0f, 0.8f, 0.0f)); // set color for gridlines

    // x gridlines
    glBindVertexArray(VAOs[0]);
    for (int i = 0; i < 101; i++)
    {
        // there is just a single line. It is drawn multiple times in different places by manipulating a custom model matrix for the gridlines
        modelx = glm::translate(baseModelMat, glm::vec3(0, 0, -50.0f + i));
        shader.setMat4("model", modelx);

        glDrawArrays(GL_LINES, 0, 2);
    }

    // z gridlines
    glBindVertexArray(VAOs[1]);
    for (int i = 0; i < 101; i++)
    {
        // same as x gridlines
        modelz = glm::translate(baseModelMat, glm::vec3(-50.0f + i, 0, 0));
        shader.setMat4("model", modelz);

        glDrawArrays(GL_LINES, 0, 2);
    }

    // NET
    // net (using the X gridlines for the horizontal portion of the net)
    glBindVertexArray(VAOs[0]);
    shader.setVec3("trueColor", glm::vec3(0.0f, 0.15f, 0.0f)); // set color for net
    for (int i = 0; i < 16; i++)
    {
        // same as x gridlines
        modelx = glm::translate(baseModelMat, glm::vec3(0.0f, 2.0f + i / 2, 0));
        shader.setMat4("model", modelx);

        glDrawArrays(GL_LINES, 0, 2);
    }

    // vertical net bits
    glBindVertexArray(VAOs[3]);
    for (int i = 0; i < 37; i++)
    {
        // same as x gridlines
        netModelMat = glm::translate(baseModelMat, glm::vec3(-18.0f + i, 0, 0));
        shader.setMat4("model", netModelMat);

        glDrawArrays(GL_LINES, 0, 2);
    }

    // texture for net posts
    if (textureToggle)
    {
        glBindTexture(GL_TEXTURE_2D, triColor);
    }
    // top part of the net
    glBindVertexArray(VAOs[4]);
    shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f)); // set color for net railing
    modelx = glm::scale(modelx, glm::vec3(18.0f, 0.5f, 0.2f));
    shader.setMat4("model", modelx);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // right post of net
    shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f)); // set color for net posts
    netModelMat = glm::scale(netModelMat, glm::vec3(0.4f, 6.0f, 0.4f));
    shader.setMat4("model", netModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // middle post of net
    netModelMat = glm::translate(netModelMat, glm::vec3(-45.0f, 0.0f, 0.0f));
    shader.setMat4("model", netModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // left post of net
    netModelMat = glm::translate(netModelMat, glm::vec3(-45.0f, 0.0f, 0.0f));
    shader.setMat4("model", netModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    if (textureToggle)
    {
        glBindTexture(GL_TEXTURE_2D, clay);
    }

    glBindVertexArray(VAOs[5]);
    // TERRAIN
    // base of terrain
    shader.setVec3("trueColor", glm::vec3(0.0f, 0.52f, 0.4f)); // set color for terrain base

    terrainModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, -1.0f, 0.0f));
    terrainModelMat = glm::scale(terrainModelMat, glm::vec3(18.0f, 0.2f, 39.0f));
    shader.setMat4("model", terrainModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // terrain lines
    shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f)); // set color for terrain lines
    // vertical lines
    // far left line
    terrainModelMat = glm::translate(baseModelMat, glm::vec3(-18.0f, -0.5f, 0.0f));
    terrainModelMat = glm::scale(terrainModelMat, glm::vec3(0.2f, 0.5f, 39.0f));
    shader.setMat4("model", terrainModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // not-as-far-left line
    terrainModelMat = glm::translate(baseModelMat, glm::vec3(-12.0f, -0.5f, 0.0f));
    terrainModelMat = glm::scale(terrainModelMat, glm::vec3(0.2f, 0.5f, 39.0f));
    shader.setMat4("model", terrainModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // middle line
    terrainModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, -0.5f, 0.0f));
    terrainModelMat = glm::scale(terrainModelMat, glm::vec3(0.2f, 0.5f, 21.0f));
    shader.setMat4("model", terrainModelMat);;
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // not-as-far-right line
    terrainModelMat = glm::translate(baseModelMat, glm::vec3(12.0f, -0.5f, 0.0f));
    terrainModelMat = glm::scale(terrainModelMat, glm::vec3(0.2f, 0.5f, 39.0f));
    shader.setMat4("model", terrainModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // far right line
    terrainModelMat = glm::translate(baseModelMat, glm::vec3(18.0f, -0.5f, 0.0f));
    terrainModelMat = glm::scale(terrainModelMat, glm::vec3(0.2f, 0.5f, 39.0f));
    shader.setMat4("model", terrainModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // horizontal lines
    // top line
    terrainModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, -0.5f, 39.0f));
    terrainModelMat = glm::scale(terrainModelMat, glm::vec3(18.0f, 0.5f, 0.2f));
    shader.setMat4("model", terrainModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // upper middle line
    terrainModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, -0.5f, 21.0f));
    terrainModelMat = glm::scale(terrainModelMat, glm::vec3(12.0f, 0.5f, 0.2f));
    shader.setMat4("model", terrainModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // lower midde line
    terrainModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, -0.5f, -21.0f));
    terrainModelMat = glm::scale(terrainModelMat, glm::vec3(12.0f, 0.5f, 0.2f));
    shader.setMat4("model", terrainModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // bottom line
    terrainModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, -0.5f, -39.0f));
    terrainModelMat = glm::scale(terrainModelMat, glm::vec3(18.0f, 0.5f, 0.2f));
    shader.setMat4("model", terrainModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindTexture(GL_TEXTURE_2D, white);

}
void renderRacket(Shader shader, glm::mat4 baseModelMat, unsigned int VAOs[6], int miniController) {

    glm::mat4 modelMatLowerArm = glm::mat4(1.0f);
    glm::mat4 modelMatUpperArm = glm::mat4(1.0f);
    glm::mat4 baseRacketModelMat = glm::mat4(1.0f);
    glm::mat4 racketCompModelMat = glm::mat4(1.0f);
    glm::mat4 vertMesh = glm::mat4(1.0f);
    glm::mat4 trueVertMesh = glm::mat4(1.0f);
    glm::mat4 horiMesh = glm::mat4(1.0f);
    glm::mat4 trueHoriMesh = glm::mat4(1.0f);

    //shader.use();

                   // Hierarchical structure: each piece of the model (lower arm, upper arm, and racket) have a model matrix that build off of the last one. 
                   // This way, all transformations (translation/rotation) are applied to the next component, but that piece can also apply it's own transformations that do not affect lower components
                   // create a scaling matrix based off of the rotation matrix so that it retains any world rotations

                   // scaling matrix - top of hierarchy
    baseModelMat = glm::translate(baseModelMat, glm::vec3(racketposx[miniController], racketposy[miniController], racketposz[miniController])); // translates entire model - rooted at origin
    baseModelMat = glm::scale(baseModelMat, glm::vec3(scaleFactor[miniController], scaleFactor[miniController], scaleFactor[miniController])); // scales entire model
    // rotations for the lower arm portion of the model - upper arm and racket reflect the same rotations
    baseModelMat = glm::rotate(baseModelMat, glm::radians(larmrotx[miniController]), glm::vec3(1.0f, 0.0f, 0.0f));
    baseModelMat = glm::rotate(baseModelMat, glm::radians(larmroty[miniController]), glm::vec3(0.0f, 1.0f, 0.0f));
    baseModelMat = glm::rotate(baseModelMat, glm::radians(larmrotz[miniController]), glm::vec3(0.0f, 0.0f, 1.0f)); // starts with an offset to give the arm the initial angle (larmrotz = -45.0f)

    //modelMatLowerArm = baseModelMat;
    modelMatLowerArm = glm::scale(baseModelMat, glm::vec3(1.0f, 4.0f, 1.0f));
    shader.setMat4("model", modelMatLowerArm);
    shader.setVec3("trueColor", glm::vec3(0.95f, 0.8f, 0.72f)); // set color for lower arm (and upper arm -> color will not be set to this same value for the upper arm)

    glBindVertexArray(VAOs[5]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // translate the upper arm so that it sits just above the lower arm
    baseModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, 8.0f, 0.0f)); // fixed offset of upper arm in reference to the lower arm
    // rotations for the upper arm protion of the model - the racket reflects the same rotations
    baseModelMat = glm::rotate(baseModelMat, glm::radians(uarmrotx[miniController]), glm::vec3(1.0f, 0.0f, 0.0f));
    baseModelMat = glm::rotate(baseModelMat, glm::radians(uarmroty[miniController]), glm::vec3(0.0f, 1.0f, 0.0f));
    baseModelMat = glm::rotate(baseModelMat, glm::radians(uarmrotz[miniController]), glm::vec3(0.0f, 0.0f, 1.0f)); // starts with a counter rotation to offset the rotation introduced by the lower arm (uarmrotz = 45.0f)

    //modelMatUpperArm = baseModelMat;
    modelMatUpperArm = glm::scale(baseModelMat, glm::vec3(1.0f, 4.0f, 1.0f));
    shader.setMat4("model", modelMatUpperArm);

    glDrawArrays(GL_TRIANGLES, 0, 36);


    // apply hierarchical rotations for the racket
    baseModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, 9.0f, 0.0f));
    baseModelMat = glm::rotate(baseModelMat, glm::radians(racketrotx[miniController]), glm::vec3(1.0f, 0.0f, 0.0f));
    baseModelMat = glm::rotate(baseModelMat, glm::radians(racketroty[miniController]), glm::vec3(0.0f, 1.0f, 0.0f));
    baseModelMat = glm::rotate(baseModelMat, glm::radians(racketrotz[miniController]), glm::vec3(0.0f, 0.0f, 1.0f));



    // by doing one side of the racket and then the other, I can simply anchor the base of the next piece to the end of the last, simplifying some translation math
    // 
    // note how for every component, I translate, then scale, then rotate. 
    // This allows me to easily anchor the base of the next component to the end of the last component. 
    // It is a 'simple' translation because I only need to translate in the Y direction and the component will translate in the rotated Y, allowing it to align perfectly

    if (textureToggle)
    {
        glBindTexture(GL_TEXTURE_2D, glossy);
    }
    // RACKET LEFT
    // racket component 1 - bottom left
    baseRacketModelMat = glm::rotate(baseModelMat, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.5f, 1.0f));
    //racketCompModelMat = glm::translate(racketCompModelMat, glm::vec3(0.0f, 0.0f, 0.0f));
    shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setMat4("model", racketCompModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // racket component 2 - left wall
    baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.5f, 0.0f));
    baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.0f, 1.0f));
    shader.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
    shader.setMat4("model", racketCompModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // racket component 3 - left first top
    baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.0f, 0.0f));
    baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.408f, 1.0f));
    shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setMat4("model", racketCompModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // racket component 4 - left second top
    baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 2.816f, 0.0f));
    baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(-40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.732f, 1.0f));
    shader.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
    shader.setMat4("model", racketCompModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);



    // RACKET RIGHT
    // racket component 5 - bottom right
    baseRacketModelMat = glm::rotate(baseModelMat, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.5f, 1.0f));
    //racketCompModelMat = glm::translate(racketCompModelMat, glm::vec3(0.0f, 0.0f, 0.0f));
    shader.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
    shader.setMat4("model", racketCompModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // racket component 6 - right wall
    baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.5f, 0.0f));
    baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.0f, 1.0f));
    shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setMat4("model", racketCompModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // racket component 7 - right first top
    baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.0f, 0.0f));
    baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.408f, 1.0f));
    shader.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
    shader.setMat4("model", racketCompModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // racket component 8 - right second top
    baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 2.816f, 0.0f));
    baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.732f, 1.0f));
    shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setMat4("model", racketCompModelMat);
    glDrawArrays(GL_TRIANGLES, 0, 36);



    // racket mesh
    shader.setVec3("trueColor", glm::vec3(0.0f, 1.0f, 0.5f));

    //vertical lines
    vertMesh = glm::scale(baseModelMat, glm::vec3(0.1f, 5.0f, 0.1f));
    vertMesh = glm::translate(vertMesh, glm::vec3(0.0f, 0.5f, 0.0f));
    for (int i = 0; i < 9; i++)
    {
        trueVertMesh = glm::translate(vertMesh, glm::vec3((i - 4.0f) * 10, 0.0f, 0.0f));
        shader.setMat4("model", trueVertMesh);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // horizontal lines
    horiMesh = glm::translate(baseModelMat, glm::vec3(0.0f, 7.5f, 0.0f));
    horiMesh = glm::scale(horiMesh, glm::vec3(4.5f, 0.1f, 0.1f));

    for (int i = 0; i < 11; i++)
    {
        trueHoriMesh = glm::translate(horiMesh, glm::vec3(0.0f, (i - 5.0f) * 10, 0.0f));
        shader.setMat4("model", trueHoriMesh);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

int main()
{
    //ensure that only 1 program is running at any given point
    std::cout << "START OF PROGRAM" << std::endl;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    int ret = -1;
    ret = Assignment2(window);
    std::cout << "Closed sucessfully!" << std::endl;
    return ret;
}
