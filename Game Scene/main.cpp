#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"


#include <random>

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 animatedModel;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLuint animatedModelLoc;


glm::mat4 lightRotation;
GLfloat lightAngle;


//directionalLight

glm::vec3 lightDirTwo[2];
GLuint lightDirLocTwo[2];
glm::vec3 lightColorTwo[2];
GLuint lightColorLocTwo[2];
GLint activeLightTwo[2] = { 1, 1 };


// camera

//-2.579208 0.216086 0.723309
//0.0f, 0.0f, 3.0f

/*
camera position:: !!!!!!!!!-1.845190 -0.394805 -0.030105
camera target:: !!!!!!!!!-0.845698 -0.375243 -0.055278

*/

gps::Camera myCamera(
    glm::vec3(-1.845190, -0.394805, -0.030105),
    glm::vec3(-0.845698, -0.375243,  -0.055278),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];


const int CNT_RAIN = 5;

// models
gps::Model3D teapot;
gps::Model3D ufo;
gps::Model3D ufo_ray;
gps::Model3D greenhouse;
gps::Model3D lake;
gps::Model3D cow;
gps::Model3D light_cow;
gps::Model3D light_chicken;
gps::Model3D light_alien;
gps::Model3D helicopter_base;
gps::Model3D helicopter_blade;
gps::Model3D rain[CNT_RAIN];
gps::Model3D trees;
gps::Model3D cornfield;
gps::Model3D plants;
gps::Model3D house;
gps::Model3D nanosuit;
gps::Model3D scene;
//gps::Model3D greenhouse;
//gps::Model3D lake;


// raindrops random positions & speed

glm::vec3 rainPosition[CNT_RAIN];
glm::vec3 rainSpeed[CNT_RAIN];
glm::mat4 rainModel[CNT_RAIN];


// rotation angle for camera
GLfloat angle;

// shaders
gps::Shader myBasicShader;
gps::Shader transparentShader;
gps::Shader treesShader;
gps::Shader animationShader;
gps::Shader twoLightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader skyboxShader;

// skybox
gps::SkyBox mySkyBox;

// shadow elements
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;
GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap;
//int retina_width, retina_height;

gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;

// fog elements

GLfloat fogDensity = 0.5f;
GLfloat valAtt = 1.0f;


// delete this!!!!

int pointLight = 1;


glm::vec3 light2Dir;
glm::vec3 light2Color;
GLuint light2ColorLoc;



GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)


bool polygonView = false;
bool wireframeView = false;
bool rainMode = false;
bool presentationMode = false;
bool thunder = false;


void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
    //TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        showDepthMap = !showDepthMap;


    if (key == GLFW_KEY_T && action == GLFW_PRESS) {

        wireframeView = !wireframeView;

        if (wireframeView) {

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS) {

        polygonView = !polygonView;

        if (polygonView) {

            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        }
        else {

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {

        rainMode = !rainMode;


    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {

        presentationMode = !presentationMode;


    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            pressedKeys[key] = true;
        else if (action == GLFW_RELEASE)
            pressedKeys[key] = false;
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        activeLightTwo[0] = 1 - activeLightTwo[0];
        printf("\n\n\n\pressed 1!!!! value = %d", activeLightTwo[0]);
        twoLightShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(twoLightShader.shaderProgram, "activeLight[0]"), activeLightTwo[0]);

    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        activeLightTwo[1] = 1 - activeLightTwo[1];
        printf("pressed 2!!!! value = %d", activeLightTwo[0]);
        twoLightShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(twoLightShader.shaderProgram, "activeLight[1]"), activeLightTwo[1]);

    }
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        pointLight = (pointLight + 1)%2;
    }
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        thunder = !thunder;
        printf("THUNDER ON!!!!!!!!!!!!!!!!!");

    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static double currX = 0.0;
    static double currY = 0.0;
    const float smooth = 0.15f;
    static int start = 1;

    if (start == 1) {
        currY = ypos;
        currX = xpos;

        start = 0;
    }

    double y = currY - ypos;
    double x = xpos - currX;
    y *= smooth;
    x *= smooth;
    currY = ypos;
    currX = xpos;

    myBasicShader.useShaderProgram();

    myCamera.rotate(-y, -x);
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    view = myCamera.getViewMatrix();

    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    transparentShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(transparentShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    treesShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(treesShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    animationShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    twoLightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(twoLightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));


}

void movementForAllShaders(glm::mat4 view) {
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    transparentShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(transparentShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    treesShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(treesShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    animationShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));


    twoLightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(twoLightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));



}

void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        movementForAllShaders(view);
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        myBasicShader.useShaderProgram();
        glUniformMatrix3fv(glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

        twoLightShader.useShaderProgram();
        glUniformMatrix3fv(glGetUniformLocation(twoLightShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3fv(lightDirLocTwo[0], 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDirTwo[0]));
        glUniform3fv(lightDirLocTwo[1], 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDirTwo[1]));

    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        movementForAllShaders(view);

        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        myBasicShader.useShaderProgram();
        glUniformMatrix3fv(glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

        twoLightShader.useShaderProgram();
        glUniformMatrix3fv(glGetUniformLocation(twoLightShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3fv(lightDirLocTwo[0], 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDirTwo[0]));
        glUniform3fv(lightDirLocTwo[1], 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDirTwo[1]));

    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        movementForAllShaders(view);

        // compute normal matrix for teapot

        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        myBasicShader.useShaderProgram();
        glUniformMatrix3fv(glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

        twoLightShader.useShaderProgram();
        glUniformMatrix3fv(glGetUniformLocation(twoLightShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3fv(lightDirLocTwo[0], 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDirTwo[0]));
        glUniform3fv(lightDirLocTwo[1], 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDirTwo[1]));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        movementForAllShaders(view);

        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        myBasicShader.useShaderProgram();
        glUniformMatrix3fv(glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));


        twoLightShader.useShaderProgram();
        glUniformMatrix3fv(glGetUniformLocation(twoLightShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3fv(lightDirLocTwo[0], 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDirTwo[0]));
        glUniform3fv(lightDirLocTwo[1], 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDirTwo[1]));
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

        twoLightShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(twoLightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(glGetUniformLocation(twoLightShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));


        animationShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(glGetUniformLocation(animationShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));


        transparentShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(transparentShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(glGetUniformLocation(transparentShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));


        treesShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(treesShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(glGetUniformLocation(treesShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));



    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        //myBasicShader.useShaderProgram();
        //glUniformMatrix3fv(glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

        //twoLightShader.useShaderProgram();
        ////glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        //glUniformMatrix3fv(glGetUniformLocation(twoLightShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));



        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

        twoLightShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(twoLightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(glGetUniformLocation(twoLightShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));



   /*     animationShader.useShaderProgram();
        glm::mat4 aux = animatedModel * model;
        glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(aux));
        glm::mat4 auxnormal = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(glGetUniformLocation(animationShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(auxnormal));*/


        transparentShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(transparentShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(glGetUniformLocation(transparentShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));


        treesShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(treesShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(glGetUniformLocation(treesShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));


    }

    if (pressedKeys[GLFW_KEY_F]) {

        if (fogDensity < 0.5f)
            fogDensity += 0.01f;
        else
            fogDensity = 0.5f;

        printf("+ FOG DENSITY: %f\n", fogDensity);
    }
    if (pressedKeys[GLFW_KEY_G]) {
        if (fogDensity > 0.01f)
            fogDensity -= 0.01f;
        else
            fogDensity = 0.0f;

        printf("- FOG DENSITY: %f\n", fogDensity);
    }


    if (pressedKeys[GLFW_KEY_J]) {
        lightAngle -= 10.0f;
    }

    if (pressedKeys[GLFW_KEY_L]) {
        lightAngle += 10.0f;
    }




}


void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");

}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    //glViewport(0, 0, retina_width, retina_height);


    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

    //transparenta
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initModels() {
    //teapot.LoadModel("models/teapot/teapot20segUT.obj");
    ufo.LoadModel("models/ufo/ufo.obj");
    trees.LoadModel("models/trees/trees.obj");
    cornfield.LoadModel("models/cornfield/corn.obj");
    plants.LoadModel("models/plants/plants.obj");
    cow.LoadModel("models/cow/cow.obj");
    light_cow.LoadModel("models/cow/light_cow.obj");
    light_chicken.LoadModel("models/chicken/light_chicken.obj");
    light_alien.LoadModel("models/alien/alien.obj");
    helicopter_base.LoadModel("models/helicopter/helicopter_base.obj");
    house.LoadModel("models/house/house.obj");
    helicopter_blade.LoadModel("models/helicopter/helicopter_blade.obj");
    scene.LoadModel("models/scene/project.obj");
    ufo_ray.LoadModel("models/ufo/ufo_ray.obj");
    greenhouse.LoadModel("models/greenhouse/greenhouse.obj");
    lake.LoadModel("models/lake/lake.obj");
    // 
    //nanosuit.LoadModel("models/nanosuit/nanosuit.obj", "models/nanosuit/");

    for (int i = 0; i < CNT_RAIN; i++) {
        rain[i].LoadModel("models/rain/rain.obj");
        rainModel[i] = glm::mat4(1.0f);
    }


}

void initShaders() {

    myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
    transparentShader.loadShader("shaders/transparent.vert", "shaders/transparent.frag");
    treesShader.loadShader("shaders/transparent2.vert", "shaders/transparent2.frag");
    animationShader.loadShader("shaders/animationShader.vert", "shaders/animationShader.frag");
    twoLightShader.loadShader("shaders/twoLightShader.vert", "shaders/twoLightShader.frag");
    depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");

}


void initSkyBox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/right.tga");
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/bottom.tga");
    faces.push_back("skybox/back.tga");
    faces.push_back("skybox/front.tga");

    mySkyBox.Load(faces);

    view = myCamera.getViewMatrix();
    skyboxShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));
    glCheckError();

    projection = glm::perspective(glm::radians(45.0f), (float)600 / (float)600, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));


}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);
    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 6.0f;
    glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;


}

void initUniforms() {
        myBasicShader.useShaderProgram();
    
        model = glm::mat4(1.0f);
        modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
        view = myCamera.getViewMatrix();
        viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight"), pointLight);


        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    
        projection = glm::perspective(glm::radians(45.0f),
            (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
            0.1f, 20.0f);
    
        projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
        // send projection matrix to shader
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        //set the light direction (direction towards the light)
        lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
        //glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
        
        light2Dir = glm::vec3(-0.968178f, -0.328295f, 0.003068f);
        glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "light2Dir"), 1, glm::value_ptr(light2Dir));

        
        
        //set light color
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
        lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    

        light2Color = glm::vec3(1.0f, 0.0f, 0.0f); 
        light2ColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "light2Color");
        glUniform3fv(light2ColorLoc, 1, glm::value_ptr(light2Color));
        //glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceTrMatrix));

        //lightShader.useShaderProgram();
        //glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            // set fog density
        glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity"), fogDensity);
            //set thunder
        glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "valAtt"), valAtt);



    /*
        TRANSPARENT SHADER
    */

    transparentShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(transparentShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(transparentShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(transparentShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    /*
        TRANSPARENT SHADER2
    */

    treesShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(treesShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(treesShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(treesShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    /*
        ANIMATION SHADER
    */
    animationShader.useShaderProgram();
    animatedModel = glm::mat4(1.0f);
    animatedModelLoc = glGetUniformLocation(animationShader.shaderProgram, "model");
    glUniformMatrix4fv(animatedModelLoc, 1, GL_FALSE, glm::value_ptr(animatedModel));
    glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));




    // first light source
    //twoLightShader.useShaderProgram();
    twoLightShader.useShaderProgram();


    glUniformMatrix4fv(glGetUniformLocation(twoLightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(twoLightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(twoLightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


    lightDirTwo[0] = glm::vec3(0.0f, 0.0f, 1.0f);
    lightDirLocTwo[0] = glGetUniformLocation(twoLightShader.shaderProgram, "lightDir[0]");
    glUniform3fv(lightDirLocTwo[0], 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDirTwo[0]));

    lightColorTwo[0] = glm::vec3(0.0f, 1.0f, 1.0f);
    lightColorLocTwo[0] = glGetUniformLocation(twoLightShader.shaderProgram, "lightColor[0]");
    glUniform3fv(lightColorLocTwo[0], 1, glm::value_ptr(lightDirTwo[0]));

    // second light source
    lightDirTwo[1] = glm::vec3(1.0f, 0.0f, 0.0f);
    lightDirLocTwo[1] = glGetUniformLocation(twoLightShader.shaderProgram, "lightDir[1]");
    glUniform3fv(lightDirLocTwo[1], 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDirTwo[1]));

    lightColorTwo[1] = glm::vec3(0.0f, 1.0f, 0.0f); 
    lightColorLocTwo[1] = glGetUniformLocation(twoLightShader.shaderProgram, "lightColor[1]");
    glUniform3fv(lightColorLocTwo[1], 1, glm::value_ptr(lightDirTwo[1]));

    // set light active
    glUniform1iv(glGetUniformLocation(twoLightShader.shaderProgram, "activeLight"), 2, activeLightTwo);

}


float getRandomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}



void getRandomRainDropPositionAndSpeed(int index) {

    float randomZ = getRandomFloat(0.0f, 0.4f);
    float randomY = getRandomFloat(-0.5f, -0.2f);
    float randomX = getRandomFloat(0.6f, 1.0f);

    rainPosition[index] = glm::vec3(randomX, randomY, randomZ);

    //todo -- make it random
    float randomspeed = getRandomFloat(0.001, 0.01);
    //float randomspeed = 0.1f;
    rainSpeed[index] = glm::vec3(0.0f, randomspeed, 0.0f);



}

void initRain() {

    for (int i = 0; i < CNT_RAIN; i++) {
        getRandomRainDropPositionAndSpeed(i);
    }
}






float delta = 0.0f;
float transValue = -0.01f;
const float MIN_RAIN = -1.6f;

float rotateUfo = 5.0f;
float rotateCow = 5.0f;
float rotateBlades = 30.0f;

void rotationAroundCenter(glm::mat4 anim_model, glm::vec3 rotationCenter, float rotAngle, glm::vec3 rotationAxis)
{
    glm::mat4 translationToCenter = glm::translate(glm::mat4(1.0f), -rotationCenter);

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotAngle), rotationAxis);

    glm::mat4 translationBack = glm::translate(glm::mat4(1.0f), rotationCenter);


    glm::mat4 sceneRot = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));

    glCheckError();


    anim_model = sceneRot * translationBack * rotation * translationToCenter * anim_model;

    glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(anim_model));
    glCheckError();


    // Set the model matrix in your shader
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void renderAnimation() {

    glCheckError();

    animationShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glCheckError();


    /*
      HELICOPTER BLADES
  */


  /*animatedModel = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
  glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));*/

    glm::vec3 coordHelicopter(-0.968808f, -0.335329f, -0.004336f);
    animatedModel = glm::translate(glm::mat4(1.0f), coordHelicopter);
    glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(animatedModel));

    rotationAroundCenter(animatedModel, coordHelicopter, rotateBlades, glm::vec3(0.0f, 1.0f, 0.0f));
    rotateBlades -= 30.0f;
    helicopter_blade.Draw(animationShader);
    glCheckError();



    /*
        UFO
    */
    glm::vec3 coordUFO(-0.275675f, -0.387558f, 0.696323f);
    animatedModel = glm::translate(glm::mat4(1.0f), coordUFO);
    glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(animatedModel));

    rotationAroundCenter(animatedModel, coordUFO, rotateUfo, glm::vec3(0.0f, 1.0f, 0.0f));

    rotateUfo += 5.0f;
    ufo.Draw(animationShader);


    /*
        COW
    */
    glm::vec3 coordCow(-0.281022f, -0.490068f, 0.688132f);
    animatedModel = glm::translate(glm::mat4(1.0f), coordCow);
    glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(animatedModel));

    rotationAroundCenter(animatedModel, coordCow, rotateCow, glm::vec3(0.0f, 1.0f, 0.0f));
    rotateCow -= 5.0f;
    cow.Draw(animationShader);




    /*

        RAIN

    */


    if (delta < MIN_RAIN) {
        delta = 0;
    }

    if (rainMode == true) {

        if (delta == 0) {
            for (int i = 0; i < CNT_RAIN; i++) {

                getRandomRainDropPositionAndSpeed(i);

                rainModel[i] = glm::translate(glm::mat4(1.0f), rainPosition[i]);
                glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(rainModel[i]));



            }
        }

        for (int i = 0; i < CNT_RAIN; i++) {
            glm::vec3 newPosition = delta * rainSpeed[i];
            rainModel[i] = glm::translate(rainModel[i], newPosition);

            std::cout << newPosition[0] << " " << newPosition[1] << " " << newPosition[2] << "\n";

            glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(rainModel[i]));



            rain[i].Draw(animationShader);
        }

    }
    delta += transValue;
    glCheckError();

}

void renderBasicObjects(gps::Shader shader) {


    shader.useShaderProgram();

    scene.Draw(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //house.Draw(shader);

}

void renderTransparentObjects(gps::Shader shader) {

    //myBasicShader.useShaderProgram();
    //glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));


    shader.useShaderProgram();
    greenhouse.Draw(shader);
    lake.Draw(shader);
    ufo_ray.Draw(shader);


}

void renderTreesLikeObjects(gps::Shader shader) {
    shader.useShaderProgram();

    //todo : add one more object
    plants.Draw(shader);
    trees.Draw(shader);
    cornfield.Draw(shader);

}




void drawObjects(gps::Shader shader, bool depthPass) {

    shader.useShaderProgram();


    model = glm::mat4(1.0f);
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));


    //renderBasicObjects(myBasicShader);

    /*   model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
       model = glm::scale(model, glm::vec3(0.5f));
       glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));*/

       // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    scene.Draw(shader);
}


//float cameraSpeed = 0.1f;
//float delta = 0.0f;

//////////


double animationSpeed = 0.01f;

int forward_count = 0;
int backward_count = 0;
int forward_count_two = 0;

void scenePresentation() {


    if (presentationMode == true) {

        if (forward_count < 190)
        {

            myCamera.move(gps::MOVE_FORWARD, animationSpeed);
            forward_count++;

        }
        else if (backward_count < 400) {
            /* myCamera.move(gps::MOVE_RIGHT, animationSpeed);
             right_count++;*/
            myCamera.rotate(0.15f, -0.3f);
            backward_count++;


        }
        else if (backward_count < 600) {
            myCamera.move(gps::MOVE_BACKWARD, animationSpeed);
            backward_count++;
        }
        else if (forward_count_two < 120) {
            myCamera.rotate(-0.15f, -0.15f);
            forward_count_two++;
        }
        else if (forward_count_two < 400) {
            myCamera.move(gps::MOVE_FORWARD, animationSpeed);
            forward_count_two++;
        }


        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %d\n", forward_count);
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %d\n", backward_count);
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %d\n", forward_count_two);

        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        transparentShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(transparentShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        treesShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(treesShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        animationShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(animationShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        twoLightShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(twoLightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    }
}





void animateThunder() {


    if (thunder && rainMode)
        valAtt = 10.0f;
    else
        valAtt = 1.0f;

    myBasicShader.useShaderProgram();
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "valAtt"), valAtt);



}

void renderScene() {
    //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    myBasicShader.useShaderProgram();

    depthMapShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);



    drawObjects(depthMapShader, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //render scene

    // render depth map on screen - toggled with the M key

    if (showDepthMap) {
        // glViewport(0, 0, retina_width, retina_height);
        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {

        // final scene rendering pass (with shadows)

        //glViewport(0, 0, retina_width, retina_height);
        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir));

        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight"), pointLight);

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));

        glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity"), fogDensity);

        animateThunder();
        //glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "valAtt"), valAtt);

        drawObjects(myBasicShader, false);

        //render the scene
        renderAnimation();
        renderTransparentObjects(transparentShader);
        renderTreesLikeObjects(treesShader);
        //renderBasicObjects(myBasicShader);
        twoLightShader.useShaderProgram();

        light_cow.Draw(twoLightShader);
        light_chicken.Draw(twoLightShader);
        light_alien.Draw(twoLightShader);


        //draw a white cube around the light

        //lightShader.useShaderProgram();

        //glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        //model = lightRotation;
        //model = glm::translate(model, lightDir);
        //model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        //glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        //lightCube.Draw(lightShader);
    }
    skyboxShader.useShaderProgram();
    mySkyBox.Draw(skyboxShader, view, projection);



}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    initRain();
    initFBO();
    initSkyBox();

    setWindowCallbacks();


    float lastTime = glfwGetTime();
    float interval = 3.0f;
    float interval2 = 1.0f;

    glCheckError();
    // application loop



    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();

        std::cout << glfwGetTime() << " --- " << glfwGetTime() - lastTime << "\n";
        if (glfwGetTime() - lastTime > interval) {
            //animateThunder();
            thunder = true;
           
            lastTime = glfwGetTime();


        }
        else  if (glfwGetTime() - lastTime > interval2) {
            thunder = false;
        }

        renderScene();
        glCheckError();
        scenePresentation();
        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

    }

    cleanup();

    return EXIT_SUCCESS;
}
