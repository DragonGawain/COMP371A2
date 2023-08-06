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

// Settings (for easy adjustability)
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
//const unsigned int SHADOW_WIDTH = 1024;
//const unsigned int SHADOW_HEIGHT = 1024;


// camera
// angle
float pitch = -45.0f;
float yaw = 270.0f;
// position
glm::vec3 position = glm::vec3(0.0f, 30.0f, 15.0f);
// relative directions (relative to the camera itself and where it is looking)
glm::vec3 front;
glm::vec3 right;
glm::vec3 up;
// constants
const float cameraMoveSpeed = 0.5f;
const float cameraAngleSpeed = 0.5f;

// name order within the position/rotation arrays
// Craig, ???, ???, ???, ???

// world rotations
float rotationx = 0.0f;
float rotationy = 0.0f;
float rotationz = 0.0f;
// world rotation speed
const float rotationFactor = 2.5f;

// complete model scaling
float scaleFactor[5] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
const float scaleFactorModifier = 0.001f;

// complete model movement
float racketposx[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float racketposy[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float racketposz[5] = { 0.0f, 10.0f, 20.0f, 30.0f, 40.0f };
const float racketMoveSpeed = 0.5f;

// lower arm rotations
float larmrotx[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float larmroty[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float larmrotz[5] = { 315.0f, 0.0f, 0.0f, 0.0f, 0.0f };

// upper arm rotations
float uarmrotx[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float uarmroty[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float uarmrotz[5] = { 45.0f, 0.0f, 0.0f, 0.0f, 0.0f };

// racket rotations
float racketrotx[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float racketroty[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float racketrotz[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

// switch var to determine which part of the model should be rotated
int subject = 0;
int controller = 0;

bool canSelect = true;
bool canTeleport = true;

// lighting
glm::vec3 lightPos(0.0f, 30.0f, 0.0f);

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// Afaf's global variables 
// starting points of model
float modelPositionX = 0.267f;
float modelPositionY = 0.01f;
float modelPositionZ = -0.5f;






int Assignment2(GLFWwindow* window)
{

    // Afaf's unit cube
    int afafCube()
    {
        //1.0f, 0.84f, 0.7f,
        float vertexArray[] = {

            // back face
            // position			//color				//normals			//texture coords
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


        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);


        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);


        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);


        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return VAO;
    }




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

    // 5 - unit cube (base building block for model)
    float unitCube[] =
    {
        // face 1 - bottom (XZ primary)
        -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        1.0f,  0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

        // face 2 - left (ZY primary)
        -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, 2.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 2.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f
        - 1.0f, 2.0f, 1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        // face 3 - back (XY primary)
        -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        1.0f,  0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 2.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        1.0f,  0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 2.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        1.0f,  2.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,

        // face 4 - top (XZ secondary)
        -1.0f, 2.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f,  2.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 2.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  2.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 2.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  2.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        // face 5 - right (ZY secondary)
        1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 2.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 2.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 2.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        // face 6 - front (XY secondary)
        -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, 2.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, 2.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        1.0f,  2.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
    };

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



    glEnable(GL_DEPTH_TEST);

    // texture
    unsigned int texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

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
    glm::mat4 modelMatLowerArm = glm::mat4(1.0f);
    glm::mat4 modelMatUpperArm = glm::mat4(1.0f);
    glm::mat4 baseRacketModelMat = glm::mat4(1.0f);
    glm::mat4 racketCompModelMat = glm::mat4(1.0f);
    glm::mat4 vertMesh = glm::mat4(1.0f);
    glm::mat4 trueVertMesh = glm::mat4(1.0f);
    glm::mat4 horiMesh = glm::mat4(1.0f);
    glm::mat4 trueHoriMesh = glm::mat4(1.0f);
    glm::mat4 axisModel = glm::mat4(1.0f);

    // set up the shadow shader with default values
    shadow.use();
    shadow.setInt("diffuseTexture", 0);
    shadow.setInt("shadowMap", 1);


    lightPos = glm::vec3(0.0f, 5.0f, 5.0f);

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
        glBindTexture(GL_TEXTURE_2D, texture);

        // the 'if (true)' statements are there just so that the code can be collapsed

        if (true)
        {
            baseModelMat = glm::mat4(1.0f);
            baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationx), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate on true X
            baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationy), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate on current Y -> I'll look into this more later, but I wasn't able to find a way to make the third rotation be on the true axis, so I decided Y should rotate on current
            baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationz), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate on true Z

            firstPass.setMat4("model", baseModelMat);


            // x coord marker
            //shader.setVec3("trueColor", glm::vec3(1.0f, 0.0f, 0.0f)); // set color for x coord marker
            glBindVertexArray(VAOs[5]);
            axisModel = glm::translate(baseModelMat, glm::vec3(5.0f, 0.0f, 0.0f));
            axisModel = glm::scale(axisModel, glm::vec3(5.0f, 1.0f, 1.0f));
            firstPass.setMat4("model", axisModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // z coord marker
            //shader.setVec3("trueColor", glm::vec3(0.0f, 0.0f, 1.0f)); // set color for z coord marker
            glBindVertexArray(VAOs[5]);
            axisModel = glm::translate(baseModelMat, glm::vec3(0.0f, 0.0f, 5.0f));
            axisModel = glm::scale(axisModel, glm::vec3(1.0f, 1.0f, 5.0f));
            firstPass.setMat4("model", axisModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // y coord marker
            //shader.setVec3("trueColor", glm::vec3(0.0f, 1.0f, 0.0f)); // set color for y coord marker
            glBindVertexArray(VAOs[5]);
            axisModel = glm::scale(baseModelMat, glm::vec3(1.0f, 5.0f, 1.0f));
            firstPass.setMat4("model", axisModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            //shader.setVec3("trueColor", glm::vec3(0.0f, 0.8f, 0.0f)); // set color for gridlines

            // x gridlines
            glBindVertexArray(VAOs[0]);
            for (int i = 0; i < 101; i++)
            {
                // there is just a single line. It is drawn multiple times in different places by manipulating a custom model matrix for the gridlines
                modelx = glm::translate(baseModelMat, glm::vec3(0, 0, -50.0f + i));
                firstPass.setMat4("model", modelx);

                glDrawArrays(GL_LINES, 0, 2);
            }

            // z gridlines
            glBindVertexArray(VAOs[1]);
            for (int i = 0; i < 101; i++)
            {
                // same as x gridlines
                modelz = glm::translate(baseModelMat, glm::vec3(-50.0f + i, 0, 0));
                firstPass.setMat4("model", modelz);

                glDrawArrays(GL_LINES, 0, 2);
            }



            // AFAF SECTION 
            int afafCubeVAO = afafCube();
            glBindVertexArray(afafCubeVAO);
            shader.setVec3("trueColor", glm::vec3(1.0f, 0.84f, 0.7f));



            mat4 shoulderPartMatrix1 = translate(mat4(1.0f), vec3(modelPositionX, modelPositionY, modelPositionZ));
            mat4 shoulderPartMatrix2 = rotate(mat4(1.0f), radians(-16.18f), vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 yrotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(larmroty[controller]), vec3(0.0, 1.0, 0.0));
            glm::mat4 zrotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(larmrotz[controller]), vec3(0.0, 0.0, 1.0));
            glm::mat4 xrotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(larmrotx[controller]), vec3(1.0, 0.0, 0.0));
            glm::mat4 rotationMatrix = xrotationMatrix * yrotationMatrix * zrotationMatrix * shoulderPartMatrix2;
            mat4 shoulderScale = scale(mat4(1.0f), vec3(0.03f + scaleFactor[controller], 0.03f + scaleFactor[controller], 0.035f));
            mat4 shoulderMatrix = shoulderPartMatrix1 * rotationMatrix * shoulderScale;
            modelHierarchyMatrix = shoulderMatrix;

            firstPass.setMat4("model", modelHierarchyMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            // Lower arm (the lower arm appearing in window - not in reality) 
            mat4 lowerArmPartMatrix = translate(mat4(1.0f), vec3(0.0f, 1.0f, 0.0f));
            mat4 lowerScale = scale(mat4(1.0f), vec3(1.0f, 3.5f, 1.0f));
            mat4 lowerArmWorld = modelHierarchyMatrix * lowerArmPartMatrix * lowerScale;

            firstPass.setMat4("model", lowerArmWorld);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // elbow - the second join. The racket and upper arm are connected to it as well
           // when I mean upper, I mean the upper arm appearing on the screen / window 
            mat4 elbowPartMatrix = translate(mat4(1.0f), vec3(-0.01f, 2.8f, 0.0f));
            mat4 elbowPartMatrix2 = rotate(mat4(1.0f), radians(16.18f), vec3(0.0f, 0.0f, 1.0f));
            mat4 elbowXRotation = rotate(mat4(1.0f), radians(uarmrotx[controller]), vec3(1.0, 0.0, 0.0));
            mat4 elbowYRotation = rotate(mat4(1.0f), radians(uarmroty[controller]), vec3(0.0, 1.0, 0.0));
            mat4 elbowZRotation = rotate(mat4(1.0f), radians(uarmrotz[controller]), vec3(0.0, 0.0, 1.0));
            mat4 elbowRotation = elbowXRotation * elbowYRotation * elbowZRotation * elbowPartMatrix2;
            mat4 elbowScale = scale(mat4(1.0f), vec3(1.0f, 0.5f, 1.0f));
            mat4 elbow = modelHierarchyMatrix * elbowPartMatrix * elbowRotation * elbowScale;

            firstPass.setMat4("model", elbow);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            // upper arm 
            mat4 upperArmPartMatrix = translate(mat4(1.0f), vec3(0.0f, 4.2f, 0.0f));
            mat4 upperScale = scale(mat4(1.0f), vec3(1.0f, 7.5f, 1.0f));
            mat4 upperArm = elbow * upperArmPartMatrix * upperScale;
            firstPass.setMat4("model", upperArm);
            glDrawArrays(GL_TRIANGLES, 0, 36);



            // Handle 
            mat4 handlePartMatrix = translate(mat4(1.0f), vec3(0.0f, 13.0, 0.0f));
            mat4 handleScale = scale(mat4(1.0f), vec3(0.3f, 9.5f, 0.3f));
            mat4 handleXRotation = rotate(mat4(1.0f), radians(racketrotx[controller]), vec3(1.0, 0.0, 0.0));
            mat4 handleYRotation = rotate(mat4(1.0f), radians(racketroty[controller]), vec3(0.0, 1.0, 0.0));
            mat4 handleZRotation = rotate(mat4(1.0f), radians(racketrotz[controller]), vec3(0.0, 0.0, 1.0));
            mat4 handleRotation = handleXRotation * handleYRotation * handleZRotation;
            mat4 handleMatrix = elbow * handlePartMatrix * handleRotation * handleScale;
            shader.setVec3("trueColor", glm::vec3(1.0f, 0.0f, 0.0f));
            firstPass.setMat4("model", handleMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);



            // Racket net
            mat4 NetPartMatrix = translate(mat4(1.0f), vec3(0.0f, 0.8f, 0.0f))
                * scale(mat4(1.0f), vec3(9.5f, 1.0f, 0.1f));
            mat4 netMatrix = handleMatrix * NetPartMatrix;
            firstPass.setMat4("model", netMatrix);
            shader.setVec3("trueColor", glm::vec3(0.0f, 0.5f, 0.3f));
            glDrawArrays(GL_TRIANGLES, 0, 36);


            // upper tube
            mat4 UpperPartMatrix = translate(mat4(1.0f), vec3(4.6f, 0.8f, 0.0f))
                * scale(mat4(1.0f), vec3(0.5f, 1.0f, 0.3f));
            mat4 FinalMatrix = handleMatrix * UpperPartMatrix;
            firstPass.setMat4("model", FinalMatrix);
            shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
            glDrawArrays(GL_TRIANGLES, 0, 36);


            // upper tube
            mat4 Upper2PartMatrix = translate(mat4(1.0f), vec3(-4.6f, 0.8f, 0.0f))
                * scale(mat4(1.0f), vec3(0.5f, 1.0f, 0.3f));
            mat4 Final2Matrix = handleMatrix * Upper2PartMatrix;
            firstPass.setMat4("model", Final2Matrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            // make sure to unbind my cubeVAO





            // CRAIG SECTION
            // Hierarchical structure: each piece of the model (lower arm, upper arm, and racket) have a model matrix that build off of the last one. 
            // This way, all transformations (translation/rotation) are applied to the next component, but that piece can also apply it's own transformations that do not affect lower components
            // create a scaling matrix based off of the rotation matrix so that it retains any world rotations

            // scaling matrix - top of hierarchy
            baseModelMat = glm::translate(baseModelMat, glm::vec3(racketposx[controller], racketposy[controller], racketposz[controller])); // translates entire model - rooted at origin
            baseModelMat = glm::scale(baseModelMat, glm::vec3(scaleFactor[controller], scaleFactor[controller], scaleFactor[controller])); // scales entire model
            // rotations for the lower arm portion of the model - upper arm and racket reflect the same rotations
            baseModelMat = glm::rotate(baseModelMat, glm::radians(larmrotx[controller]), glm::vec3(1.0f, 0.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(larmroty[controller]), glm::vec3(0.0f, 1.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(larmrotz[controller]), glm::vec3(0.0f, 0.0f, 1.0f)); // starts with an offset to give the arm the initial angle (larmrotz = -45.0f)



            //modelMatLowerArm = baseModelMat;
            modelMatLowerArm = glm::scale(baseModelMat, glm::vec3(1.0f, 4.0f, 1.0f));
            firstPass.setMat4("model", modelMatLowerArm);
            //shader.setVec3("trueColor", glm::vec3(0.95f, 0.8f, 0.72f)); // set color for lower arm (and upper arm -> color will not be set to this same value for the upper arm)

            glBindVertexArray(VAOs[5]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // translate the upper arm so that it sits just above the lower arm
            baseModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, 8.0f, 0.0f)); // fixed offset of upper arm in reference to the lower arm
            // rotations for the upper arm protion of the model - the racket reflects the same rotations
            baseModelMat = glm::rotate(baseModelMat, glm::radians(uarmrotx[controller]), glm::vec3(1.0f, 0.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(uarmroty[controller]), glm::vec3(0.0f, 1.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(uarmrotz[controller]), glm::vec3(0.0f, 0.0f, 1.0f)); // starts with a counter rotation to offset the rotation introduced by the lower arm (uarmrotz = 45.0f)

            //modelMatUpperArm = baseModelMat;
            modelMatUpperArm = glm::scale(baseModelMat, glm::vec3(1.0f, 4.0f, 1.0f));
            firstPass.setMat4("model", modelMatUpperArm);

            glDrawArrays(GL_TRIANGLES, 0, 36);


            // apply hierarchical rotations for the racket
            baseModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, 9.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(racketrotx[controller]), glm::vec3(1.0f, 0.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(racketroty[controller]), glm::vec3(0.0f, 1.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(racketrotz[controller]), glm::vec3(0.0f, 0.0f, 1.0f));

            //shader.setVec3("trueColor", glm::vec3(0.5f, 0.5f, 0.5f)); // SETTING ALL OF RACKET TO BE GREY FOR NOW

            // by doing one side of the racket and then the other, I can simply anchor the base of the next piece to the end of the last, simplifying some translation math
            // 
            // note how for every component, I translate, then scale, then rotate. 
            // This allows me to easily anchor the base of the next component to the end of the last component. 
            // It is a 'simple' translation because I only need to translate in the Y direction and the component will translate in the rotated Y, allowing it to align perfectly

            // RACKET LEFT
            // racket component 1 - bottom left
            baseRacketModelMat = glm::rotate(baseModelMat, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.5f, 1.0f));
            //racketCompModelMat = glm::translate(racketCompModelMat, glm::vec3(0.0f, 0.0f, 0.0f));
            //shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
            firstPass.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 2 - left wall
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.5f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.0f, 1.0f));
            //shader.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
            firstPass.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 3 - left first top
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.0f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.408f, 1.0f));
            //shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
            firstPass.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 4 - left second top
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 2.816f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(-40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.732f, 1.0f));
            //shader.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
            firstPass.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);



            // RACKET RIGHT
            // racket component 5 - bottom right
            baseRacketModelMat = glm::rotate(baseModelMat, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.5f, 1.0f));
            //racketCompModelMat = glm::translate(racketCompModelMat, glm::vec3(0.0f, 0.0f, 0.0f));
            //shader.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
            firstPass.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 6 - right wall
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.5f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.0f, 1.0f));
            //shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
            firstPass.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 7 - right first top
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.0f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.408f, 1.0f));
            //shader.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
            firstPass.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 8 - right second top
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 2.816f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.732f, 1.0f));
            //shader.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
            firstPass.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket mesh
            //shader.setVec3("trueColor", glm::vec3(0.0f, 1.0f, 0.5f));

            //vertical lines
            vertMesh = glm::scale(baseModelMat, glm::vec3(0.1f, 5.0f, 0.1f));
            vertMesh = glm::translate(vertMesh, glm::vec3(0.0f, 0.5f, 0.0f));
            for (int i = 0; i < 9; i++)
            {
                trueVertMesh = glm::translate(vertMesh, glm::vec3((i - 4.0f) * 10, 0.0f, 0.0f));
                firstPass.setMat4("model", trueVertMesh);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // horizontal lines
            horiMesh = glm::translate(baseModelMat, glm::vec3(0.0f, 7.5f, 0.0f));
            horiMesh = glm::scale(horiMesh, glm::vec3(4.5f, 0.1f, 0.1f));

            for (int i = 0; i < 11; i++)
            {
                trueHoriMesh = glm::translate(horiMesh, glm::vec3(0.0f, (i - 5.0f) * 10, 0.0f));
                firstPass.setMat4("model", trueHoriMesh);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

        }


        // redner first passes

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


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);


        if (true)
        {






            baseModelMat = glm::mat4(1.0f);
            baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationx), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate on true X
            baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationy), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate on current Y -> I'll look into this more later, but I wasn't able to find a way to make the third rotation be on the true axis, so I decided Y should rotate on current
            baseModelMat = glm::rotate(baseModelMat, glm::radians(rotationz), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate on true Z



            // set the light source to 30 units 'up', whatever that up may be relative to the world orientation
            baseModelMat = glm::translate(baseModelMat, glm::vec3(0.1f, 25.0f, 10.0f));
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
            shadow.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            baseModelMat = glm::translate(baseModelMat, glm::vec3(-0.1f, -25.0f, -10.0f));



            //shader.use();

            shadow.setMat4("model", baseModelMat);

            // x coord marker
            shadow.setVec3("trueColor", glm::vec3(1.0f, 0.0f, 0.0f)); // set color for x coord marker
            glBindVertexArray(VAOs[5]);
            axisModel = glm::translate(baseModelMat, glm::vec3(5.0f, 0.0f, 0.0f));
            axisModel = glm::scale(axisModel, glm::vec3(5.0f, 1.0f, 1.0f));
            shadow.setMat4("model", axisModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // z coord marker
            shadow.setVec3("trueColor", glm::vec3(0.0f, 0.0f, 1.0f)); // set color for z coord marker
            glBindVertexArray(VAOs[5]);
            axisModel = glm::translate(baseModelMat, glm::vec3(0.0f, 0.0f, 5.0f));
            axisModel = glm::scale(axisModel, glm::vec3(1.0f, 1.0f, 5.0f));
            shadow.setMat4("model", axisModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // y coord marker
            shadow.setVec3("trueColor", glm::vec3(0.0f, 1.0f, 0.0f)); // set color for y coord marker
            glBindVertexArray(VAOs[5]);
            axisModel = glm::scale(baseModelMat, glm::vec3(1.0f, 5.0f, 1.0f));
            shadow.setMat4("model", axisModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            shadow.setVec3("trueColor", glm::vec3(0.0f, 0.8f, 0.0f)); // set color for gridlines

            // x gridlines
            glBindVertexArray(VAOs[0]);
            for (int i = 0; i < 101; i++)
            {
                // there is just a single line. It is drawn multiple times in different places by manipulating a custom model matrix for the gridlines
                modelx = glm::translate(baseModelMat, glm::vec3(0, 0, -50.0f + i));
                shadow.setMat4("model", modelx);

                glDrawArrays(GL_LINES, 0, 2);
            }

            // z gridlines
            glBindVertexArray(VAOs[1]);
            for (int i = 0; i < 101; i++)
            {
                // same as x gridlines
                modelz = glm::translate(baseModelMat, glm::vec3(-50.0f + i, 0, 0));
                shadow.setMat4("model", modelz);

                glDrawArrays(GL_LINES, 0, 2);
            }


            // AFAF SECTION 2 
               // AFAF SECTION 


            // make sure to unbind my VAO afterwards
            int afafCubeVAO = afafCube();
            glBindVertexArray(afafCubeVAO);

            shadow.setVec3("trueColor", glm::vec3(1.0f, 0.84f, 0.7f));



            mat4 shoulderPartMatrix1 = translate(mat4(1.0f), vec3(modelPositionX, modelPositionY, modelPositionZ));
            mat4 shoulderPartMatrix2 = rotate(mat4(1.0f), radians(-16.18f), vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 yrotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(larmroty[controller]), vec3(0.0, 1.0, 0.0));
            glm::mat4 zrotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(larmrotz[controller]), vec3(0.0, 0.0, 1.0));
            glm::mat4 xrotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(larmrotx[controller]), vec3(1.0, 0.0, 0.0));
            glm::mat4 rotationMatrix = xrotationMatrix * yrotationMatrix * zrotationMatrix * shoulderPartMatrix2;
            mat4 shoulderScale = scale(mat4(1.0f), vec3(0.03f + scaleFactor[controller], 0.03f + scaleFactor[controller], 0.035f));
            mat4 shoulderMatrix = shoulderPartMatrix1 * rotationMatrix * shoulderScale;
            modelHierarchyMatrix = shoulderMatrix;

            shadow.setMat4("model", modelHierarchyMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            // Lower arm (the lower arm appearing in window - not in reality) 
            mat4 lowerArmPartMatrix = translate(mat4(1.0f), vec3(0.0f, 1.0f, 0.0f));
            mat4 lowerScale = scale(mat4(1.0f), vec3(1.0f, 3.5f, 1.0f));
            mat4 lowerArmWorld = modelHierarchyMatrix * lowerArmPartMatrix * lowerScale;

            shadow.setMat4("model", lowerArmWorld);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // elbow - the second join. The racket and upper arm are connected to it as well
           // when I mean upper, I mean the upper arm appearing on the screen / window 
            mat4 elbowPartMatrix = translate(mat4(1.0f), vec3(-0.01f, 2.8f, 0.0f));
            mat4 elbowPartMatrix2 = rotate(mat4(1.0f), radians(16.18f), vec3(0.0f, 0.0f, 1.0f));
            mat4 elbowXRotation = rotate(mat4(1.0f), radians(uarmrotx[controller]), vec3(1.0, 0.0, 0.0));
            mat4 elbowYRotation = rotate(mat4(1.0f), radians(uarmroty[controller]), vec3(0.0, 1.0, 0.0));
            mat4 elbowZRotation = rotate(mat4(1.0f), radians(uarmrotz[controller]), vec3(0.0, 0.0, 1.0));
            mat4 elbowRotation = elbowXRotation * elbowYRotation * elbowZRotation * elbowPartMatrix2;
            mat4 elbowScale = scale(mat4(1.0f), vec3(1.0f, 0.5f, 1.0f));
            mat4 elbow = modelHierarchyMatrix * elbowPartMatrix * elbowRotation * elbowScale;

            shadow.setMat4("model", elbow);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            // upper arm 
            mat4 upperArmPartMatrix = translate(mat4(1.0f), vec3(0.0f, 4.2f, 0.0f));
            mat4 upperScale = scale(mat4(1.0f), vec3(1.0f, 7.5f, 1.0f));
            mat4 upperArm = elbow * upperArmPartMatrix * upperScale;
            shadow.setMat4("model", upperArm);
            glDrawArrays(GL_TRIANGLES, 0, 36);



            // Handle 
            mat4 handlePartMatrix = translate(mat4(1.0f), vec3(0.0f, 13.0, 0.0f));
            mat4 handleScale = scale(mat4(1.0f), vec3(0.3f, 9.5f, 0.3f));
            mat4 handleXRotation = rotate(mat4(1.0f), radians(racketrotx[controller]), vec3(1.0, 0.0, 0.0));
            mat4 handleYRotation = rotate(mat4(1.0f), radians(racketroty[controller]), vec3(0.0, 1.0, 0.0));
            mat4 handleZRotation = rotate(mat4(1.0f), radians(racketrotz[controller]), vec3(0.0, 0.0, 1.0));
            mat4 handleRotation = handleXRotation * handleYRotation * handleZRotation;
            mat4 handleMatrix = elbow * handlePartMatrix * handleRotation * handleScale;
            shadow.setVec3("trueColor", glm::vec3(1.0f, 0.0f, 0.0f));
            shadow.setMat4("model", handleMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);



            // Racket net
            mat4 NetPartMatrix = translate(mat4(1.0f), vec3(0.0f, 0.8f, 0.0f))
                * scale(mat4(1.0f), vec3(9.5f, 1.0f, 0.1f));
            mat4 netMatrix = handleMatrix * NetPartMatrix;
            shadow.setMat4("model", netMatrix);
            shadow.setVec3("trueColor", glm::vec3(0.0f, 0.5f, 0.3f));
            glDrawArrays(GL_TRIANGLES, 0, 36);


            // upper tube
            mat4 UpperPartMatrix = translate(mat4(1.0f), vec3(4.6f, 0.8f, 0.0f))
                * scale(mat4(1.0f), vec3(0.5f, 1.0f, 0.3f));
            mat4 FinalMatrix = handleMatrix * UpperPartMatrix;
            shadow.setMat4("model", FinalMatrix);
            shadow.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
            glDrawArrays(GL_TRIANGLES, 0, 36);


            // upper tube
            mat4 Upper2PartMatrix = translate(mat4(1.0f), vec3(-4.6f, 0.8f, 0.0f))
                * scale(mat4(1.0f), vec3(0.5f, 1.0f, 0.3f));
            mat4 Final2Matrix = handleMatrix * Upper2PartMatrix;
            shadow.setMat4("model", Final2Matrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            // make sure to unbind my cubeVAO







            //shader.use();

            // Hierarchical structure: each piece of the model (lower arm, upper arm, and racket) have a model matrix that build off of the last one. 
            // This way, all transformations (translation/rotation) are applied to the next component, but that piece can also apply it's own transformations that do not affect lower components
            // create a scaling matrix based off of the rotation matrix so that it retains any world rotations

            // scaling matrix - top of hierarchy
            baseModelMat = glm::translate(baseModelMat, glm::vec3(racketposx[controller], racketposy[controller], racketposz[controller])); // translates entire model - rooted at origin
            baseModelMat = glm::scale(baseModelMat, glm::vec3(scaleFactor[controller], scaleFactor[controller], scaleFactor[controller])); // scales entire model
            // rotations for the lower arm portion of the model - upper arm and racket reflect the same rotations
            baseModelMat = glm::rotate(baseModelMat, glm::radians(larmrotx[controller]), glm::vec3(1.0f, 0.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(larmroty[controller]), glm::vec3(0.0f, 1.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(larmrotz[controller]), glm::vec3(0.0f, 0.0f, 1.0f)); // starts with an offset to give the arm the initial angle (larmrotz = -45.0f)

            //modelMatLowerArm = baseModelMat;
            modelMatLowerArm = glm::scale(baseModelMat, glm::vec3(1.0f, 4.0f, 1.0f));
            shadow.setMat4("model", modelMatLowerArm);
            shadow.setVec3("trueColor", glm::vec3(0.95f, 0.8f, 0.72f)); // set color for lower arm (and upper arm -> color will not be set to this same value for the upper arm)

            glBindVertexArray(VAOs[5]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // translate the upper arm so that it sits just above the lower arm
            baseModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, 8.0f, 0.0f)); // fixed offset of upper arm in reference to the lower arm
            // rotations for the upper arm protion of the model - the racket reflects the same rotations
            baseModelMat = glm::rotate(baseModelMat, glm::radians(uarmrotx[controller]), glm::vec3(1.0f, 0.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(uarmroty[controller]), glm::vec3(0.0f, 1.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(uarmrotz[controller]), glm::vec3(0.0f, 0.0f, 1.0f)); // starts with a counter rotation to offset the rotation introduced by the lower arm (uarmrotz = 45.0f)

            //modelMatUpperArm = baseModelMat;
            modelMatUpperArm = glm::scale(baseModelMat, glm::vec3(1.0f, 4.0f, 1.0f));
            shadow.setMat4("model", modelMatUpperArm);

            glDrawArrays(GL_TRIANGLES, 0, 36);


            // apply hierarchical rotations for the racket
            baseModelMat = glm::translate(baseModelMat, glm::vec3(0.0f, 9.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(racketrotx[controller]), glm::vec3(1.0f, 0.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(racketroty[controller]), glm::vec3(0.0f, 1.0f, 0.0f));
            baseModelMat = glm::rotate(baseModelMat, glm::radians(racketrotz[controller]), glm::vec3(0.0f, 0.0f, 1.0f));

            shadow.setVec3("trueColor", glm::vec3(0.5f, 0.5f, 0.5f)); // SETTING ALL OF RACKET TO BE GREY FOR NOW

            // by doing one side of the racket and then the other, I can simply anchor the base of the next piece to the end of the last, simplifying some translation math
            // 
            // note how for every component, I translate, then scale, then rotate. 
            // This allows me to easily anchor the base of the next component to the end of the last component. 
            // It is a 'simple' translation because I only need to translate in the Y direction and the component will translate in the rotated Y, allowing it to align perfectly

            // RACKET LEFT
            // racket component 1 - bottom left
            baseRacketModelMat = glm::rotate(baseModelMat, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.5f, 1.0f));
            //racketCompModelMat = glm::translate(racketCompModelMat, glm::vec3(0.0f, 0.0f, 0.0f));
            shadow.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
            shadow.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 2 - left wall
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.5f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.0f, 1.0f));
            shadow.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
            shadow.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 3 - left first top
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.0f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.408f, 1.0f));
            shadow.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
            shadow.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 4 - left second top
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 2.816f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(-40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.732f, 1.0f));
            shadow.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
            shadow.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);



            // RACKET RIGHT
            // racket component 5 - bottom right
            baseRacketModelMat = glm::rotate(baseModelMat, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.5f, 1.0f));
            //racketCompModelMat = glm::translate(racketCompModelMat, glm::vec3(0.0f, 0.0f, 0.0f));
            shadow.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
            shadow.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 6 - right wall
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.5f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 3.0f, 1.0f));
            shadow.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
            shadow.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 7 - right first top
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 6.0f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.408f, 1.0f));
            shadow.setVec3("trueColor", glm::vec3(0.9f, 0.2f, 0.2f));
            shadow.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket component 8 - right second top
            baseRacketModelMat = glm::translate(baseRacketModelMat, glm::vec3(0.0f, 2.816f, 0.0f));
            baseRacketModelMat = glm::rotate(baseRacketModelMat, glm::radians(40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            racketCompModelMat = glm::scale(baseRacketModelMat, glm::vec3(1.0f, 1.732f, 1.0f));
            shadow.setVec3("trueColor", glm::vec3(1.0f, 1.0f, 1.0f));
            shadow.setMat4("model", racketCompModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // racket mesh
            shadow.setVec3("trueColor", glm::vec3(0.0f, 1.0f, 0.5f));

            //vertical lines
            vertMesh = glm::scale(baseModelMat, glm::vec3(0.1f, 5.0f, 0.1f));
            vertMesh = glm::translate(vertMesh, glm::vec3(0.0f, 0.5f, 0.0f));
            for (int i = 0; i < 9; i++)
            {
                trueVertMesh = glm::translate(vertMesh, glm::vec3((i - 4.0f) * 10, 0.0f, 0.0f));
                shadow.setMat4("model", trueVertMesh);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // horizontal lines
            horiMesh = glm::translate(baseModelMat, glm::vec3(0.0f, 7.5f, 0.0f));
            horiMesh = glm::scale(horiMesh, glm::vec3(4.5f, 0.1f, 0.1f));

            for (int i = 0; i < 11; i++)
            {
                trueHoriMesh = glm::translate(horiMesh, glm::vec3(0.0f, (i - 5.0f) * 10, 0.0f));
                shadow.setMat4("model", trueHoriMesh);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

        }

        // render second passes


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
    glViewport(0, 0, width, height);
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
        if (scaleFactor[controller] >= 5)
            scaleFactor[controller] = 5;
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
            racketposx[i] = 0.0f;
            racketposy[i] = 0.0f;
            racketposz[i] = 10 * i;;


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

        // camera position
        position = glm::vec3(0.0f, 30.0f, 15.0f);

        // camera angle
        yaw = 270.0f;
        pitch = -45.0f;

        // model component selector
        subject = 0;

        // model selector
        controller = 0;

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
