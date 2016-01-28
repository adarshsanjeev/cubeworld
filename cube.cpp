#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "cube.h"

using namespace std;

class Camera
{
public:
  float eye_x, eye_y;
  float zoom, pan;
  Camera () {
	eye_x = 0;
	eye_y = 0;
	zoom = 4.0;
	pan = 0.0;
  }
}Eye;

// class Character {
// 	public:
// 		float x, y;
// 		float radius;
// 		glm::vec2 Vel;
// 		float health;
// 		float fric_const;
// 		float elast_const;
// 		float rest_const;
// 		float air_const;
// 		bool alive;
// 		VAO *sprite;
// };

class Wall {
public:
	float x, y;
	float size_x, size_y;
	VAO *sprite;
} Floor;

void gravity() {
}

/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Function is called first on GLFW_PRESS.

	if (action == GLFW_RELEASE) {
		}
	else if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			quit(window);
			break;
		}
	}
	else if (action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_UP:
			Eye.eye_y += 1;
			break;
		case GLFW_KEY_DOWN:
			Eye.eye_y -= 1;
			break;
		case GLFW_KEY_LEFT:
			Eye.eye_x -=1;
			break;
		case GLFW_KEY_RIGHT:
			Eye.eye_x += 1;
			break;
		}
	}
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
	case 'Q':
	case 'q':
		quit(window);
		break;
	case 'i':
		Eye.zoom += 0.5;
		Eye.zoom = min(6.5f, Eye.zoom);
		break;
	case 'k':
		Eye.zoom -= 0.5;
		Eye.zoom = max(0.5f, Eye.zoom);
		break;
	case 'j':
		Eye.pan+=0.5;
		break;
	case 'l':
		Eye.pan-=0.5;
		break;
	}
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_RELEASE)
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_RELEASE) {
		}
		break;
	default:
		break;
	}
}

/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
	int fbwidth=width, fbheight=height;
	/* With Retina display on Mac OS X, GLFW's FramebufferSize
	   is different from WindowSize */
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
	// Perspective projection for 3D views
	// Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

	Eye.pan = Eye.zoom;

	// Ortho projection for 2D views
	Matrices.projection = glm::ortho(Eye.zoom*-1.0f - Eye.pan, Eye.zoom + Eye.pan, Eye.zoom*-1.0f, Eye.zoom, 0.1f, 500.0f);
	// Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}

void createFloor ()
{
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

	static const GLfloat g_color_buffer_data[] = {
		0.0f, 0.0f, 0.0f, // triangle 1 : begin
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, // triangle 1 : end
		1.0f, 1.0f, 1.0f, // triangle 2 : begin
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, // triangle 2 : end
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,1.0f,
		1.0f,1.0f, 1.0f,
		1.0f,1.0f, 1.0f,
		1.0f,1.0f,1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f,1.0f, 1.0f,
		1.0f,0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,1.0f,1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f,1.0f,1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,1.0f, 1.0f,
		1.0f, 1.0f,1.0f,
		1.0f, 1.0f,0.0f,
		1.0f, 1.0f,1.0f,
		1.0f,1.0f, 1.0f,
		1.0f, 1.0f,1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,1.0f, 1.0f
	};

	Floor.sprite = create3DObject(GL_TRIANGLES, 45, g_vertex_buffer_data, g_color_buffer_data, GL_FILL);
}

float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float triangle_rotation = 0;

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw ()
{
	// clear the color and depth in the frame buffer
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use the loaded shader program
	// Don't change unless you know what you are doing
	glUseProgram (programID);

	// Eye - Location of camera. Don't change unless you are sure!!
	glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	// Target - Where is the camera looking at.  Don't change unless you are sure!!
	glm::vec3 target (0, 0, 0);
	// Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
	glm::vec3 up (0, 1, 0);

	// Compute Eye matrix (view)
	// Matrices.view = glm::lookAt( eye, target, up ); // Rotating Eye for 3D
	//  Don't change unless you are sure!!
	Matrices.view = glm::lookAt(glm::vec3(Eye.eye_x, Eye.eye_y, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
	//  Don't change unless you are sure!!
	glm::mat4 VP = Matrices.projection * Matrices.view;

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	//  Don't change unless you are sure!!
	glm::mat4 MVP;	// MVP = Projection * View * Model

	// Load identity to model matrix
	Matrices.model = glm::mat4(1.0f);

	/* Render your scene */

	glm::mat4 translateTriangle = glm::translate (glm::vec3(-2.0f, 0.0f, 0.0f)); // glTranslatef
	glm::mat4 rotateTriangle = glm::rotate((float)(triangle_rotation*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
	glm::mat4 triangleTransform = translateTriangle * rotateTriangle;
	Matrices.model *= triangleTransform;
	MVP = VP * Matrices.model; // MVP = p * V * M

	//  Don't change unless you are sure!!
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// draw3DObject draws the VAO given to it using current MVP matrix
	// draw3DObject(triangle);

	// Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
	// glPopMatrix ();
	Matrices.model = glm::mat4(1.0f);

	glm::mat4 translateRectangle = glm::translate (glm::vec3(2, 0, 0));        // glTranslatef
	glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
	Matrices.model *= (translateRectangle * rotateRectangle);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// draw3DObject draws the VAO given to it using current MVP matrix
	// draw3DObject(rectangle);

	//####################################################################################################

	MVP = VP * glm::translate (glm::vec3(Floor.x, Floor.y, 0));
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(Floor.sprite);

	// Matrices.model = glm::mat4(1.0f);
	// glm::mat4 translateBarrel = glm::translate (glm::vec3(Cannon.x, Cannon.y, 0));
	// glm::mat4 rotateBarrel = glm::rotate((float)(Cannon.angle*M_PI/180.0f), glm::vec3(0,0,1));
	// Matrices.model *= translateBarrel * rotateBarrel;
	// MVP = VP * Matrices.model;
	// glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// // draw3DObject draws the VAO given to it using current MVP matrix
	// draw3DObject(Cannon.barrel);
	// MVP = VP * glm::translate (glm::vec3(PowerBar.x, PowerBar.y, 0)) * glm::scale(glm::vec3(2*Cannon.power, 1.0f, 1.0f));
	// glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// // draw3DObject draws the VAO given to it using current MVP matrix
	// draw3DObject(PowerBar.sprite);

	//####################################################################################################

	// Increment angles
	float increments = 1;
}

void scrollFunc(GLFWwindow *window, double xpos, double ypos)
{
	if(ypos == 1) {
		Eye.zoom -= 0.5;
		Eye.zoom = max(0.5f, Eye.zoom);
	}
	else {
		Eye.zoom += 0.5;
		Eye.zoom = min(6.5f, Eye.zoom);
	}
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
	GLFWwindow* window; // window desciptor/handle

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Anger Bird", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval( 1 );

	/* --- register callbacks with GLFW --- */

	/* Register function to handle window resizes */
	/* With Retina display on Mac OS X GLFW's FramebufferSize
	   is different from WindowSize */
	glfwSetFramebufferSizeCallback(window, reshapeWindow);
	glfwSetWindowSizeCallback(window, reshapeWindow);

	/* Register function to handle window close */
	glfwSetWindowCloseCallback(window, quit);

	/* Register function to handle keyboard input */
	glfwSetKeyCallback(window, keyboard);      // general keyboard input
	glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

	/* Register function to handle mouse click */
	glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
	glfwSetScrollCallback(window, scrollFunc);

	return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
	/* Objects should be created before any other gl function and shaders */
	// Create the models

    // Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
	
	reshapeWindow (window, width, height);

	// Background color of the scene
	glClearColor (0.5f, 0.5f, 0.9f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	createFloor();

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}

int main (int argc, char** argv)
{
	int width = 1400;
	int height = 800;

	GLFWwindow* window = initGLFW(width, height);

	initGL (window, width, height);
	double last_update_time = glfwGetTime(), current_time;

	/* Draw in loop */
	while (!glfwWindowShouldClose(window)) {
		reshapeWindow (window, width, height);

		// OpenGL Draw commands
		gravity();
		draw();

		// Swap Frame Buffer in double buffering
		glfwSwapBuffers(window);

		// Poll for Keyboard and mouse events
		glfwPollEvents();

		glfwSetCursorPosCallback(window, cursor_position_callback);

		// The function signature for cursor position callbacks. More...
		// Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
		current_time = glfwGetTime(); // Time in seconds
		if ((current_time - last_update_time) >= 0.1) {

			last_update_time = current_time;
		}
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
