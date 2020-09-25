#ifdef _WIN32
#  include <windows.h>
#  undef max
#  undef min
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#ifdef __APPLE__
#   include <OpenGL/gl3.h>
#   define __gl_h_ /* Prevent inclusion of the old gl.h */
#else
#   include <GL/gl.h>
#endif

//#define GLFW_INCLUDE_GLU
#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#else
#define GL_GLEXT_PROTOTYPES
#endif

/* Global variable */
float gRotateAngles = 0.0f;

/* Helper functions and callbacks */
static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

static double get_seconds() {
	return std::chrono::duration< double >(std::chrono::system_clock::now().time_since_epoch()).count();
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		gRotateAngles += 0.1;
	} else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		gRotateAngles -= 0.1;
	}
}

/* Entry point of the program */
int main(int argc, char** argv) {

	std::cout << "OpenGL demo usage:\n";
	std::cout << "  - use key r to rotate the cube;\n";
	std::cout << "  - use key t to rotate inversely.";

	/* A cube to draw */
	static const GLfloat cubeVertices[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};
	static const GLfloat cubeColors[] = {
		0.583f, 0.771f, 0.014f,
		0.609f, 0.115f, 0.436f,
		0.327f, 0.483f, 0.844f,
		0.822f, 0.569f, 0.201f,
		0.435f, 0.602f, 0.223f,
		0.310f, 0.747f, 0.185f,
		0.597f, 0.770f, 0.761f,
		0.559f, 0.436f, 0.730f,
		0.359f, 0.583f, 0.152f,
		0.483f, 0.596f, 0.789f,
		0.559f, 0.861f, 0.639f,
		0.195f, 0.548f, 0.859f,
		0.014f, 0.184f, 0.576f,
		0.771f, 0.328f, 0.970f,
		0.406f, 0.615f, 0.116f,
		0.676f, 0.977f, 0.133f,
		0.971f, 0.572f, 0.833f,
		0.140f, 0.616f, 0.489f,
		0.997f, 0.513f, 0.064f,
		0.945f, 0.719f, 0.592f,
		0.543f, 0.021f, 0.978f,
		0.279f, 0.317f, 0.505f,
		0.167f, 0.620f, 0.077f,
		0.347f, 0.857f, 0.137f,
		0.055f, 0.953f, 0.042f,
		0.714f, 0.505f, 0.345f,
		0.783f, 0.290f, 0.734f,
		0.722f, 0.645f, 0.174f,
		0.302f, 0.455f, 0.848f,
		0.225f, 0.587f, 0.040f,
		0.517f, 0.713f, 0.338f,
		0.053f, 0.959f, 0.120f,
		0.393f, 0.621f, 0.362f,
		0.673f, 0.211f, 0.457f,
		0.820f, 0.883f, 0.371f,
		0.982f, 0.099f, 0.879f
	};

	/* GLSL shader sources */
	static const char* vertexShaderSrc = "#version 330\n"
			"in vec3 inPosition;"
			"in vec3 inColor;"
			"out vec3 outColor;"
			"uniform mat4 modelMat;"
			"uniform mat4 viewMat;"
			"uniform mat4 projectMat;"
			"void main() {"
			"    gl_Position = projectMat * viewMat * modelMat * vec4(inPosition, 1.0);"
			"    outColor = inColor;"
			"}";
	static const char* fragShaderSrc = "#version 330\n"
			"in vec3 outColor;"
			"out vec4 fragColor;"
			"void main() {"
			"    fragColor = vec4(outColor, 1.0);"
			"}";

	int windowWidth = 640;
	int windowHeight = 480;

	/* Use GLFW to open a window for OpenGL to draw */
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	/* Initialize the library */
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Demo", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
#ifdef _WIN32
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		exit(EXIT_FAILURE);
	}
#endif
	// Handle retina displays (windows and mac)
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	int width_window, height_window;
	glfwGetWindowSize(window, &width_window, &height_window);
	double highdpi = width / width_window;
	glfwSetWindowSize(window, windowWidth / highdpi, windowHeight / highdpi);

	/* Set call back */
	glfwSetKeyCallback(window, key_callback);

	/* Bind data to opengl buffers and compile shaders */
	GLuint vertexArray; // VAO
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW);
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(fragShader);
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);
	GLint vertexLocation = glGetAttribLocation(program, "inPosition");
	GLint colorLocation = glGetAttribLocation(program, "inColor");
	GLint modelMatLocation = glGetUniformLocation(program, "modelMat");
	GLint viewMatLocation = glGetUniformLocation(program, "viewMat");
	GLint projectMatLocation = glGetUniformLocation(program, "projectMat");

	/* Self-maintained model, view and projection matrix used in shaders. */
	glm::mat4 viewMat = glm::lookAt(glm::vec3(4.0f, 4.0f, 3.0f),
	                                glm::vec3(0.0f, 0.0f, 0.0f),
	                                glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projectMat = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 10.0f);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		double tic = get_seconds();

		glViewport(0, 0, windowWidth, windowHeight);
		/* Render here */
		glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		/*Rotate the cube by applying a rotation matrix to the model matrix */
		glm::mat4 modelMat = glm::rotate(glm::mat4(1.0f), gRotateAngles, glm::vec3(0.0f, 1.0f, 0.0f));

		glUseProgram(program);

		glEnableVertexAttribArray(vertexLocation);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(colorLocation);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glUniformMatrix4fv(modelMatLocation, 1, GL_FALSE, &modelMat[0][0]);
		glUniformMatrix4fv(viewMatLocation, 1, GL_FALSE, &viewMat[0][0]);
		glUniformMatrix4fv(projectMatLocation, 1, GL_FALSE, &projectMat[0][0]);

		/* Draw the cube */
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

		glDisableVertexAttribArray(vertexLocation);
		glDisableVertexAttribArray(colorLocation);

		glDisable(GL_DEPTH_TEST);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		glfwPollEvents();
		// In microseconds
		double duration = 1000000. * (get_seconds() - tic);
		const double min_duration = 1000000. / 30.; // 30 fps
		if (duration < min_duration) {
			std::this_thread::sleep_for(std::chrono::microseconds((int)(min_duration - duration)));
		}
	}

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vertexArray);

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
