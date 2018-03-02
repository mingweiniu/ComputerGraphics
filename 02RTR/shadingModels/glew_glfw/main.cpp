#ifndef MAIN_CPP
#define MAIN_CPP
/*
 * if UNIX, compile with
 *     g++ main.cpp -L/usr/local/lib -lGL -lGLEW -lglfw -o shadingModels
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>

#define GLEW_STATIC      // GLEW
#include <GL/glew.h>     // GLEW
#include <GLFW/glfw3.h>  //glfw
//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "shader.hpp"
#include "camera.hpp"
#include "LoadObj.hpp"

std::clock_t clock_cnt_start = std::clock();
std::clock_t clock_cnt_end{};
bool couont{};

enum class Shading { FLAT, Gouraud, Phong, };
Shading model = Shading::Phong;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();
bool keys[1024];
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;
// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
// Window dimensions
const GLuint WIDTH = 600, HEIGHT = 600;
void MVP(Camera camera, Shader shader);



// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "enter at clock " << clock_cnt_start << "." << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "'F'lat \'G'ouraud \'P'hong", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);

	// Build and compile our shader program
	Shader ourShader("shader/teapot.vert", "shader/teapot.frag");

	const int DATA_PER_VERTEX = 9;
	std::vector<GLfloat> vertices{
		//0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,    // Test data a1
		//0.5f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,    // Test data a2
		//0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,   // Test data a3

		//-5.0f,  -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // Test data b1
		//-1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,   // Test data b2
		//0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 1.0f,   // Test data b3
	};

	const char* teapot_dir = "module/sTeapot.obj";
	std::vector<glm::vec4> teapot_vertices{};
	std::vector<glm::vec3> teapot_normals{};
	std::vector<GLushort> teapot_elements{};
	LoadObj(teapot_dir, teapot_vertices, teapot_normals, teapot_elements);
	std::cout << teapot_dir << " has " << teapot_vertices.size() << " vertices, " << teapot_normals.size() << " normal," << teapot_elements.size() << " fragments." << std::endl;

	//calculating Gouraud_normals
	std::vector<std::vector<glm::vec3>> normals_of_vertices(teapot_vertices.size());
	std::vector<glm::vec3> Gouraud_normals{};
	for (int i = 0;i < teapot_elements.size(); ++i) {
		normals_of_vertices[teapot_elements[i]].push_back(teapot_normals[teapot_elements[i]]);
	}
	for (int i = 0;i < normals_of_vertices.size(); ++i) {
		glm::vec3 temp_normal{};
		std::for_each(
			normals_of_vertices[i].begin(),
			normals_of_vertices[i].end(),
			[&](glm::vec3 normals) {
			temp_normal += normals;
		});
		Gouraud_normals.push_back(glm::normalize(temp_normal));
	}

	//push date into array buffer
	vertices.reserve(teapot_elements.size() * 3);
	for (int i = 0;i < teapot_elements.size(); i += 6) {
		for (int j = 0; j < 6;++j) {
			//vertex
			vertices.push_back(teapot_vertices[teapot_elements[i + j]].x);
			vertices.push_back(teapot_vertices[teapot_elements[i + j]].y);
			vertices.push_back(teapot_vertices[teapot_elements[i + j]].z);
			//Flat_normal
			vertices.push_back(teapot_normals[teapot_elements[i]].x);
			vertices.push_back(teapot_normals[teapot_elements[i]].y);
			vertices.push_back(teapot_normals[teapot_elements[i]].z);
			//Gouraud_normals
			vertices.push_back(Gouraud_normals[teapot_elements[i + j]].x);
			vertices.push_back(Gouraud_normals[teapot_elements[i + j]].y);
			vertices.push_back(Gouraud_normals[teapot_elements[i + j]].z);
		}
	}

	GLuint VAO{};
	glGenVertexArrays(1, &VAO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	GLuint VBO{};
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, DATA_PER_VERTEX * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Flat_normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, DATA_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//Gouraud_normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, DATA_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw the triangle
		ourShader.Use();
		MVP(camera, ourShader);

		GLint FlatLoc = glGetUniformLocation(ourShader.Program, "Flat");
		if (model == Shading::FLAT) {
			glUniform1i(FlatLoc, 1);
		}else {
			glUniform1i(FlatLoc, 0);
		}
		GLint phongLoc = glGetUniformLocation(ourShader.Program, "Phong");
		if (model == Shading::Phong) {
			glUniform1i(phongLoc, 1);
		}else {
			glUniform1i(phongLoc, 0);
		}

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(vertices.size() / DATA_PER_VERTEX));
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
		
		clock_cnt_end = std::clock();
		if (!couont) {
			couont = true;
			std::cout << "first swap at clock : " << clock_cnt_end << "." << std::endl;
			float totaltime = static_cast<float> (clock_cnt_end - clock_cnt_start) / CLOCKS_PER_SEC;
			std::cout << totaltime << " second is used." << std::endl;
		}
		std::cout << "FPS is " << static_cast<double>( CLOCKS_PER_SEC/ static_cast<double> (clock_cnt_end - clock_cnt_start) )<< std::endl;
		clock_cnt_start = clock_cnt_end;

	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

void MVP(Camera camera,Shader shader) {
	glm::mat4 model;
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	
	// Get the uniform locations
	GLint modelLoc = glGetUniformLocation(shader.Program, "model");
	GLint viewLoc = glGetUniformLocation(shader.Program, "view");
	GLint projLoc = glGetUniformLocation(shader.Program, "projection");

	// Pass the matrices to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

}



// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024){
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos){
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset) {
}
void do_movement() {
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(Move::FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(Move::BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(Move::LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(Move::RIGHT, deltaTime);
	if (keys[GLFW_KEY_P])
		model = Shading::Phong;
	if (keys[GLFW_KEY_F])
		model = Shading::FLAT;
	if (keys[GLFW_KEY_G])
		model = Shading::Gouraud;
}

#endif // !MAIN_CPP
