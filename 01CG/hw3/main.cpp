#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /* offsetof */

#include "../GL/glew.h"
#include "../GL/glut.h"

#include "../shader_lib/shader.h"
#include "glm/glm.h"
extern "C"
{
	#include "glm_helper.h"
}

GLMmodel *XiaoPingGuo;GLfloat light_pos[] = { 10.0, 10.0, 0.0 };
struct view {
	float eyex = 0.0, eyey = 0.0, zshift = 0.0, xshift = 0.0, yshift = 0.0;
	float rotate = 0.0f;
}lookat;

GLuint shad_program;
void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void idle(void);

bool myShader = true;
enum class Shading { FLAT, Phong, Gouraud, };
Shading use_model = Shading::Phong;
const int DATA_PER_VERTEX = 8;
std::vector<GLfloat> vertices{};
void drawApple();
void MVP();

void drawApple() {
	//std::cout << "(eyex,eyey) is " << lookat.eyex << ", " << lookat.eyey << std::endl;
	GLuint VAO{};
	glGenVertexArrays(1, &VAO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	GLuint VBO{};
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, DATA_PER_VERTEX * sizeof(GLfloat), 0); // stride 0 for tightly packed
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, DATA_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, DATA_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(NULL); // Unbind VAO


	glUseProgram(shad_program);
	MVP();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, XiaoPingGuo->textures->id);
	glUniform1i(glGetUniformLocation(shad_program, "appleTex"), 0);


	GLint viewPosLoc = glGetUniformLocation(shad_program, "viewPos");
	glUniform3f(viewPosLoc, lookat.eyex + lookat.xshift, lookat.eyey + lookat.yshift, 3.0 - lookat.zshift);

	GLint light_posLoc = glGetUniformLocation(shad_program, "light_pos");
	glUniform3f(light_posLoc, 1, GL_FALSE, *light_pos);
	

	GLint FlatLoc = glGetUniformLocation(shad_program, "Flat");
	if (use_model == Shading::FLAT) {
		glUniform1i(FlatLoc, 1);
	}
	else {
		glUniform1i(FlatLoc, 0);
	}
	GLint phongLoc = glGetUniformLocation(shad_program, "Phong");
	if (use_model == Shading::Phong) {
		glUniform1i(phongLoc, 1);
	}
	else {
		glUniform1i(phongLoc, 0);
	}


	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(vertices.size() / DATA_PER_VERTEX));
	glBindVertexArray(NULL);

	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(NULL);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void MVP() {
	/*
	glm::mat4 model;
	glm::mat4 view = glm::lookAt(
		glm::vec3(eyex + xshift, eyey + yshift, 3.0 - zshift),
		glm::vec3(xshift, yshift, -zshift),
		glm::vec3(0.0, 1.0, 0.0)
	);
	glm::mat4 projection = glm::perspective(45.0, 1.0, 1e-2, 1e2);
	*/
	glm::mat4 Model;
	lookat.rotate += 0.002f;
	Model = glm::rotate(Model, lookat.rotate, glm::vec3(0.0f, 1.0f, 0.0f));
	float View[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, View);
	float Projection[16];
	glGetFloatv(GL_PROJECTION_MATRIX, Projection);

	GLint ModelLoc = glGetUniformLocation(shad_program, "Model");
	GLint ViewLoc = glGetUniformLocation(shad_program, "View");
	GLint ProjLoc = glGetUniformLocation(shad_program, "Projection");

	glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(Model));
	glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, View);
	glUniformMatrix4fv(ProjLoc, 1, GL_FALSE, Projection);
}

void reshape(int width, int height) { glViewport(0, 0, width, height); }
void idle(void) { glutPostRedisplay(); }
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27 : {lookat = {}; break;}
	case 'd': {lookat.eyex += 0.1; break;}
	case 'a': {lookat.eyex -= 0.1; break;}
	case 'w': {lookat.eyey += 0.1; break;}
	case 's': {lookat.eyey -= 0.1; break;}
	case 'f': {use_model = Shading::FLAT;std::cout << "Shading::FLAT" << std::endl; break;}
	//case 'g': {use_model = Shading::Gouraud;std::cout << "Shading::Gouraud" << std::endl; break;}
	case 'p': {use_model = Shading::Phong;std::cout << "Shading::Phong" << std::endl; break;}
	case 'y': {lookat.zshift += 0.1; break;}
	case 'h': {lookat.zshift -= 0.1; break;}
	case 'j': {lookat.xshift -= 0.1; break;}
	case 'l': {lookat.xshift += 0.1; break;}
	case 'i': {lookat.yshift += 0.1; break;}
	case 'k': {lookat.yshift -= 0.1; break;}
	default: {break;}
	}
}
void init(void) {
	glEnable(GL_DEPTH_TEST);
	XiaoPingGuo = glmReadOBJ("Model/apple.obj");
	glmUnitize(XiaoPingGuo);
	glmFacetNormals(XiaoPingGuo);
	glmVertexNormals(XiaoPingGuo, 90.0, GL_FALSE);
	print_model_info(XiaoPingGuo);

	/* Your initialization */
	if (myShader) {
		shad_program = createProgram(
			createShader("Shaders/apple.vert", "vertex"),
			createShader("Shaders/apple.frag", "fragment")
		);
		vertices.reserve(XiaoPingGuo->numtriangles * DATA_PER_VERTEX);
		for (int i = 0; i < XiaoPingGuo->numtriangles; ++i) {
			int indexV[3] = {
				XiaoPingGuo->triangles[i].vindices[0],
				XiaoPingGuo->triangles[i].vindices[1],
				XiaoPingGuo->triangles[i].vindices[2]
			};
			int indexN[3] = {
				XiaoPingGuo->triangles[i].nindices[0],
				XiaoPingGuo->triangles[i].nindices[1],
				XiaoPingGuo->triangles[i].nindices[2]
			};
			int indexT[3] = {
				XiaoPingGuo->triangles[i].tindices[0],
				XiaoPingGuo->triangles[i].tindices[1],
				XiaoPingGuo->triangles[i].tindices[2]
			};
			for (int j = 0; j < 3; ++j) {
				vertices.push_back(XiaoPingGuo->vertices[3 * indexV[j]]);
				vertices.push_back(XiaoPingGuo->vertices[3 * indexV[j] + 1]);
				vertices.push_back(XiaoPingGuo->vertices[3 * indexV[j] + 2]);

				vertices.push_back(XiaoPingGuo->normals[3 * indexN[j]]);
				vertices.push_back(XiaoPingGuo->normals[3 * indexN[j] + 1]);
				vertices.push_back(XiaoPingGuo->normals[3 * indexN[j] + 2]);

				vertices.push_back(XiaoPingGuo->texcoords[2 * indexT[j]]);
				vertices.push_back(XiaoPingGuo->texcoords[2 * indexT[j] + 1]);
			}
		}
	}
}
void display(void){
	glClearColor(0.1, 0.2, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1e-2, 1e2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		lookat.eyex + lookat.xshift, lookat.eyey + lookat.yshift, 3.0 - lookat.zshift,
		lookat.xshift, lookat.yshift, -lookat.zshift,
		0.0, 1.0, 0.0
	);
	/* Write your code here */
	if (myShader) { drawApple(); }
	else {
		glmDraw(XiaoPingGuo, GLM_TEXTURE); /* Make sure glmLoadOBJ work. If it works, remove this line. */
	}
	glutSwapBuffers();
}
int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glutCreateWindow("OpenGL HW3 - 0556641 Ming-Wei, Niu");
	glutReshapeWindow(512, 512);

	glewInit();init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	glmDelete(XiaoPingGuo);
	return 0;
}
