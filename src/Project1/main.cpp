#pragma comment (lib, "OpenGL32.lib")
#pragma comment(lib, "glu32")
#pragma comment(lib, "freeglut")

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/GLUT.h>

#define ILUT_USE_OPENGL		// This MUST be defined before calling the DevIL headers or we don't get OpenGL functionality
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600
#define WINDOW_TITLE	"CS552 Project 1"

GLUquadricObj *sphere = NULL;
GLuint texture;
glm::vec3 lastPoint;
enum Movement {ROTATE, ZOOM, NONE};
Movement movement = NONE;
static const float m_ROTSCALE = 90.0;
static const float m_ZOOMSCALE = 0.008;
static GLfloat objectXform[4][4] = {
	{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 }
};

// Output the message and pause the console if necessary.
void PrintError(const char* str);

void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

glm::vec3 trackBallMapping(glm::vec2 position);

void display() {
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);

	//Draw here
	glTranslatef(0.0f, 0.0f, -1.0f);

	static const GLfloat	potamb[3] = { 0.8f, 0.8f, 0.8f },
		potdfs[3] = { 0.9f, 0.9f, 0.9f },
		potspc[3] = { 1.0f, 1.0f, 1.0f };

	glEnable(GL_LIGHTING);

	glMaterialfv(GL_FRONT, GL_AMBIENT, potamb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, potdfs);
	glMaterialfv(GL_FRONT, GL_SPECULAR, potspc);
	glMateriali(GL_FRONT, GL_SHININESS, 50);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	gluSphere(sphere, 2, 20, 20);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	glutSwapBuffers();
}

void reshape(int w, int h) {

}

void onKeyPress(unsigned char key, int x, int y) {

}

void onMouseButton(int button, int state, int x, int y) {
	if(state != GLUT_DOWN) {
		movement = NONE;
	} else {
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
		{
			printf("Left button down at: %f, %f\n", (float)x, (float)y);
			movement = ROTATE;
			glm::vec2 position = glm::vec2(x, y);
			lastPoint = trackBallMapping(position);
			glMatrixMode(GL_MODELVIEW);
			break;
		}
		case GLUT_RIGHT_BUTTON:
		{
			printf("Right button down at: %f, %f\n", x, y);
			movement = ZOOM;
			lastPoint.x = x;
			lastPoint.y = y;
			glMatrixMode(GL_PROJECTION);
			break;
		}
		}
	}
}

void onMouseDrag(int x, int y) {
	glm::vec3 direction;
	float pixel_diff;
	float rot_angle, zoom_factor;
	glm::vec3 curPoint;
	glm::vec2 position = glm::vec2(x, y);

	switch (movement)
	{
	case ROTATE:
	{
		std::cout << "Round round round we go!" << std::endl;
		curPoint = trackBallMapping(position);  // Map the mouse position to a logical sphere location.
		direction = curPoint - lastPoint;
		float velocity = direction.length();
		if (velocity > 0.0001)
		{
			//
			// Rotate about the axis that is perpendicular to the great circle connecting the mouse movements.
			//
			glm::vec3 rotAxis = glm::cross(lastPoint, curPoint);
			rot_angle = velocity * m_ROTSCALE;
			//
			// We need to apply the rotation as the last transformation.
			//   1. Get the current matrix and save it.
			//   2. Set the matrix to the identity matrix (clear it).
			//   3. Apply the trackball rotation.
			//   4. Pre-multiply it by the saved matrix.
			//
			glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);
			glLoadIdentity();
			glRotatef(rot_angle, rotAxis.x, rotAxis.y, rotAxis.z);
			glMultMatrixf((GLfloat *)objectXform);
		}
		break;
	}
	case ZOOM:
	{
		std::cout << "Zoooooomies!" << std::endl;
		// Right-mouse button is being held down
		//
		// Zoom into or away from the scene based upon how far the mouse moved in the x-direction.
		//   This implementation does this by scaling the eye-space.
		//   This should be the first operation performed by the GL_PROJECTION matrix.
		//   1. Calculate the signed distance
		//       a. movement to the left is negative (zoom out).
		//       b. movement to the right is positive (zoom in).
		//   2. Calculate a scale factor for the scene s = 1 + a*dx
		//   3. Call glScalef to have the scale be the first transformation.
		// 
		pixel_diff = position.x - lastPoint.x;
		zoom_factor = 1.0 + pixel_diff * m_ZOOMSCALE;
		glScalef(zoom_factor, zoom_factor, zoom_factor);
		printf("Zoom factor: %f", zoom_factor);
		//
		// Set the current point, so the lastPoint will be saved properly below.
		//
		curPoint.x = (float)position.x;
		curPoint.y = (float)position.y;
		(float)curPoint.z = 0;
		break;
	}
	}
	lastPoint = curPoint;
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(WINDOW_TITLE);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKeyPress);
	glutMouseFunc(onMouseButton);
	glutMotionFunc(onMouseDrag);

	// Enable glew experimental, this enables some more OpenGL extensions.
	if (glewInit() != GLEW_OK)
	{
		PrintError("Failed to initialize GLEW");
		return EXIT_FAILURE;
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	// Initialise DevIL
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	GLfloat aspect = (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT;
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);

	sphere = gluNewQuadric();
	gluQuadricTexture(sphere, GL_TRUE);

	ILstring textureFilename = "BasketballColor.jpg";
	texture = ilutGLLoadImage(textureFilename);
	std::cout << "Image width         : " << ilGetInteger(IL_IMAGE_WIDTH) << std::endl;
	std::cout << "Image height        : " << ilGetInteger(IL_IMAGE_HEIGHT) << std::endl;
	std::cout << "Image bits per pixel: " << ilGetInteger(IL_IMAGE_BITS_PER_PIXEL) << std::endl;

	static const GLfloat lightPos[4] = { 2.0f, 2.0f, 2.0f, 0.0f };
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glutMainLoop();

	return EXIT_SUCCESS;
}

void PrintError(const char* str)
{
	std::cout << str << '\n';

#ifdef _DEBUG
	std::cin.get();
#endif
}

void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	GLdouble xmin, xmax, ymin, ymax;

	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;

	glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

glm::vec3 trackBallMapping(glm::vec2 position)
{
	glm::vec3 v;
	float d;
	float width = (float)glutGet(GLUT_WINDOW_WIDTH);
	float height = (float) glutGet(GLUT_WINDOW_HEIGHT);
	v.x = (2.0 * position.x - width) / width;
	v.y = (height - 2.0 * position.y) / height;
	v.z = 0.0;
	d = v.length();
	d = (d<1.0) ? d : 1.0;
	v.z = sqrtf(1.001 - d * d);
	return glm::normalize(v);
}