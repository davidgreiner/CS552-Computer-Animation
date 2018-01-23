#pragma comment (lib, "OpenGL32.lib")
#pragma comment(lib, "glu32")

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include <SDL.h>
#include <SDL_opengl.h>

#define ILUT_USE_OPENGL		// This MUST be defined before calling the DevIL headers or we don't get OpenGL functionality
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600
#define WINDOW_TITLE	"CS552 Project 1"

GLuint texture;

// Output the message and pause the console if necessary.
void PrintError(const char* str);

void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

int main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_GLContext context;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != NULL)
	{
		PrintError(SDL_GetError());
		return EXIT_FAILURE;
	}

	// Set the desired OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// Create the window using SDL
	window = SDL_CreateWindow(WINDOW_TITLE,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

	if (window == NULL)
	{
		PrintError(SDL_GetError());
		return EXIT_FAILURE;
	}

	// Create the OpenGL context for the window using SDL
	context = SDL_GL_CreateContext(window);
	if (context == NULL)
	{
		PrintError(SDL_GetError());
		return EXIT_FAILURE;
	}

	// Enable glew experimental, this enables some more OpenGL extensions.
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		PrintError("Failed to initialize GLEW");
		return EXIT_FAILURE;
	}

	// Initialise DevIL
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	bool isRunning = true;

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

	GLUquadricObj *sphere = NULL;
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

	SDL_Event event;
	while (isRunning)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT: 
				isRunning = false;
				break;
			}
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);

		// Reset the current transform
		glLoadIdentity();

		//Draw here
		glTranslatef(0.0f, 1.0f, -16.0f);

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

		// Swap the OpenGL window buffers
		SDL_GL_SwapWindow(window);
	}

	// Release resources
	SDL_GL_DeleteContext(context);
	SDL_Quit();

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
