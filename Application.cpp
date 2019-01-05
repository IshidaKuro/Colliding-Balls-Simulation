#include <memory>
#include <functional>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"

#include "Application.h"


int Application::SCREEN_WIDTH = 0;
int Application::SCREEN_HEIGHT = 0;

// Camera
Camera Application::camera = Camera::Camera(glm::vec3(0.0f, 0.0f, 0.0f));;
double Application::lastX = WIDTH / 2.0;
double Application::lastY = HEIGHT / 2.0;

bool Application::firstMouse = true;
bool Application::keys[1024];


Application::Application()
{
}


Application::~Application()
{
}


// Moves/alters the camera positions based on user input
void Application::doMovement(GLfloat deltaTime)
{
	glfwPollEvents();

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

// Is called whenever a key is pressed/released via GLFW
static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			Application::keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			Application::keys[key] = false;
		}
	}
}

static void mouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (Application::firstMouse)
	{
		Application::lastX = xPos;
		Application::lastY = yPos;
		Application::firstMouse = false;
	}

	double xOffset = xPos - Application::lastX;
	double yOffset = Application::lastY - yPos;

	Application::lastX = xPos;
	Application::lastY = yPos;

	Application::camera.ProcessMouseMovement((GLfloat)xOffset, (GLfloat)yOffset);
}


static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
	Application::camera.ProcessMouseScroll((GLfloat)yOffset);
}

// Renderer initialisation
int Application::initRender() {
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	m_window = glfwCreateWindow(WIDTH, HEIGHT, "Physics-Based Animation", nullptr, nullptr);

	if (nullptr == m_window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(m_window);
	glfwGetFramebufferSize(m_window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(m_window, keyCallback);
	glfwSetCursorPosCallback(m_window, mouseCallback);
	glfwSetScrollCallback(m_window, scrollCallback);

	// remove the mouse cursor
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// moder GLEW approach
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return 1;
}

// clear buffer
void Application::clear() {
	// Clear the colorbuffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


// draw mesh
// draw mesh
void Application::draw(const Mesh &mesh)
{
	mesh.getShader().Use();
	// view and projection matrices
	m_projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);
	m_view = camera.GetViewMatrix();

	// Get the uniform locations
	GLint modelLoc = glGetUniformLocation(mesh.getShader().Program, "model");
	GLint viewLoc = glGetUniformLocation(mesh.getShader().Program, "view");
	GLint projLoc = glGetUniformLocation(mesh.getShader().Program, "projection");
	GLint rotateLoc = glGetUniformLocation(mesh.getShader().Program, "rotate");


	// Pass the matrices to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mesh.getModel()));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(rotateLoc, 1, GL_FALSE, glm::value_ptr(mesh.getRotate()));


	glBindVertexArray(mesh.getVertexArrayObject());
	glDrawArrays(GL_TRIANGLES, 0, mesh.getNumIndices());
	glBindVertexArray(0);
}

void Application::display() {
	glBindVertexArray(0);
	// Swap the buffers
	glfwSwapBuffers(m_window);
}

void Application::showFPS() {

	static double previousSeconds = 0.0;

	static int frameCount;

	double elapsedSeconds;

	double currentSeconds = glfwGetTime();



	elapsedSeconds = currentSeconds - previousSeconds;



	// limit FPS refresh rate to 4 times per second

	if (elapsedSeconds > 0.25) {

		previousSeconds = currentSeconds;

		double FPS = (double)frameCount / elapsedSeconds;

		double msPerFrame = 1000.0 / FPS;



		std::ostringstream outs;

		outs.precision(3);

		outs << std::fixed

			<< "Physics Simulation  "

			<< "FPS: " << FPS << "  "

			<< "Frame time: " << msPerFrame << "ms" << std::endl;

		glfwSetWindowTitle(m_window, outs.str().c_str());



		frameCount = 0;

	}

	frameCount++;

}