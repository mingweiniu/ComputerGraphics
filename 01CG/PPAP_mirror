#include <stdlib.h>
#include <math.h>
#include "../GL/glut.h"

void init(void);//no need to modify this function
void display(void);
void reshape(int width, int height);//no need to modify this function
void keyboard(unsigned char key, int x, int y);//you can modify anything if you need
void idle(void);//no need to modify this function

void draw_PPAP(bool in_mirror);//in_mirror is TRUE if you draw mirror scene
void drawpen(float l, float r);//no need to modify this function
void drawapple(float sz);//no need to modify this function
void drawbox(float xrot, float yrot, bool in_mirror);//no need to modify this function
void draw_mirror_only();//you can modify anything if you need

void draw_mirror(int i, float alpha);
void reflection();

float delta = 0.0;
float alpha = 2.0;
float box_xrot = 0.0f;
float box_yrot = 0.0f;
bool PPAP = false;
float camera_x = 0.0f;
float camera_z = 8.0f;
float camera_y{};
float camera_rot = 0.0f;
double mirror_pos[6][3] = {
	{ 0.0,3.0,0.0 },//up
	{ 0.0,-3.0,0.0 },//bottom
	{ 3.0,0.0,0.0 },//right
	{ 1.5,0.0,-3.5 },//right-front
	{ -1.5,0.0,-3.5 },//left-front
	{ -3.0,0.0,0.0 },//left
					 //{ 0.0,0.0,5.0 },//back
};
double mirror_rotate_axis[6][3] = {
	{ 1.0,0.0,0.0 },//up
	{ 1.0,0.0,0.0 },//bottom
	{ 0.0,1.0,0.0 },//right
	{ 0.0,1.0,0.0 },//right-front
	{ 0.0,1.0,0.0 },//left-front
	{ 0.0,1.0,0.0 },//left
					//{ 0.0,1.0,0.0 },//back
};
float mirror_color[6][4] = {
	{ 1.0,0.0,0.0,0.4 },//up
	{ 0.0,1.0,0.0,0.4 },//bottom
	{ 0.0,0.0,1.0,0.4 },//right
	{ 1.0,1.0,0.0,0.4 },//right-front
	{ 1.0,0.0,1.0,0.4 },//left-front
	{ 0.0,1.0,1.0,0.4 },//left
};
double mirror_rotate_angle[6] = { 90.0,-90.0,90.0,45.0,-45.0,-90.0 };
double mirror_size_x[6] = { 5.0,5.0,5.0,2.121,2.121,5.0 };
double mirror_size_y[6] = { 5.0,5.0,5.0,3.0,3.0,5.0 };
float MatAmb[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Material - Ambient Values
float MatDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// PPAP Material - Diffuse Values
float MatSpc[] = { 0.0f, 0.0f, 0.0f, 1.0f };			// Material - Specular Values
float MatShn[] = { 0.1f };								// Material - Shininess


void reflection() {
//#define NOT_SHOW_UP_AND_DOWN 
//#define NOT_SHOW_RIGHT_AND_LEFT
#define USE_STENCIL

	for (int i = 0; i < 6; i++) {

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_TRUE);
		draw_mirror_only();
		glDepthMask(GL_FALSE);

		glDepthFunc(GL_EQUAL);
#if defined (USE_STENCIL) 
		glEnable(GL_STENCIL_TEST);
		// Draw floor
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF); // Write to stencil buffer
		glDepthMask(GL_FALSE); // Don't write to depth buffer
		glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)
#endif

		draw_mirror(i, 0.0);

#if defined (USE_STENCIL) 
		glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
		glStencilMask(0x00); // Don't write anything to stencil buffer
		glDepthMask(GL_TRUE); // Write to depth buffer
#endif
		glClear(GL_DEPTH_BUFFER_BIT);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthFunc(GL_LESS);
		glPushMatrix();
#if !defined (NOT_SHOW_UP_AND_DOWN) 
		if (i < 2) {
			glTranslated(mirror_pos[i][0], mirror_pos[i][1], mirror_pos[i][2]);
			glTranslated(mirror_pos[i][0], mirror_pos[i][1], mirror_pos[i][2]);
			glScalef(-1, 1, 1);
			glRotated(180, 1, 0, 0);
			glRotated(180, 0, 1, 0);
			draw_PPAP(true);
		}
#endif
#if !defined (NOT_SHOW_RIGHT_AND_LEFT) 
		if ((i == 2) || (i == 5)) {
			glTranslated(mirror_pos[i][0], mirror_pos[i][1], mirror_pos[i][2]);
			glTranslated(mirror_pos[i][0], mirror_pos[i][1], mirror_pos[i][2]);
			glScalef(1, -1, 1);
			glRotated(180, 1, 0, 0);
			glRotated(mirror_rotate_angle[i], mirror_rotate_axis[i][0], mirror_rotate_axis[i][1], mirror_rotate_axis[i][2]);
			glRotated(mirror_rotate_angle[i], mirror_rotate_axis[i][0], mirror_rotate_axis[i][1], mirror_rotate_axis[i][2]);
			draw_PPAP(true);
		}
#endif

#if !defined (NOT_SHOW_FRONT_RIGHT) 
		if (i == 3) {
			glTranslated(-2.3, 0, -1.7);
			//glTranslated(mirror_pos[i][0], mirror_pos[i][1], mirror_pos[i][2]);
			glScalef(1, -1, 1);
			glRotated(180, 1, 0, 0);
			glRotated(-90, 0, 1, 0);
			//glRotated(mirror_rotate_angle[i], mirror_rotate_axis[i][0], mirror_rotate_axis[i][1], mirror_rotate_axis[i][2]);
			//glRotated(mirror_rotate_angle[i], mirror_rotate_axis[i][0], mirror_rotate_axis[i][1], mirror_rotate_axis[i][2]);
			draw_PPAP(true);

		}
#endif



#if !defined (NOT_SHOW_FRONT_LEFT) 
		if (i == 4) {
			glTranslated(2.1, 0, -1.9);
			glScalef(-1, 1, 1);
			glRotated(-90, 0, 1, 0);
			draw_PPAP(true);
		}
#endif

		glPopMatrix();

#if defined (USE_STENCIL) 
		glDisable(GL_STENCIL_TEST);
#endif
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera_x, camera_y, camera_z, /* Eye pos XYZ */
		camera_x + 1.0f*sin(camera_rot), camera_y, camera_z - 1.0f*cos(camera_rot), /* Target pos XYZ */
		0.0, 1.0, 0.0); /* Up vector */
	box_xrot += 0.5f;
	box_yrot += 0.5f;
	if (PPAP)
	{
		alpha = alpha > 0 ? alpha - 0.1 : 0;
		delta = 90.0;
	}
	else
	{
		alpha = 2.0;
		delta += 1.0;
	}
	//code below this line
	
	//glDepthFunc(GL_EQUAL);
	//draw_mirror_only();
	glEnable(GL_DEPTH_TEST);
	reflection();
	//glDepthFunc(GL_LESS);
	//glDepthMask(GL_TRUE);
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//glDepthFunc(GL_EQUAL);
	

	//glDepthFunc(GL_EQUAL);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);
	draw_mirror_only();
	glDepthMask(GL_FALSE);


	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthFunc(GL_EQUAL);
	draw_mirror_only();


	glDepthFunc(GL_LESS);
	draw_PPAP(false);
	
	//draw_mirror_only();
	glutSwapBuffers();
}

//you can modify anything if you need
void draw_mirror_only()
{

	glDisable(GL_LIGHTING);
	for (int i = 0; i < 6; i++)
	{
		glPushMatrix();
		glTranslated(mirror_pos[i][0], mirror_pos[i][1], mirror_pos[i][2]);
		glRotated(mirror_rotate_angle[i], mirror_rotate_axis[i][0], mirror_rotate_axis[i][1], mirror_rotate_axis[i][2]);
		glColor4d(mirror_color[i][0], mirror_color[i][1], mirror_color[i][2], 0.4);

		glBegin(GL_QUADS);
		glVertex3d(mirror_size_x[i], mirror_size_y[i], 0.0);
		glVertex3d(mirror_size_x[i], -mirror_size_y[i], 0.0);
		glVertex3d(-mirror_size_x[i], -mirror_size_y[i], 0.0);
		glVertex3d(-mirror_size_x[i], mirror_size_y[i], 0.0);
		glEnd();
		glPopMatrix();
	}
	glEnable(GL_LIGHTING);

}
//you can modify anything if you need
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
	{
		camera_z -= 0.5*cos(camera_rot);
		camera_x += 0.5*sin(camera_rot);
		break;
	}
	case 's':
	{
		camera_z += 0.5*cos(camera_rot);
		camera_x -= 0.5*sin(camera_rot);
		break;
	}
	case 'a':
	{
		camera_z -= 0.5*sin(camera_rot);
		camera_x -= 0.5*cos(camera_rot);
		break;
	}
	case 'd':
	{
		camera_z += 0.5*sin(camera_rot);
		camera_x += 0.5*cos(camera_rot);
		break;
	}
	case 'q':
	{
		camera_rot -= 0.03f;
		break;
	}
	case 'e':
	{
		camera_rot += 0.03f;
		break;
	}
	case 'z':
	{
		camera_y += 0.03f;
		break;
	}
	case 'c':
	{
		camera_y -= 0.03f;
		break;
	}
	case 13:
	{   //enter
		PPAP = !PPAP;
		break;
	}
	case 27:
	{   //ESC
		exit(0);
		break;
	}
	default:
	{
		break;
	}
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//no need to modify this function
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutCreateWindow("OpenGL Assignment 2 - 0556641 Ming-Wei, Niu"); //SUDO
	glutReshapeWindow(512, 512);

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}
//no need to modify this function
void init(void) {

	GLfloat position[] = { 1.0, 1.0, 5.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MatAmb);			// Set Material Ambience
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MatDif);			// Set Material Diffuse
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpc);		// Set Material Specular
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, MatShn);		// Set Material Shininess
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Set The Blending Function For Translucency

}
//no need to modify this function
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)width / (GLfloat)height, 0.1, 100.0);
	//glOrtho(-5.0,5.0,-5.0,5.0,0.0,200.0);
}
//no need to modify this function
void idle(void)
{
	glutPostRedisplay();
}
//in_mirror is TRUE if you draw mirror scene
void draw_PPAP(bool in_mirror)
{
	drawbox(box_xrot, box_yrot, in_mirror);
	glDisable(GL_BLEND);
	glPushMatrix();
	glTranslatef(-alpha, 0.0, 0.0);
	if (PPAP)
	{
		glRotatef(-90, 0.0, 0.0, 1.0);
	}
	else
	{
		glRotatef(delta, 0.0, 1.0, 0.0);
		glRotatef(10, 0.0, 0.0, 1.0);
	}
	drawpen(2.0, 0.1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(alpha, 0.0, 0.0);
	glRotatef(delta, 0.0, 1.0, 0.0);
	drawapple(0.8);
	glPopMatrix();
	glEnable(GL_BLEND);
}
//no need to modify this function
void drawpen(float l, float r)
{
	glTranslatef(0.0, l * 0.5, 0.0);
	glPushMatrix();
	GLUquadricObj *quadratic = gluNewQuadric();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, r, r, l, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(r, l * 0.2, 32, 32);
	glPopMatrix();
}
//no need to modify this function
void drawapple(float sz)
{
	glPushMatrix();
	glutSolidSphere(sz, 32, 32);
	glTranslatef(0.0, sz, 0.0);

	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(sz * 0.05, sz * 0.4, 32, 32);
	glPopMatrix();

	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0, 0.0, -sz * 0.4);
	glutSolidCone(sz * 0.05, sz * 0.4, 32, 32);
	glPopMatrix();
}
//no need to modify this function
void drawbox(float xrot, float yrot, bool in_mirror)
{

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	//glDisable(GL_DEPTH_TEST);
	if (!in_mirror) glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);



	glPushMatrix();
	glScaled(0.5, 0.5, 0.5);
	glTranslatef(0.0f, 3.0f, 0.0f);
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);

	glBegin(GL_QUADS);
	// Front Face
	glNormal3f(0.0f, 0.0f, 1.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.5);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	// Back Face
	glNormal3f(0.0f, 0.0f, -1.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 0.5);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	// Top Face
	glNormal3f(0.0f, 1.0f, 0.0f);
	glColor4f(0.0f, 0.0f, 1.0f, 0.5);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	// Bottom Face
	glNormal3f(0.0f, -1.0f, 0.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 0.5);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	// Right face
	glNormal3f(1.0f, 0.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.9);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	// Left Face
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.1);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	if (!in_mirror) glDisable(GL_BLEND);
}

void draw_mirror(int i,float alpha) {
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslated(mirror_pos[i][0], mirror_pos[i][1], mirror_pos[i][2]);
	glRotated(mirror_rotate_angle[i], mirror_rotate_axis[i][0], mirror_rotate_axis[i][1], mirror_rotate_axis[i][2]);
	glColor4d(mirror_color[i][0], mirror_color[i][1], mirror_color[i][2], alpha);

	glBegin(GL_QUADS);
	glVertex3d(mirror_size_x[i], mirror_size_y[i], 0.0);
	glVertex3d(mirror_size_x[i], -mirror_size_y[i], 0.0);
	glVertex3d(-mirror_size_x[i], -mirror_size_y[i], 0.0);
	glVertex3d(-mirror_size_x[i], mirror_size_y[i], 0.0);
	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
}
