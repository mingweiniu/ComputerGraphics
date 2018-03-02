/*
* Course     : Computer Graphics
* Homework   : 01
* Department : 多工一
* ID         : 0556641
* student    : 牛明韋
* env        : VS14
*/

//README:                                        
//    press keyboard "Enter" to see PPAP         
//    press keyboard "ESC" to reset              
//    cube will change alpha gradually           

#ifndef MAIN_CPP
#define MAIN_CPP
#include <cstdlib>
#include <iostream>
#include <cmath>

#define HW1_and_example //due to the file in this folder
#if defined(HW1_and_example)
#include "../GL/glut.h"
#else
#include <GL/glut.h>
#endif

template<class T>
class handle_gl_obj {
public:
	T *handle{};
	handle_gl_obj() {
		handle = gluNewQuadric();
	};
	~handle_gl_obj() {
		gluDeleteQuadric(handle);
		handle = nullptr;
	};
};

struct {
	int state = 0; // show PPAP if 1
	float counter{}; // for rotate and alpha
	float ppap{}; // for ppap
}animation;


class pen {
public:
	pen() {
		init_pen();
	};
	~pen() {};
private:
	handle_gl_obj<GLUquadricObj> pen_body{};
	handle_gl_obj<GLUquadricObj> pen_head{};
	void init_pen();
};

void pen::init_pen() {
	glPushMatrix();
	glTranslatef(-1.5f, 0.0f, 0.0f);
	if (animation.state == 0) {
		glRotatef(-animation.counter * 100, 1.0f, 0.0f, 0.0f);
		glRotatef(-45.0f, 0.0f, 1.0f, 1.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	}
	if (animation.state == 1) {
		glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, animation.ppap);
	}
	gluCylinder(pen_head.handle, 0.1f, 0.1f, 2.0f, 16, 16);
	glTranslatef(0.0f, 0.0f, -0.5f);
	gluCylinder(pen_body.handle, 0.0f, 0.1f, 0.5f, 16, 16);
	glPopMatrix();
}

class apple {
public:
	apple() {
		init_apple();
	};
	~apple() {};
private:
	handle_gl_obj<GLUquadricObj> stem{};
	handle_gl_obj<GLUquadricObj> leaf{};
	handle_gl_obj<GLUquadricObj> body{};
	void init_apple();
};

void apple::init_apple() {
	glPushMatrix();
	glTranslatef(1.5f, 0.0f, -0.0f);
	if (animation.state == 0) {
		glRotatef(-animation.counter * 200, 0.0f, 0.0f, 1.0f);
	}
	if (animation.state == 1) {
		glTranslatef(animation.ppap / 1.7, 0.0f, 0.0f);
	}
	gluSphere(body.handle, 0.6f, 32, 32);
	glTranslatef(0.0f, 0.0f, -1.0f);
	gluCylinder(stem.handle, 0.05f, 0.0f, 1.0f, 16, 16);
	glTranslatef(-0.5f, 0.0f, 0.2f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(leaf.handle, 0.05f, 0.0f, 0.5f, 16, 16);
	glPopMatrix();
}

class cube
{
public:
	cube() {
		init_cube();
	};
	~cube() {};
private:
	float cube_size = 0.9;
	float cube_alpha{};
	void init_cube();
	//counterclock is the normal direction
	void right_face(); // Right  face (x = 1.0f)
	void left_face();  // Left   face (x = -1.0f)
	void front_face(); // Front  face (y = 1.0f)
	void back_face();  // Back   face (y = -1.0f)
	void top_face();   // Top    face (z = 1.0f)
	void down_face();  // Bottom face (z = -1.0f)
};

void cube::init_cube() {
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glPushMatrix();
	glTranslatef(0.0f, -2.0f, -3.0f);
	cube_alpha = pow(cos(animation.counter ), 2);
	glRotatef(animation.counter * 50, 1.0f, 0.0f, 0.0f);
	glRotatef(animation.counter * 150, 0.0f, 1.0f, 0.0f);
	glRotatef(animation.counter * 100, 0.0f, 0.0f, 1.0f);

	right_face();
	left_face();
	top_face();
	down_face();
	front_face();
	back_face();

	//return
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

}
void cube::right_face() {
	glBegin(GL_QUADS);
	glColor4f(1.0f, 0.0f, 0.0f, cube_alpha);    // Red
	glVertex3f(cube_size, cube_size, -cube_size);
	glVertex3f(cube_size, -cube_size, -cube_size);
	glVertex3f(cube_size, -cube_size, cube_size);
	glVertex3f(cube_size, cube_size, cube_size);
	glEnd();
}
void cube::left_face() {
	glBegin(GL_QUADS);
	glColor4f(0.0f, 0.0f, 1.0f, cube_alpha);    // Blue
	glVertex3f(-cube_size, -cube_size, -cube_size);
	glVertex3f(-cube_size, cube_size, -cube_size);
	glVertex3f(-cube_size, cube_size, cube_size);
	glVertex3f(-cube_size, -cube_size, cube_size);
	glEnd();
}
void cube::front_face() {
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, cube_alpha);     // White
	glVertex3f(-cube_size, cube_size, -cube_size);
	glVertex3f(cube_size, cube_size, -cube_size);
	glVertex3f(cube_size, cube_size, cube_size);
	glVertex3f(-cube_size, cube_size, cube_size);
	glEnd();
}
void cube::back_face() {
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 0.0f, cube_alpha);    // Yellow
	glVertex3f(-cube_size, -cube_size, cube_size);
	glVertex3f(cube_size, -cube_size, cube_size);
	glVertex3f(cube_size, -cube_size, -cube_size);
	glVertex3f(-cube_size, -cube_size, -cube_size);
	glEnd();
}
void cube::top_face() {
	glBegin(GL_QUADS);
	glColor4f(0.0f, 1.0f, 0.0f, cube_alpha);     // Green
	glVertex3f(cube_size, cube_size, cube_size);
	glVertex3f(cube_size, -cube_size, cube_size);
	glVertex3f(-cube_size, -cube_size, cube_size);
	glVertex3f(-cube_size, cube_size, cube_size);
	glEnd();
}
void cube::down_face() {
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 0.0f, cube_alpha);  // Orange
	glVertex3f(-cube_size, -cube_size, -cube_size);
	glVertex3f(cube_size, -cube_size, -cube_size);
	glVertex3f(cube_size, cube_size, -cube_size);
	glVertex3f(-cube_size, cube_size, -cube_size);
	glEnd();
}

void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void idle(void);

int main(int argc, char **argv) {
	std::cout <<
		"README:                         \n \
		press keyboard Enter to see PPAP \n \
		press keyboard ESC to reset      \n \
		cube will change alpha gradually \n \
		";
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("OpenGL Assignment 1 : 0556641");
	glutReshapeWindow(512, 512);

	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}

void init(void) {
	GLfloat position[] = { 1.0, 1.0, 5.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);//開啟混成，不然無法使用半透明
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);//混成的規則
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		0.0, 6.0, 4.0, /* Eye pos XYZ */
		0.0, 0.0, 0.0, /* Target pos XYZ */
		0.0, 1.0, 0.0  /* Up vector */
	);
	// do something awesome here : 
	apple apple{};
	pen pen{};
	cube cube{};
	animation.counter += 0.001f;
	if (animation.state == 1) {
		if (animation.ppap > -2.5) {
			animation.ppap -= 0.002f;
		}
	}
	glutSwapBuffers();
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 100.0);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 13: {//enter
		animation.state = (animation.state + 1) % 2;
		animation.ppap = {};
		break;
	}
	case 27: {//ESC
		animation = {};
		break;
	}
	default: {
		break;
	}

	}
}

void idle(void) {
	glutPostRedisplay();
}
#endif // !MAIN_CPP
