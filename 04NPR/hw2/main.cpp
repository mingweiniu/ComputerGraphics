#include <functional>
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>

#include <cstddef>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
bool mouse_R = false;
struct mouse { double xpos; double ypos; };
mouse mouse_get, mouse_current;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void key_movement(); 
void do_movement();
void clean();
bool keys[1024];

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const GLchar* vertexShaderSource = 
"#version 330 core\n"
"uniform int WIDTH;\n"
"uniform int HEIGHT;\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 surface;\n"
"layout (location = 2) in vec4 middle;\n"
"layout (location = 3) in vec4 deep;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"gl_Position = vec4((position.x-WIDTH)/WIDTH , (HEIGHT-position.y)/HEIGHT, 0.0f, 1.0f );\n"
"fragColor = vec4(surface.xyz + middle.xyz + deep.xyz , 1);\n"
"}\0"
;

const GLchar* fragmentShaderSource = 
"#version 330 core\n"
"in vec4 fragColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1-fragColor.x, 1-fragColor.y, 1-fragColor.z, 1.0);\n"
"}\0"
;


int ink_contain = 40;
int ink_start = ink_contain;

enum class Water { Dry = 1, Wet = 10};
Water water = Water::Dry;

enum class Paper {	First,	Second, Third, size,};
Paper paper = Paper::Third;
Paper pixelType = Paper::First;

enum class color {	R,	G,	B,	size,};
color pen = color::R;

struct Radius {
	int A = 15;
	int B = 15;
	int C = 40;
};
Radius radius;

struct Vertex {
	float pos[4]{};
	float surface[4]{};
	float middle[4]{};
	float deep[4]{};
};
std::vector<Vertex> vertices(800 * 600, Vertex{});

void thread_it(std::function<void(void)> target){ std::thread T(target); T.join(); }


// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	for (int x = 0; x < WIDTH;++x) {
		for (int y = 0; y < HEIGHT;++y) {
			vertices[x + y * WIDTH].pos[0] = static_cast<float>(x);
			vertices[x + y * WIDTH].pos[1] = static_cast<float>(y);
		}
	}

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	
	// Game loop
	while (!glfwWindowShouldClose(window))
	{		
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		thread_it(key_movement);
		thread_it(do_movement);

		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, pos));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, surface));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, middle));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, deep));
		glEnableVertexAttribArray(3);

		// Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindVertexArray(NULL); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

		// Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		const int neighbor = 4;

		int potions[neighbor]{};
		for (int x = 1; x < WIDTH-1 ; ++x) {
			for (int y = 1; y < HEIGHT - 1; ++y) {
				auto local = x + 0 + (y + 0) * WIDTH;

				if (pixelType == Paper::First) {
					potions[0] = x + 1 + (y + 0) * WIDTH;
					potions[1] = x - 1 + (y + 0) * WIDTH;
					potions[2] = x - 0 + (y + 1) * WIDTH;
					potions[3] = x - 0 + (y - 1) * WIDTH;
				}
				else if (pixelType == Paper::Second) {
					potions[0] = x - 1 + (y - 1) * WIDTH;
					potions[1] = x - 1 + (y + 1) * WIDTH;
					potions[2] = x + 1 + (y - 1) * WIDTH;
					potions[3] = x + 1 + (y + 1) * WIDTH;
				}

				for (int c = 0; c < static_cast<int>(color::size); ++c) {
					// diffuse
					for (int i = 0; i < neighbor; ++i) {
						auto speed = vertices[potions[i]].middle[c] - vertices[local].middle[c];
						if (speed > (0.1 / static_cast<int>(water))) {
							vertices[local].middle[c] += speed / 2;
							vertices[potions[i]].middle[c] -= speed / 2;
						}
					}
					// middle
					if (vertices[local].surface[c] > 0.2) {
						vertices[local].surface[c] -= 0.03f;
						vertices[local].middle[c] += 0.03f;
					}
					// deep
					if (vertices[local].middle[c] > 0.1) {
						vertices[local].middle[c] -= 0.01f;
						vertices[local].deep[c] += 0.005f;
					}

				}
			}
		}

		// Draw our first triangle
		glUseProgram(shaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgram, "WIDTH"), WIDTH / 2);
		glUniform1i(glGetUniformLocation(shaderProgram, "HEIGHT"), HEIGHT / 2);

		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, static_cast<int>(vertices.size()));
		glBindVertexArray(NULL);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void key_movement()
{
	if (keys[GLFW_KEY_Z]) { std::thread T(clean);	T.join(); }

	if (keys[GLFW_KEY_C]) pen = color::R;
	if (keys[GLFW_KEY_M]) pen = color::G;
	if (keys[GLFW_KEY_Y]) pen = color::B;

	if (keys[GLFW_KEY_RIGHT]) ink_contain += 1;
	if (keys[GLFW_KEY_LEFT]) if (ink_contain > 2) ink_contain -= 1;

	if (keys[GLFW_KEY_UP]) radius.C += 10;
	if (keys[GLFW_KEY_DOWN]) if (radius.C > 15) radius.C -= 10;

	if (keys[GLFW_KEY_I]) if (radius.B > 3) { radius.A += 2; radius.B -= 2; }
	if (keys[GLFW_KEY_U]) if (radius.A > 3) { radius.A -= 2; radius.B += 2; }

	if (keys[GLFW_KEY_A]) paper = Paper::First;
	if (keys[GLFW_KEY_S]) paper = Paper::Second;
	if (keys[GLFW_KEY_D]) paper = Paper::Third;

	if (keys[GLFW_KEY_W]) water = Water::Wet;
	if (keys[GLFW_KEY_E]) water = Water::Dry;
}

void do_movement() {
	if (mouse_R) {
		if(ink_start > 1){ --ink_start; }
		std::thread T([mouse_current = mouse_current]() {
			for (auto& v : vertices){
				auto x_2 = std::pow(mouse_current.xpos - v.pos[0], 2);
				auto y_2 = std::pow(mouse_current.ypos - v.pos[1], 2);
				if ((x_2 * radius.A + y_2 * radius.B) < radius.C * radius.C) {
					v.surface[static_cast<int>(pen)] += 0.005f * ink_start;
				}
			}
		});
		T.join();
	}
	else if (!mouse_R) {
		ink_start = ink_contain;
	}


	if (paper == Paper::First) {
		pixelType = Paper::First;
	}
	else if (paper == Paper::Second) {
		pixelType = Paper::Second;
	}
	else if (paper == Paper::Third) {
		if (pixelType == Paper::First) {
			pixelType = Paper::Second;
		}
		else if (pixelType == Paper::Second) {
			pixelType = Paper::First;
		}
	}
	

}

void clean() {
	for (auto& v : vertices){
		for (int i = 0; i < static_cast<int>(color::size); ++i) {
			v.surface[i] = {};
			v.middle[i] = {};
			v.deep[i] = {};
		}
	}
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos) {
	mouse_current.xpos = xpos;
	mouse_current.ypos = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (GLFW_PRESS == action)
			mouse_R = true;
		else if (GLFW_RELEASE == action)
			mouse_R = false;
	}
}