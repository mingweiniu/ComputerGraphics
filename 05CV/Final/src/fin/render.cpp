#include <fin/render.hpp>


#include <iostream>
#include <vector>
#include <string>

#include <cmath>
#include <ctime>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

fin::render::render()
{
	init();
	createShader();
}

fin::render::~render()
{
}

int fin::render::draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// TexCoord attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO
	// Game loop
	float counter{};
	while (!glfwWindowShouldClose(window))
	{
		using glm::vec3;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw the triangle
		glUseProgram(shaderProgram);
		float fov = 45.0f;
		auto projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 10.0f);
		counter += 0.01f;
		auto cameraPos = vec3(1.0f, 0.5f-std::cos(counter) / 2, 3.0f);
		auto cameraFront = vec3(0.0f, std::cos(counter) / 2, -1.0f);
		auto cameraUp = vec3(-1.0f, 0.0f, 0.0f);
		auto view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		MVP = projection * view;
		
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
		
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(vertices.size() / (sizeof(DrawVertex) / sizeof(GLfloat))));
		glPointSize(5);
		glDrawArrays(GL_POINTS, 0, static_cast<int>(vertices.size() / (sizeof(DrawVertex) / sizeof(GLfloat))));
		glBindVertexArray(0);

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

void fin::render::init()
{
	using std::cout;

	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(Width, Height, "CV_final", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window\n";
		glfwTerminate();
	}


	// Set the required callback functions
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

	});

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, Width, Height);


	glGenVertexArrays(1, &VAO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);


}

void fin::render::createShader()
{
	using std::string;

	string vertexShaderSource =
		"#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"layout (location = 1) in vec3 color;\n"
		"uniform mat4 MVP;\n"
		"out vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"gl_Position = MVP * vec4(position, 1.0);\n"
		"ourColor = color;\n"
		"}\0";

	string fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 ourColor;\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"color = vec4(ourColor, 1.0f);\n"
		"}\n\0";
	shaderProgram = glCreateProgram();
	shader(vertexShaderSource, ShaderType::Vertex);
	shader(fragmentShaderSource, ShaderType::Fragment);

}

void fin::render::shader(const std::string &Shader, ShaderType Type)
{
	int ShaderBuffer{};
	if (Type == ShaderType::Vertex) {
		ShaderBuffer = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (Type == ShaderType::Fragment) {
		ShaderBuffer = glCreateShader(GL_FRAGMENT_SHADER);
	}
	else {
		return ;
	}
	const char *ShaderCode = Shader.c_str();
	glShaderSource(ShaderBuffer, 1, &ShaderCode, NULL);
	glCompileShader(ShaderBuffer);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(ShaderBuffer, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ShaderBuffer, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glAttachShader(shaderProgram, ShaderBuffer);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(ShaderBuffer);

}
