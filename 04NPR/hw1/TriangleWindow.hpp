#ifndef TRIANGLEWINDOW_HPP
#define TRIANGLEWINDOW_HPP

#include "OpenGLWindow.hpp"
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QMouseEvent>

class TriangleWindow : public OpenGLWindow
{
public:
	TriangleWindow();

	void initialize() override;
	void render() override;

private:
	GLuint m_posAttr;
	GLuint m_colAttr;
	GLuint m_matrixUniform;
	QOpenGLShaderProgram* m_program;
	int m_frame;

	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	float mouse_x, mouse_y;
	bool mouse_click, mouse_pressing;

	void keyPressEvent(QKeyEvent* event) override;
	float A = 1.0f;
	float B = 1.0f;

	float Radius = 0.03f;
	struct vec3 { float x, y, z; };
	vec3 color{1.0f, 1.0f, 1.0f};
	struct DrawVertex {
		vec3 vertex;
		vec3 color;
	};
	std::vector<GLfloat> vertices{};
	GLuint VBO{};
	GLuint VAO{};
	
};

#endif // !TRIANGLEWINDOW_HPP