#ifndef FINAL_RENDER_HPP
#define FINAL_RENDER_HPP

#include <string>
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>



namespace fin {
	class render
	{
	public:
		render();
		~render();
		int draw();
		std::vector<GLfloat> vertices;
		glm::mat4 MVP;
	private:
		enum class ShaderType {
			Vertex,
			Fragment
		};
		int Width = 800;
		int Height = 600;
		void init();
		GLFWwindow* window;
		GLuint VAO, VBO;


		void createShader();
		void shader(const std::string &Shader, ShaderType Type);
		int shaderProgram;

		struct DrawVertex {
			float vertex[3];
			float color[3];
		};		
	};

	
}

#endif // !FINAL_RENDER_HPP