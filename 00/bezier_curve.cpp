/*
 * g++ main.cpp -lGLEW -lglfw -lGL -lX11 -lpthread -lXrandr -lXi
 */
#ifndef _BASE_INCLUDE_HPP
#define _BASE_INCLUDE_HPP

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#endif // !_BASE_INCLUDE_HPP


#ifndef CURVE_HPP
#define CURVE_HPP

#define CURVE_SEG_NUM 100

class curve
{
public:
	curve();
	~curve();
	void get_key_input();

	std::vector<GLfloat> mouse_data;

	int get_degree() {
		return degree;
	}
	int get_points_number() {
		return static_cast<int>(points_interval.size() - degree * 2 + 2);
	}

	void cal_temp_points();
	void cal_bezdata();
	std::vector<GLfloat> bezdata{};
private:
	void cal_u_vector(int need_points, int dim);
	std::vector<std::vector<float>> u_vector{};


	std::vector<std::vector<GLfloat>> bmatrix{
		{ -1,3,-3,1 },
		{ 3,-6,3,0 },
		{ -3,3,0,0 },
		{ 1,0,0,0 },
	};

	std::vector<GLfloat> temp_points_x_y{}; //cal_temp_points



	uint32_t line_seg = CURVE_SEG_NUM;
	int degree{};
	std::vector<float> points_interval{};
	
};

curve::curve() { cal_u_vector(line_seg, 4); }
curve::~curve() {}

void curve::get_key_input() {


	//get degree 
	int d;
	std::cout << "please keyin integer degree" << std::endl;
	std::cin >> d;
	assert(d >= 0);
	degree = d;


	//get intervals
	float weight{};
	std::cout << "please keyin interval between each point" << std::endl;
	std::cout << "(if -1 stop )" << std::endl;
	while (weight >= 0) {
		std::cin >> weight;
		if (weight >= 0)
			points_interval.push_back(weight);
	};

	float sum_temp{};
	sum_temp = std::accumulate(points_interval.begin(), points_interval.end(), sum_temp);
	//std::cout << "accumulate:" << sum_temp << std::endl;
	for (auto i = 0; i < points_interval.size(); ++i) {
		points_interval[i] /= sum_temp;
	}
	for (auto i = 0; i < (points_interval.size() - 1); ++i) {
		points_interval[i + 1] += points_interval[i];

	}
	points_interval.insert(points_interval.begin(), d, 0);
	points_interval.insert(points_interval.end(), d - 1, 1);

	std::cout << "weights are:" << std::endl;
	std::for_each(
		points_interval.begin(),
		points_interval.end(),
		[](auto var) {std::cout << var << ","; }
	);
	std::cout << std::endl;

}

inline void curve::cal_temp_points() {
	int vetex_num = static_cast<int>(mouse_data.size() / 2);


	for (int i = 0; i < vetex_num; i++) {
		float temp_x = 0;
		for (int j = 0; j < vetex_num; ++j) {
			temp_x += bmatrix[i][j] * mouse_data[2 * j];
		}
		temp_points_x_y.push_back(temp_x);

		float temp_y = 0;
		for (int j = 0; j < vetex_num; ++j) {
			temp_y += bmatrix[i][j] * mouse_data[2 * j + 1];
		}
		temp_points_x_y.push_back(temp_y);
	}


}

inline void curve::cal_bezdata() {
	int vetex_num = static_cast<int>(temp_points_x_y.size() / 2);

	for (int i = 0; i < static_cast<int>(line_seg); ++i) {
		float temp_x = 0;
		for (int j = 0; j < vetex_num; ++j) {
			temp_x += u_vector[i][j] * temp_points_x_y[2 * j];
		}
		bezdata.push_back(temp_x);

		float temp_y = 0;
		for (int j = 0; j < vetex_num; ++j) {
			temp_y += u_vector[i][j] * temp_points_x_y[2 * j + 1];
		}
		bezdata.push_back(temp_y);

		bezdata.push_back(0.0f);
		bezdata.push_back(0.0f);
		bezdata.push_back(1.0f);
		bezdata.push_back(0.0f);


	}
}

inline void curve::cal_u_vector(int need_points, int dim) {
	float space = 1.0f / (need_points - 1);
	u_vector.resize(need_points);
	float u_temp = 0;
	for (int i = 0; i < need_points; ++i) {

		for (int j = 0; j < dim; ++j)
			u_vector[i].push_back(pow(u_temp, j));
		std::reverse(u_vector[i].begin(), u_vector[i].end());
		u_temp += space;
	}
}

#endif // !CURVE_HPP

#ifndef MAIN_HPP
#define MAIN_HPP

// Shaders
const GLchar* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0);\n"
"ourColor = color;\n"
"}\0";

const GLchar* fragmentShaderSource =
"#version 330 core\n"
"in vec3 ourColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(ourColor, 1.0f);\n"
"}\n\0";



#endif // !MAIN_HPP

#ifndef MAIN_CPP
#define MAIN_CPP

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void popup_menu(GLFWwindow* window, int button, int action, int mods);
std::vector<double> mouse_x_y;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "glfw_draw", nullptr, nullptr);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Build and compile our shader program
	GLint success{};
	GLchar infoLog[512]{};

	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
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


#define DATA_PER_VERTEX 6
	// Set up vertex data (and buffer(s)) and attribute pointers
#define USE_USER_DATA
#if !defined(USE_USER_DATA)
	std::vector<GLfloat> vertices{
		// Positions         // Colors
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom Left
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   // Top 
		1.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // Test data
		-1.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // Test data
		1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,   // Test data
		1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,   // Test data

	};
#else //defined USE_USER_DATA
	std::vector<GLfloat> vertices{};
#endif //!USE_USER_DATA

	bool get_enough_data = false;
	int need_mouse = 14;



	GLuint VAO{};
	GLuint VBO{};
	bool clean_previous = true;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
#if defined(USE_USER_DATA)
		//std::cout << "mouse_x_y.size()" << mouse_x_y.size() << std::endl;
		if (mouse_x_y.size() >= 8) {
			if (clean_previous) {
				glDeleteVertexArrays(1, &VAO);
				glDeleteBuffers(1, &VBO);
				vertices.resize(0);
			}
			curve curve_data;
			for (int i = 0; i < mouse_x_y.size(); i += 2) {
				float x_temp = static_cast<float>(mouse_x_y[i] / (WIDTH / 2) - 1);
				float y_temp = static_cast<float>(-mouse_x_y[i + 1] / (HEIGHT / 2) + 1);
				vertices.push_back(x_temp);
				vertices.push_back(y_temp);
				vertices.push_back(0.0f);
				vertices.push_back(1.0f);
				vertices.push_back(0.0f); 
				vertices.push_back(0.0f);
				curve_data.mouse_data.push_back(x_temp);
				curve_data.mouse_data.push_back(y_temp);

				if (i == mouse_x_y.size() - 2) {
					vertices.push_back(x_temp);
					vertices.push_back(y_temp);
					vertices.push_back(0.0f);
					vertices.push_back(0.1f);
					vertices.push_back(0.1f);
					vertices.push_back(0.1f);

				}
			}
			
			mouse_x_y.resize(0);
			curve_data.cal_temp_points();
			curve_data.cal_bezdata();
			
			vertices.push_back(curve_data.bezdata[0]);
			vertices.push_back(curve_data.bezdata[1]);
			vertices.push_back(curve_data.bezdata[2]);
			vertices.push_back(0.1f);
			vertices.push_back(0.1f);
			vertices.push_back(0.1f);
			
			std::for_each(
				curve_data.bezdata.begin(),
				curve_data.bezdata.end(),
				[&vertices](auto var) {vertices.push_back(var); }
			);


#endif
			glGenVertexArrays(1, &VAO);
			// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
			glBindVertexArray(VAO);


			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

			// Position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, DATA_PER_VERTEX * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			// Color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, DATA_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			glBindVertexArray(0); // Unbind VAO

			// Render
			// Clear the colorbuffer
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Draw the triangle
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);
			//draw ( GL_POINTS | GL_TRIANGLES | GL_LINE_STRIP )
			glDrawArrays(GL_LINE_STRIP, 0, static_cast<int>(vertices.size()/ DATA_PER_VERTEX));
			glBindVertexArray(0);

			// Swap the screen buffers
			glfwSwapBuffers(window);
#if defined(USE_USER_DATA)
		}
#endif

	}

	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(
	GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void mouse_button_callback(
	GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		popup_menu(window, button, action, mods);
}

void popup_menu(GLFWwindow* window, int button, int action, int mods){
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	mouse_x_y.push_back(xpos);
	mouse_x_y.push_back(ypos);
	std::cout << "push[CursorPos(x,y)]=(" << xpos << "," << ypos << ")" << std::endl;	
}

#endif // !MAIN_CPP
