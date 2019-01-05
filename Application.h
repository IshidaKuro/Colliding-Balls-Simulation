#pragma once



// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// project includes
#include "Camera.h"
#include "Mesh.h"


class Application
{

public:
	Application();
	~Application();

	// static variables for callback function
	// window
	static const GLuint WIDTH = 800;
	static const GLuint HEIGHT = 600;
	static int SCREEN_WIDTH, SCREEN_HEIGHT;

	// Camera
	static Camera camera;
	static double lastX;
	static double lastY;

	static bool firstMouse; 
	static bool keys[1024];


	// get and set methods
	GLFWwindow* getWindow() { return m_window; }

	// Function prototypes
	
	//static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
	//void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);
	//void mouseCallback(GLFWwindow *window, double xPos, double yPos);
	void doMovement(GLfloat deltaTime);
	int initRender();

	// other functions
	void clear();
	void draw(const Mesh &mesh);
	void display();
	void terminate(){ glfwTerminate(); }
	void showFPS();

private:

	// view and projection matrices
	glm::mat4 m_view = glm::mat4(1.0f);
	glm::mat4 m_projection = glm::mat4(1.0f);

	// window
	GLFWwindow* m_window = NULL;

};

