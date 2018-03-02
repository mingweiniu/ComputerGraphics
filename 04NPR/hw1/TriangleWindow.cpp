#include "TriangleWindow.hpp"
#include "TriangleWindow.hpp"
#include "TriangleWindow.hpp"
#include <cmath>
#include <cstddef>



TriangleWindow::TriangleWindow()
	: m_program(nullptr)
	, m_frame(0)
	, mouse_click(false)
	, mouse_pressing(false)
{
}

void TriangleWindow::initialize()
{
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
	
	m_program = new QOpenGLShaderProgram(this);
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_program->link();
	m_matrixUniform = m_program->uniformLocation("matrix");

	glGenBuffers(1, &VBO);
	//glEnable(GL_DEPTH_TEST);
}

void TriangleWindow::render()
{
	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);
	glClear(GL_COLOR_BUFFER_BIT);

	m_program->bind();

	QMatrix4x4 matrix;
	matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	m_program->setUniformValue(m_matrixUniform, matrix);
	
	if (mouse_click) {
		float center_x = static_cast<float>(mouse_x / (geometry().width() / 2) - 1);
		float center_y = static_cast<float>(-mouse_y / (geometry().height() / 2) + 1);

		const double PI = 3.14159265358979323846;
		int edges = static_cast<int>(Radius * 500 + 10);

		for (int i = 0; i <= edges;++i) {

			vertices.push_back(center_x);
			vertices.push_back(center_y);
			vertices.push_back(0.0f);
			vertices.push_back(color.x);
			vertices.push_back(color.y);
			vertices.push_back(color.z);

			auto first_x = center_x + Radius * A * std::cos(i * 2 * PI / edges);
			auto first_y = center_y + Radius * B * std::sin(i * 2 * PI / edges);
			vertices.push_back(first_x);
			vertices.push_back(first_y);
			vertices.push_back(0.0f);
			vertices.push_back(color.x);
			vertices.push_back(color.y);
			vertices.push_back(color.z);

			auto second_x = first_x - B * std::sin(i * PI * 2 / edges) / 70;
			auto second_y = first_y + A * std::cos(i * PI * 2 / edges) / 70;
			vertices.push_back(second_x);
			vertices.push_back(second_y);
			vertices.push_back(0.0f);
			vertices.push_back(color.x);
			vertices.push_back(color.y);
			vertices.push_back(color.z);

		}
		//std::cout << vertices.size() << '\n';
		mouse_click = false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), (GLvoid*)offsetof(DrawVertex, vertex));
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DrawVertex), (GLvoid*)offsetof(DrawVertex, color));
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(vertices.size() / (sizeof(DrawVertex) / sizeof(GLfloat))));
	
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	m_program->release();

}

void TriangleWindow::mousePressEvent(QMouseEvent* event)
{
	mouse_x = event->pos().x();
	mouse_y = event->pos().y();
	mouse_click = true;
	mouse_pressing = true;
}

void TriangleWindow::mouseReleaseEvent(QMouseEvent* event)
{
	mouse_pressing = false;
}

void TriangleWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (mouse_pressing) {
		mouse_x = event->pos().x();
		mouse_y = event->pos().y();
		mouse_click = true;
	}
}

void TriangleWindow::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {

	case Qt::Key_R:
		color = { 1.0f, 0.0f, 0.0f };
		break;
	case Qt::Key_G:
		color = { 0.0f, 1.0f, 0.0f };
		break;
	case Qt::Key_K:
		color = { 0.0f, 0.0f, 1.0f };
		break;
	case Qt::Key_B:
		color = { 0.0f, 0.0f, 0.0f };
		break;
	case Qt::Key_W:
		color = { 1.0f, 1.0f, 1.0f };
		break;
	case Qt::Key_Up:
		Radius += 0.01f;
		break;
	case Qt::Key_Down:
		if (Radius > 0.01f) {
			Radius -= 0.01f;
		}
		break;
	case Qt::Key_A:
		B = 1.0f;
		A = 1.0f;
		break;
	case Qt::Key_S:
		B = 3.0f;
		A = 1.0f;
		break;
	case Qt::Key_D:
		B = 1.0f;
		A = 3.0f;
		break;
	case Qt::Key_C:
		vertices.resize(0);
		break;
	default:
		break;
	}
}
