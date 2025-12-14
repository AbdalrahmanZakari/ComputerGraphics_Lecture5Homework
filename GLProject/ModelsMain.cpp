#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>

#include "learnopengl/shader_m.h"
#include "learnopengl/camera.h"
#include "learnopengl/model.h"

using namespace std;
using namespace glm;

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

Camera camera(vec3(0.0f, 40.0f, 120.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float earthOrbitAngle = 0.0f;
float moonOrbitAngle = 0.0f;
float earthRotationAngle = 0.0f;

float sunScale = 0.1f;
float earthScale = 0.04f;
float moonScale = 0.2f;

float earthOrbitRadius = 200.0f;
float moonOrbitRadius = 80.0f;

vec3 sunLightPos = vec3(0.0f);
vec3 sunLightColor = vec3(1.0f, 0.98f, 0.95f); 
bool forceSolarEclipse = false;
bool forceLunarEclipse = false;

vector<vec3> earthOrbitPoints;
vector<vec3> moonOrbitPoints;

unsigned int orbitVAO = 0, orbitVBO = 0;

void framebuffer_size_callback(GLFWwindow*, int, int);
void mouse_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);
void processInput(GLFWwindow*);
void updateOrbitPoints();

vector<vec3> createCircle(float r, int s) {
    vector<vec3> p;
    for (int i = 0; i <= s; i++) {
        float a = 2.0f * 3.14159f * i / s;
        p.push_back(vec3(cos(a) * r, 0.0f, sin(a) * r));
    }
    return p;
}

void setupOrbit(const vector<vec3>& p) {
    if (orbitVAO) {
        glDeleteVertexArrays(1, &orbitVAO);
        glDeleteBuffers(1, &orbitVBO);
    }
    glGenVertexArrays(1, &orbitVAO);
    glGenBuffers(1, &orbitVBO);
    glBindVertexArray(orbitVAO);
    glBindBuffer(GL_ARRAY_BUFFER, orbitVBO);
    glBufferData(GL_ARRAY_BUFFER, p.size() * sizeof(vec3), &p[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

float calculateLightIntensity(vec3 objectPos, vec3 sunPos, bool isInShadow) {
    if (isInShadow) {
        return 0.1f; 
    }

   
    float distance = length(objectPos - sunPos);
    float intensity = 200.0f / distance; 

    return glm:: clamp(intensity, 0.5f, 2.0f);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System - HW2", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    Shader planetShader("./shaders/vs/L5.vs", "./shaders/fs/L5-Model.fs");
    Shader orbitShader("./shaders/vs/LightSource.vs", "./shaders/fs/LightSource.fs");

    Model sun("./models/sun/sun.obj");
    Model earth("./models/earth1/earth1.obj");
    Model moon("./models/moon1.obj");

    earthOrbitPoints = createCircle(earthOrbitRadius, 100);

    cout << "G = Solar Eclipse | H = Lunar Eclipse | J = Normal\n";

    while (!glfwWindowShouldClose(window)) {
        float t = glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;

        processInput(window);

        if (!forceSolarEclipse && !forceLunarEclipse) {
            earthOrbitAngle += deltaTime * 0.15f;
            moonOrbitAngle += deltaTime * 0.8f;
            earthRotationAngle += deltaTime * 2.0f;
        }

        updateOrbitPoints();

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 projection = perspective(radians(70.0f),
            (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 1000.0f);
        mat4 view = camera.GetViewMatrix();

        planetShader.use();
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);
        planetShader.setVec3("lightPos", sunLightPos);
        planetShader.setVec3("viewPos", camera.Position);
        planetShader.setBool("blinn", true);

        planetShader.setVec3("lightColor", sunLightColor);
        planetShader.setFloat("lightIntensity", 1.0f);

        float earthX, earthZ, moonX, moonZ;

        if (forceSolarEclipse) {
            earthX = earthOrbitRadius;
            earthZ = 0.0f;
            moonX = earthX - moonOrbitRadius;
            moonZ = 0.0f;
        }
        else if (forceLunarEclipse) {
            earthX = earthOrbitRadius;
            earthZ = 0.0f;
            moonX = earthX + moonOrbitRadius;
            moonZ = 0.0f;
        }
        else {
            earthX = earthOrbitRadius * cos(earthOrbitAngle);
            earthZ = earthOrbitRadius * sin(earthOrbitAngle);
            moonX = earthX + moonOrbitRadius * cos(moonOrbitAngle);
            moonZ = earthZ + moonOrbitRadius * sin(moonOrbitAngle);
        }

        
        vec3 toSun = normalize(vec3(-earthX, 0, -earthZ)); 
        vec3 toMoon = normalize(vec3(moonX - earthX, 0, moonZ - earthZ)); 
        float angle = dot(toSun, toMoon);
        bool isNaturalEclipse = (angle > 0.99f);

        vec3 earthPos = vec3(earthX, 0, earthZ);
        vec3 moonPos = vec3(moonX, 0, moonZ);

       
        mat4 m = scale(mat4(1.0f), vec3(sunScale));
        planetShader.setMat4("model", m);
     
        planetShader.setVec3("objectColor", vec3(10.0f, 9.8f, 9.0f)); 
        sun.Draw(planetShader);

        m = translate(mat4(1.0f), vec3(earthX, 0, earthZ));
        m = rotate(m, earthRotationAngle, vec3(0, 1, 0));
        m = rotate(m, radians(23.5f), vec3(1, 0, 0));
        m = scale(m, vec3(earthScale));
        planetShader.setMat4("model", m);

        bool earthInShadow = false;
        float earthLightMultiplier = 1.0f;

        if (forceSolarEclipse) {
           
            earthInShadow = true;
            earthLightMultiplier = 0.1f;
        }
        else if (forceLunarEclipse) {
          
            if (isNaturalEclipse) {
               
                earthInShadow = true;
                earthLightMultiplier = 0.1f;
            }
            else {
               
                earthLightMultiplier = calculateLightIntensity(earthPos, sunLightPos, false);
            }
        }
        else {
           
            if (isNaturalEclipse) {
                
                earthInShadow = true;
                earthLightMultiplier = 0.1f;
            }
            else {
             
                earthLightMultiplier = calculateLightIntensity(earthPos, sunLightPos, false);
            }
        }

        
        planetShader.setVec3("objectColor", vec3(0.8f, 0.9f, 1.0f) * earthLightMultiplier);
        earth.Draw(planetShader);

       
        m = translate(mat4(1.0f), vec3(moonX, 0, moonZ));
        m = scale(m, vec3(moonScale));
        planetShader.setMat4("model", m);

      
        float moonLightMultiplier = 1.0f;

        if (forceLunarEclipse) {
           
            moonLightMultiplier = 0.1f;
        }
        else {
            
            moonLightMultiplier = calculateLightIntensity(moonPos, sunLightPos, false);
        }

        
        planetShader.setVec3("objectColor", vec3(1.2f, 1.2f, 1.1f) * moonLightMultiplier);
        moon.Draw(planetShader);

        orbitShader.use();
        orbitShader.setMat4("projection", projection);
        orbitShader.setMat4("view", view);

        setupOrbit(earthOrbitPoints);
        orbitShader.setVec3("objectColor", vec3(0.5f, 0.7f, 1.0f));
        glBindVertexArray(orbitVAO);
        glDrawArrays(GL_LINE_LOOP, 0, earthOrbitPoints.size());
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* w) {
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(w, true);

    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    static bool g = false, h = false, j = false;
    if (glfwGetKey(w, GLFW_KEY_G) == GLFW_PRESS && !g) { forceSolarEclipse = true;forceLunarEclipse = false;g = true; }
    if (glfwGetKey(w, GLFW_KEY_G) == GLFW_RELEASE) g = false;
    if (glfwGetKey(w, GLFW_KEY_H) == GLFW_PRESS && !h) { forceLunarEclipse = true;forceSolarEclipse = false;h = true; }
    if (glfwGetKey(w, GLFW_KEY_H) == GLFW_RELEASE) h = false;
    if (glfwGetKey(w, GLFW_KEY_J) == GLFW_PRESS && !j) { forceSolarEclipse = false;forceLunarEclipse = false;j = true; }
    if (glfwGetKey(w, GLFW_KEY_J) == GLFW_RELEASE) j = false;
}

void mouse_callback(GLFWwindow*, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    camera.ProcessMouseMovement(xpos - lastX, lastY - ypos);
    lastX = xpos;
    lastY = ypos;
}

void scroll_callback(GLFWwindow*, double, double yoffset) {
    camera.ProcessMouseScroll((float)yoffset);
}

void framebuffer_size_callback(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
    SCR_WIDTH = w;
    SCR_HEIGHT = h;
}

void updateOrbitPoints() {
    moonOrbitPoints.clear();
}