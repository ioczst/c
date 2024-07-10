#pragma once
// #include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

const int steps = 50;
const float stepAngle = 3.1415926f * 2.f / steps;

int main() {
    GLFWwindow* window;    
    if (!glfwInit()) {
        std::cout << "Init error";
        return -1;
    }
    
    window = glfwCreateWindow(800,600,"Hello",0,0);
    if (!window) {
        std::cout << "Window creation error";
        glfwTerminate();
        return -1;
    }    
    glfwMakeContextCurrent(window);
    
    // render loop
    while (!glfwWindowShouldClose(window)) {
        
        glClearColor(1.0,1.0,1.0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        float xCenter = 0.0f;
        float yCenter = 0.0f;
        float radius = 1.f;
        
        // first point
        float rx = xCenter;
        float ry = yCenter-radius;
        
        for (int i=0;i<=steps;i++) {
            float angle = stepAngle*i;
            float newRx = radius*sinf(angle);
            float newRy = -radius*cosf(angle);
            glBegin(GL_TRIANGLES);
            glColor3f(0.f,0.75f,0.f);
            glVertex3f(0.f,0.f,0.f);
            glVertex3f(rx,ry,0.f);
            glVertex3f(newRx,newRy,0.f);
            glEnd();
            rx = newRx;
            ry = newRy;
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}


