#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
int main() {	
	GLFWwindow* window;
	if (!glfwInit()) {
		std::cout << "Init error";
		return -1;
	}
	
	window = glfwCreateWindow(640, 480, "Hello Window!", NULL, NULL);    
	if (!window) {
        std::cout << "Window creation error";
        glfwTerminate();
        return -1;
    }
	glfwMakeContextCurrent(window);

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_TRIANGLES);		

		glColor3f(1.f,0.f,0.f);
		glVertex3f(-0.6f,-0.4f,0.f);
		glColor3f(0.f,1.f,0.f);
		glVertex3f(0.6f,-0.4f,0.f);
		glColor3f(0.f,0.f,1.f);
		glVertex3f(0.f,0.6f,0.f);

		glEnd();

		glfwSwapBuffers(window);
		
	}

	glfwTerminate();
	return 0;
}