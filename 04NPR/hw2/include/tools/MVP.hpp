#ifndef MVP_HPP
#define MVP_HPP

#define GLEW_STATIC      // GLEW
#include <GL/glew.h>     // GLEW
#include <GLFW/glfw3.h>  // glfw
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class MVP {

public:
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	void setProgram(GLuint Program) {this->Program = Program;}
	void Pass() {
		// Get the uniform locations
		modelLoc = glGetUniformLocation(Program, "model");
		viewLoc = glGetUniformLocation(Program, "view");
		projLoc = glGetUniformLocation(Program, "projection");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	}

private:
	GLuint Program{};
	GLint modelLoc{};
	GLint viewLoc{};
	GLint projLoc{};

};













#endif // !MVP_HPP