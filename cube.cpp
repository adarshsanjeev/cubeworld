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
#define FLOOR_LENGTH 20
#define	GRAV_CONST 0.009;

using namespace std;

enum camera_type {ADVENTURE, FOLLOW, TOWER, TOP, HELI};
enum block_type {STABLE, DEAD, CRUMBLING, HOVERING, STICKY};
enum compass {UP=0, DOWN=1, LEFT=2, RIGHT=3};
enum player_state {NORMAL, FALLING};
int GAME_DIFFICULTY = 1;

void createChar();
void resetGame();
void win_game();

GLfloat g_normal_buffer_data[36*3] = {

    // Left face
    -1.0f,0.0f,0.0f, //-ve xaxis
    -1.0f,0.0f,0.0f, //-ve xaxis
    -1.0f,0.0f,0.0f, //-ve xaxis

    -1.0f,0.0f,0.0f, //-ve xaxis
    -1.0f,0.0f,0.0f, //-ve xaxis
    -1.0f,0.0f,0.0f, //-ve xaxis


    // Back face
    0.0f,0.0f,-1.0f, // -ve zaxis
    0.0f,0.0f,-1.0f, // -ve zaxis
    0.0f,0.0f,-1.0f, // -ve zaxis

    0.0f,0.0f,-1.0f, // -ve zaxis
    0.0f,0.0f,-1.0f, // -ve zaxis
    0.0f,0.0f,-1.0f, // -ve zaxis

    // Bottom face
    0.0f,-1.0f,0.0f, // -ve yaxis
    0.0f,-1.0f,0.0f, // -ve yaxis
    0.0f,-1.0f,0.0f, // -ve yaxis

    0.0f,-1.0f,0.0f, // -ve yaxis
    0.0f,-1.0f,0.0f, // -ve yaxis
    0.0f,-1.0f,0.0f, // -ve yaxis

    // Front face
    0.0f,0.0f,1.0f, // +ve zaxis
    0.0f,0.0f,1.0f, // +ve zaxis
    0.0f,0.0f,1.0f, // +ve zaxis

    0.0f,0.0f,1.0f, // +ve zaxis
    0.0f,0.0f,1.0f, // +ve zaxis
    0.0f,0.0f,1.0f, // +ve zaxis

    // Right face
    1.0f,0.0f,0.0f, // +ve xaxis
    1.0f,0.0f,0.0f, // +ve xaxis
    1.0f,0.0f,0.0f, // +ve xaxis

    1.0f,0.0f,0.0f, // +ve xaxis
    1.0f,0.0f,0.0f, // +ve xaxis
    1.0f,0.0f,0.0f, // +ve xaxis


    // Topface
    0.0f,1.0f,0.0f, // +ve yaxis
    0.0f,1.0f,0.0f, // +ve yaxis
    0.0f,1.0f,0.0f, // +ve yaxis

    0.0f,1.0f,0.0f, // +ve yaxis
    0.0f,1.0f,0.0f, // +ve yaxis
    0.0f,1.0f,0.0f, // +ve yaxis
          	};

class mouseHold
{
public:
	float x, y;
	float angle;
	float held_x, held_y;
	bool MOUSE_HELD;
}Mouse;

class Camera
{
public:
	float x, y, z;
	int health;
	float LookAt_x, LookAt_y, LookAt_z;
	float zoom, pan;
	float angle, elev;
	camera_type camera_type;
	Camera () {
		camera_type = HELI;
		x = 0;
		y = 3;
		z = 3;
		angle = 90;
		elev = 10;
		LookAt_x = 0;
		LookAt_y = -2.5;
		LookAt_z = 0;
		zoom = 4.0;
		pan = 0.0;  }
}Eye;

class Character {
public:
	float x, y, z;
	float angle;
    glm::vec3 tilt;
	glm::vec3 vel;
	float size_x, size_y, size_z;
	float scale;
	float speed_mod;
	player_state state;
	VAO *sprite;

	void checkDie() {
		if (y<-10)
			resetGame();
	}
} Cube, Shadow;

class Wall {
public:
	float x, y, z;
	float size_x, size_y, size_z;
	block_type type;
	int direction;

	GLfloat g_vertex_buffer_data[36*3]= {
// Left face
    -1.0f,-0.6f,-1.0f, //vertex 1
    -1.0f,-0.6f, 1.0f, //vertex 2
    -1.0f, 1.0f, 1.0f, //vertex 3

    -1.0f,-0.6f,-1.0f, //vertex 1
    -1.0f, 1.0f, 1.0f, // vertex 3
    -1.0f, 1.0f,-1.0f, // vertex 4

    //Back face
    1.0f, 1.0f,-1.0f, //vertex 5
    -1.0f,-0.6f,-1.0f, // vertex 1
    -1.0f, 1.0f,-1.0f, // vertex 4

    1.0f, 1.0f,-1.0f, //vertex 5
    1.0f,-0.6f,-1.0f, // vertex 7
    -1.0f,-0.6f,-1.0f, //vertex 1

    //  Bottom face
    1.0f,-0.6f, 1.0f, // vertex 6
    -1.0f,-0.6f,-1.0f, // vertex 1
    1.0f,-0.6f,-1.0f, // vertex 7

    1.0f,-0.6f, 1.0f, //vertex 6
    -1.0f,-0.6f, 1.0f, //vertex 2
    -1.0f,-0.6f,-1.0f, //vertex 1

    //  Front face
    -1.0f, 1.0f, 1.0f, //vertex 3
    -1.0f,-0.6f, 1.0f, //vertex 2
    1.0f,-0.6f, 1.0f, //vertex 6

    1.0f, 1.0f, 1.0f,  //vertex 8
    -1.0f, 1.0f, 1.0f, //vertex 3
    1.0f,-0.6f, 1.0f, //vertex 6

    //  Right face
    1.0f, 1.0f, 1.0f, //vertex 8
    1.0f,-0.6f,-1.0f, //vertex 7
    1.0f, 1.0f,-1.0f, //vertex 5

    1.0f,-0.6f,-1.0f, //vertex 7
    1.0f, 1.0f, 1.0f, //vertex 8
    1.0f,-0.6f, 1.0f, //vertex 6

    //Topface
    1.0f, 1.0f, 1.0f, //vertex 8
    1.0f, 1.0f,-1.0f, //vertex 5
    -1.0f, 1.0f,-1.0f, //vertex 4

    1.0f, 1.0f, 1.0f, //vertex 8
    -1.0f, 1.0f,-1.0f, //vertex 4
    -1.0f, 1.0f, 1.0f, //vertex 3
	};

	GLfloat g_color_buffer_data[36*3] = {
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
		0.7f, 0.7f, 0.7f, // triangle 1 : begin
		0.7f, 0.7f, 0.7f,
		0.7f, 0.7f, 0.7f, // triangle 1 : end
	};

	VAO *sprite;
} Floor[FLOOR_LENGTH][FLOOR_LENGTH];

pair<int, int> getFloorSquare(float x, float z)
{
	z*=-1;
	if (x<-1.5 || x>FLOOR_LENGTH*2-0.5 || z<-1.5 || z>FLOOR_LENGTH*2-0.5)
		return make_pair(-1, -1);
	else
		return make_pair((int)(x+1)/2, (int)(z+1)/2);
}

Wall* getFloorPointer(float x, float z)
{
	z*=-1;
	if (x<-1.5 || x>FLOOR_LENGTH*2-0.5 || z<-1.5 || z>FLOOR_LENGTH*2-0.5)
		return NULL;
	else
		return &Floor[(int)(x+1)/2][(int)(z+1)/2];
}

void resetGame() {
	for(int i=0; i<FLOOR_LENGTH; i++)
		for(int j=0; j<FLOOR_LENGTH; j++)
			if (Floor[i][j].type == CRUMBLING) {
				Floor[i][j].y = -3;
		}
	createChar();
}

void winGame() {
	GAME_DIFFICULTY -= 1;
	GAME_DIFFICULTY = max(GAME_DIFFICULTY, 1);
	resetGame();
}

void VertColl(float x, float z) {
	pair<int, int> floor_pair = getFloorSquare(x, z);
	int floor_i = floor_pair.first;
	int floor_j = floor_pair.second;

	if (floor_i == -1 || floor_j == -1) {
	    if (Cube.y <= -0.5)
	  	  Cube.tilt[0] += 5;
	    return;
	}

	Wall* Block = &Floor[floor_i][floor_j];
	if ( (Cube.y-Cube.size_y/2) <= (Block->y+Block->size_y/2) &&
		 (Cube.y+Cube.size_y/2) >= (Block->y-Block->size_y/2) ) // Y collision
		if (Cube.x > Block->x-Block->size_x/2 && Cube.x<Block->x+Block->size_x/2) // X collision
			if (Cube.z > Block->z-Block->size_z/2 && Cube.z < Block->z+Block->size_z/2) // Z collision
				if (Block->type != DEAD) {
					Cube.state = NORMAL;
					if (Cube.vel[1] < -0.12)
						resetGame();
					Cube.vel[1] = Cube.vel[1]>0 ? Cube.vel[1]:0;
					Cube.y +=  (Block->y+Block->size_y/2) - (Cube.y-Cube.size_y/2);
					if (Block->type == CRUMBLING) {
						if (Block->y > -7) {
							Block->y -= 0.01;
							Cube.y -= 0.01;
						}
						else
							Block->y += 100.00;
					}
					if (Block->type == HOVERING && Block->direction == -1)
						Cube.y -= 0.01;
					if (Block->type == STICKY)
						Cube.speed_mod = 0.5;
					else
						Cube.speed_mod = 1;
			}

	if (Cube.y <= Block->y-0.5)
	  Cube.tilt[0] += 10;
}

bool moveShadow() {
	pair<int, int> floor_pair = getFloorSquare(Cube.x, Cube.z);
	int floor_i = floor_pair.first;
	int floor_j = floor_pair.second;

	if(floor_i == -1 || floor_j == -1)
		return false;

	Wall* Block = &Floor[floor_i][floor_j];

	if (Block->type == DEAD)
		return false;

	if (Cube.y < Block->y)
		return false;

	Shadow.x = Cube.x;
	Shadow.y = Block->y + Block->size_y/2+0.01;
	Shadow.z = Cube.z;
	Shadow.scale = 1/(0.6+Cube.y - Shadow.y);
	Shadow.scale = Shadow.scale<0.65? Shadow.scale:0.65;
	if (Cube.y < Shadow.y)
		return false;
	return true;
}

void gravity() {
	Cube.vel += glm::vec3(0.0f, -0.0009f, 0.0f);

	for(int i=0; i<FLOOR_LENGTH; i++)
		for(int j=0; j<FLOOR_LENGTH; j++)
			if (Floor[i][j].type == HOVERING) {
				if (Floor[i][j].y > 0 || Floor[i][j].y < -6)
					Floor[i][j].direction *= -1;
				Floor[i][j].y += 0.01 * Floor[i][j].direction;
		}

	// Vertical collision
	VertColl(Cube.x, Cube.z);

	Cube.x += Cube.vel[0];
	Cube.y += Cube.vel[1];
	Cube.z += Cube.vel[2];
	Cube.checkDie();
}

bool moveCube(float x, float z)
{
	pair<int, int> floor_pair = getFloorSquare(Cube.x, Cube.z);
	int floor_i = floor_pair.first;
	int floor_j = floor_pair.second;

	if (x>0) {
		if (floor_i == -1 || floor_i == FLOOR_LENGTH-1)
			return true;

		Wall* NextCube = &Floor[floor_i+1][floor_j];
		if (NextCube->type == DEAD)
			return true;

		if ( (Cube.y-Cube.size_y/2+0.1) <= (NextCube->y+NextCube->size_y/2) &&
			 (Cube.y+Cube.size_y/2) >= (NextCube->y-NextCube->size_y/2) ) { // Check no y overlap
			 // If y overlap then no x overlap allowed
			if ( (Cube.x+Cube.size_x/2+0.01) < (NextCube->x-NextCube->size_x/2) )
				return true;
		}
		else
			return true;
	}

	else if (x<0) {
		if (floor_i == -1 || floor_i == 0)
			return true;

		Wall* NextCube = &Floor[floor_i-1][floor_j];
		if (NextCube->type == DEAD)
			return true;

		if ( (Cube.y-Cube.size_y/2+0.1) <= (NextCube->y+NextCube->size_y/2) &&
			 (Cube.y+Cube.size_y/2) >= (NextCube->y-NextCube->size_y/2) ) { // Check no y overlap
			if ( (Cube.x-Cube.size_x/2-0.01) > (NextCube->x+NextCube->size_x/2) )
				return true;
		}
		else
			return true;
	}

	else if(z<0) {
		if(floor_j == -1 || floor_j == FLOOR_LENGTH-1)
			return true;
		Wall* NextCube = &Floor[floor_i][floor_j+1];
		if (NextCube->type == DEAD)
			return true;

			if ( (Cube.y-Cube.size_y/2+0.1) <= (NextCube->y+NextCube->size_y/2) &&
			 (Cube.y+Cube.size_y/2) >= (NextCube->y-NextCube->size_y/2) ) { // Check no y overlap
			if ( (Cube.z-Cube.size_z/2-0.01) > (NextCube->z+NextCube->size_z/2) )
				return true;
		}
		else
			return true;
	}

	else if(z>0) {
		if(floor_j == -1 || floor_j == 0)
			return true;

		Wall* NextCube = &Floor[floor_i][floor_j-1];
		if (NextCube->type == DEAD)
			return true;

		if ( (Cube.y-Cube.size_y/2+0.1) <= (NextCube->y+NextCube->size_y/2) &&
			 (Cube.y+Cube.size_y/2) >= (NextCube->y-NextCube->size_y/2) ) { // Check no y overlap
			if ( (Cube.z+Cube.size_z/2+0.01) < (NextCube->z-NextCube->size_z/2) )
				return true;
		}
		else
			return true;
	}
	return false;
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
		case GLFW_KEY_SPACE:
			if (Cube.tilt[0])
				break;
			Wall* point = getFloorPointer(Cube.x, Cube.z);
			if (Cube.state == NORMAL) {
				if (point && point->type == STICKY)
					Cube.vel += glm::vec3 (0, 0.03, 0);
				else
					Cube.vel += glm::vec3 (0, 0.05, 0);
				Cube.state = FALLING;
			}
			break;
		}
	}
	else if (action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_UP:
			Eye.elev += 10;
			break;
		case GLFW_KEY_DOWN:
			Eye.elev -= 10;
			break;
		case GLFW_KEY_LEFT:
			Eye.angle -= 10;
			break;
		case GLFW_KEY_RIGHT:
			Eye.angle += 10;
			break;
		}
	}
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	float movement, x_move, z_move;
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
	case 'f':
		Cube.speed_mod = Cube.speed_mod < 1.5 ? Cube.speed_mod+0.1:Cube.speed_mod;
		Cube.speed_mod += 0.1;
		break;
	case 'g':
		Cube.speed_mod = Cube.speed_mod > 0.5 ? Cube.speed_mod-0.1:Cube.speed_mod;
		break;
	case 's':
		if (Cube.tilt[0])
			break;
		movement = -0.1 * Cube.speed_mod;
		x_move = -1 * movement * sin(Cube.angle*M_PI/180.0f);
		z_move = -1 * movement * cos(Cube.angle*M_PI/180.0f);
		if (moveCube(x_move, 0))
			Cube.x += x_move;
		if (moveCube(0, z_move))
			Cube.z += z_move;
		break;
	case 'w':
		if (Cube.tilt[0])
			break;
		movement = 0.1 * Cube.speed_mod;
		x_move = -1 * movement * sin(Cube.angle*M_PI/180.0f);
		z_move = -1 * movement * cos(Cube.angle*M_PI/180.0f);
		if (moveCube(x_move, 0))
			Cube.x += x_move;
		if (moveCube(0, z_move))
			Cube.z += z_move;
		break;
	case 'a':
		Cube.angle += 5;
		break;
	case 'd':
		Cube.angle -= 5;
		break;
	case 'r':
		resetGame();
		break;

	case '1':
		Eye.camera_type = TOWER;
		Eye.zoom = 4;
		break;
	case '2':
		Eye.camera_type = TOP;
		Eye.zoom = 4;
		break;
	case '3':
		Eye.camera_type = ADVENTURE;
		Eye.zoom = 0.7;
		break;
	case '4':
		Eye.camera_type = FOLLOW;
		Eye.zoom = 2;
		break;
	case '5':
		Eye.camera_type = HELI;
		Eye.zoom = 4;
		Eye.LookAt_x = Cube.x;
		Eye.LookAt_y = Cube.y;
		Eye.LookAt_z = Cube.z;
		break;
	}
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			Mouse.MOUSE_HELD = true;
			Mouse.held_x = Mouse.x;
			Mouse.held_y = Mouse.y;
			Mouse.angle = Eye.angle;
		}
		if (action == GLFW_RELEASE) {
			Mouse.MOUSE_HELD = false;
		}
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

	// Ortho projection for 2D views
	Matrices.projection = glm::ortho(Eye.zoom*-1.0f - Eye.pan, Eye.zoom + Eye.pan, Eye.zoom*-1.0f, Eye.zoom, 0.1f, 500.0f);
	// Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}

void applyFlEff ()
{
	for(int i=0; i<FLOOR_LENGTH; i++)
		for(int j=0; j<FLOOR_LENGTH; j++) {
			if((i==0 && j==0) || (i==FLOOR_LENGTH-1&&j==FLOOR_LENGTH-1))
				continue;
			if (rand()%GAME_DIFFICULTY &&rand()%GAME_DIFFICULTY&&rand()%GAME_DIFFICULTY)
				continue;

			// Dead blocks
			if(rand()%3) {
				Floor[i][j].type = DEAD;
				continue;
			}

			// Sticky floors
			if (rand()%2) {
				if (rand()%2)
				{
					Floor[i][j].type = STICKY;
					for(int k=0; k<36*3; k++)
						if((k-1)%3)
							Floor[i][j].g_color_buffer_data[k] /= 1.7;
					continue;
				}
				else {
					// Hovering floors
					Floor[i][j].type = HOVERING;
					Floor[i][j].y = (rand()%3)*2-6;
					Floor[i][j].direction = -1;
					for(int k=0; k<36*3; k++) {
						if(k%3)
							Floor[i][j].g_color_buffer_data[k] = 0;
					}
					continue;
				}
			}

			// Crumbling floors
			if(rand()%3) {
				Floor[i][j].type = CRUMBLING;
				for(int k=0; k<36*3; k++)
					Floor[i][j].g_color_buffer_data[k] /= 3.7;
				continue;
			}

		}
}

void createFloor ()
{
	for(int i=0; i<FLOOR_LENGTH; i++)
		for(int j=0; j<FLOOR_LENGTH; j++) {
			Floor[i][j].size_x = 2;
			Floor[i][j].size_y = 2;
			Floor[i][j].size_z = 2;
			Floor[i][j].x = 2*i;
			Floor[i][j].z = -2*j;
			Floor[i][j].type = STABLE;
			Floor[i][j].y = -3;
		}

	applyFlEff();

	for(int i=0; i<FLOOR_LENGTH; i++)
		for(int j=0; j<FLOOR_LENGTH; j++)
		  Floor[i][j].sprite = create3DObject(GL_TRIANGLES, 36, Floor[i][j].g_vertex_buffer_data, Floor[i][j].g_color_buffer_data, GL_FILL, g_normal_buffer_data);

}

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
	// glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	// Target - Where is the camera looking at.  Don't change unless you are sure!!
	// glm::vec3 target (0, 0, 0);
	// Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
	// glm::vec3 up (0, 1, 0);

	// Compute Eye matrix (view)
	// Matrices.view = glm::lookAt( eye, target, up ); // Rotating Eye for 3D
	//  Don't change unless you are sure!!
	Matrices.view = glm::lookAt(glm::vec3(Eye.x, Eye.y, Eye.z), glm::vec3(Eye.LookAt_x, Eye.LookAt_y, Eye.LookAt_z), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

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

	//####################################################################################################

	for(int i=0; i<FLOOR_LENGTH; i++)
		for(int j=0; j<FLOOR_LENGTH; j++) {
			MVP = VP * glm::translate (glm::vec3(Floor[i][j].x, Floor[i][j].y, Floor[i][j].z));
			glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
			if (Floor[i][j].type != DEAD)
				draw3DObject(Floor[i][j].sprite);
}

	if (moveShadow()) {
		MVP = VP * glm::translate (glm::vec3(Shadow.x, Shadow.y, Shadow.z)) * glm::scale(glm::vec3(Shadow.scale, 1.0f, Shadow.scale));
		// if (Eye.camera_type != ADVENTURE)
		MVP = MVP * glm::rotate((float)(Cube.angle*M_PI/180.0f), glm::vec3(0,1,0));
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(Shadow.sprite);
	}

	if (Eye.camera_type != ADVENTURE) {
	    MVP = VP * glm::translate (glm::vec3(Cube.x, Cube.y, Cube.z)) * glm::rotate((float)(Cube.angle*M_PI/180.0f), glm::vec3(0,1,0));
	    MVP *= glm::rotate((float)(Cube.tilt[0]*M_PI/180.0f), glm::vec3(1,0,0)) * glm::rotate((float)(Cube.tilt[2]*M_PI/180.0f), glm::vec3(0,0,1));
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(Cube.sprite);
	}

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
		Eye.zoom -= 0.25;
		Eye.zoom = max(0.5f, Eye.zoom);
	}
	else {
		Eye.zoom += 0.25;
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

	window = glfwCreateWindow(width, height, "CUBE", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval( 1 );
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

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
	createChar();

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Mouse.x = xpos;
	Mouse.y = ypos;
}

// x=1,y=2,z=-1;
// LookAt_x=10, LookAt_y=1, LookAt_z=-10;

void set_cam()
{
	switch(Eye.camera_type) {
	case ADVENTURE:
		Eye.zoom = 0.7;
		Eye.pan = Eye.zoom + 0.25;
		Eye.x = Cube.x;
		Eye.y = Cube.y;
		Eye.z = Cube.z;
		Eye.LookAt_x = Cube.x-sin(Cube.angle*M_PI/180.0f);
		Eye.LookAt_y = Cube.y-0.1;
		Eye.LookAt_z = Cube.z-cos(Cube.angle*M_PI/180.0f);
		break;
	case TOWER:
		Eye.x = Cube.x;
		Eye.y = Cube.y+3;
		Eye.pan = Eye.zoom;
		Eye.z = Cube.z+3;
		Eye.LookAt_x = Cube.x;
		Eye.LookAt_y = Cube.y;
		Eye.LookAt_z = Cube.z;
		break;
	case TOP:
		Eye.x = Cube.x;
		Eye.y = Cube.y+3;
		Eye.pan = Eye.zoom;
		Eye.z = Cube.z+0.0001;
		Eye.LookAt_x = Cube.x;
		Eye.LookAt_y = Cube.y;
		Eye.LookAt_z = Cube.z;
		break;
	case FOLLOW:
		Eye.x = Cube.x+3*sin(Cube.angle*M_PI/180.0f);
		Eye.y = Cube.y+3;
		Eye.z = Cube.z+3*cos(Cube.angle*M_PI/180.0f);
		Eye.pan = Eye.zoom;
		Eye.LookAt_x = Cube.x;
		Eye.LookAt_y = Cube.y;
		Eye.LookAt_z = Cube.z;
		break;
	case HELI:
		Eye.x = Cube.x + 5*cos(Eye.angle*M_PI/180.0f);
		Eye.z = Cube.z + 5*sin(Eye.angle*M_PI/180.0f);
		Eye.pan = Eye.zoom;
		Eye.elev = Eye.elev>80? 80:Eye.elev;
		Eye.elev = Eye.elev<-80? -80:Eye.elev;
		Eye.y = Cube.y + 5*sin(Eye.elev*M_PI/180.0f);
		Eye.LookAt_x = Cube.x;
		Eye.LookAt_y = Cube.y;
		Eye.LookAt_z = Cube.z;
		break;
	}
}

void mouse_drag()
{
	int x_dist = Mouse.x - Mouse.held_x;
	int y_dist = Mouse.y - Mouse.held_y;
	Eye.angle = Mouse.angle + x_dist/2;
	Eye.elev = y_dist/10;
	Eye.x = Cube.x + 5*cos(Eye.angle*M_PI/180.0f);
	Eye.z = Cube.z + 5*sin(Eye.angle*M_PI/180.0f);
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

		// Swap Frame Buffer in double buffering
		glfwSwapBuffers(window);

		// Poll for Keyboard and mouse events
		glfwPollEvents();

		glfwSetCursorPosCallback(window, cursor_position_callback);

		gravity();

		if (Mouse.MOUSE_HELD)
			mouse_drag();

		set_cam();

		// OpenGL Draw commands
		draw();

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

void createChar ()
{
	GLfloat g_vertex_buffer_data[] = {
		// TREADS
		-0.5f,-0.5f,-0.5f,
		-0.3f,-0.5f,-0.5f,
		-0.5f, 0.0f, 0.0f,
		-0.3f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		-0.3f,-0.5f,-0.5f,
		-0.5f,-0.5f, 0.5f,
		-0.3f,-0.5f, 0.5f,
		-0.5f, 0.0f, 0.0f,
		-0.3f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		-0.3f,-0.5f, 0.5f,
		-0.5f,-0.45f,-0.5f,
		-0.5f,-0.45f, 0.5f,
		-0.3f,-0.45f,-0.5f,
		-0.3f,-0.45f,-0.5f,
		-0.5f,-0.45f, 0.5f,
		-0.3f,-0.45f, 0.5f,
		 0.5f,-0.5f,-0.5f,
		 0.3f,-0.5f,-0.5f,
		 0.5f, 0.0f, 0.0f,
		 0.3f, 0.0f, 0.0f,
		 0.5f, 0.0f, 0.0f,
		 0.3f,-0.5f,-0.5f,
		 0.5f,-0.5f, 0.5f,
		 0.3f,-0.5f, 0.5f,
		 0.5f, 0.0f, 0.0f,
		 0.3f, 0.0f, 0.0f,
		 0.5f, 0.0f, 0.0f,
		 0.3f,-0.5f, 0.5f,
		 0.5f,-0.45f,-0.5f,
		 0.5f,-0.45f, 0.5f,
		 0.3f,-0.45f,-0.5f,
		 0.3f,-0.45f,-0.5f,
		 0.5f,-0.45f, 0.5f,
		 0.3f,-0.45f, 0.5f,

		// Body
		-0.3f,-0.3f,-0.3f,
		-0.3f,-0.3f, 0.3f,
		-0.3f, 0.3f, 0.3f,
		-0.3f,-0.3f,-0.3f,
		-0.3f, 0.3f, 0.3f,
		-0.3f, 0.3f,-0.3f,
		0.3f, 0.3f,-0.3f,
		-0.3f,-0.3f,-0.3f,
		-0.3f, 0.3f,-0.3f,
		0.3f,-0.3f, 0.3f,
		-0.3f,-0.3f,-0.3f,
		0.3f,-0.3f,-0.3f,
		0.3f, 0.3f,-0.3f,
		0.3f,-0.3f,-0.3f,
		-0.3f,-0.3f,-0.3f,
		0.3f,-0.3f, 0.3f,
		-0.3f,-0.3f, 0.3f,
		-0.3f,-0.3f,-0.3f,
		-0.3f, 0.3f, 0.3f,
		-0.3f,-0.3f, 0.3f,
		0.3f,-0.3f, 0.3f,
		0.3f, 0.3f, 0.3f,
		0.3f,-0.3f,-0.3f,
		0.3f, 0.3f,-0.3f,
		0.3f,-0.3f,-0.3f,
		0.3f, 0.3f, 0.3f,
		0.3f,-0.3f, 0.3f,
		0.3f, 0.3f, 0.3f,
		0.3f, 0.3f,-0.3f,
		-0.3f, 0.3f,-0.3f,
		0.3f, 0.3f, 0.3f,
		-0.3f, 0.3f,-0.3f,
		-0.3f, 0.3f, 0.3f,
		0.3f, 0.3f, 0.3f,
		-0.3f, 0.3f, 0.3f,
		0.3f,-0.3f, 0.3f,

		// Head
		-0.15f, 0.35f,-0.15f,
		-0.15f, 0.35f, 0.15f,
		-0.15f, 0.65f, 0.15f,
		-0.15f, 0.35f,-0.15f,
		-0.15f, 0.65f, 0.15f,
		-0.15f, 0.65f,-0.15f,
		 0.15f, 0.65f,-0.15f,
		-0.15f, 0.35f,-0.15f,
		-0.15f, 0.65f,-0.15f,
		 0.15f, 0.35f, 0.15f,
		-0.15f, 0.35f,-0.15f,
		 0.15f, 0.35f,-0.15f,
		 0.15f, 0.65f,-0.15f,
		 0.15f, 0.35f,-0.15f,
		-0.15f, 0.35f,-0.15f,
		 0.15f, 0.35f, 0.15f,
		-0.15f, 0.65f, 0.15f,
		-0.15f, 0.35f,-0.15f,
		-0.15f, 0.65f, 0.15f,
		-0.15f, 0.35f, 0.15f,
		 0.15f, 0.35f, 0.15f,
		 0.15f, 0.65f, 0.15f,
		 0.15f, 0.35f,-0.15f,
		 0.15f, 0.65f,-0.15f,
		 0.15f, 0.35f,-0.15f,
		 0.15f, 0.65f, 0.15f,
		 0.15f, 0.35f, 0.15f,
		 0.15f, 0.65f, 0.15f,
		 0.15f, 0.65f,-0.15f,
		-0.15f, 0.65f,-0.15f,
		 0.15f, 0.65f, 0.15f,
		-0.15f, 0.65f,-0.15f,
		-0.15f, 0.65f, 0.15f,
		 0.15f, 0.65f, 0.15f,
		-0.15f, 0.65f, 0.15f,
		 0.15f, 0.35f, 0.15f,
	};

	GLfloat g_color_buffer_data[] = {
		0.2f, 0.2f, 0.2f, // triangle 1 : begin
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f, // triangle 1 : end
		0.2f, 0.2f, 0.2f, // triangle 2 : begin
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f, // triangle 2 : end
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,

		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
};

	GLfloat g_normal_buffer_data[] = {
		0.2f, 0.2f, 0.2f, // triangle 1 : begin
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f, // triangle 1 : end
		0.2f, 0.2f, 0.2f, // triangle 2 : begin
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f, // triangle 2 : end
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		0.2f, 0.2f, 0.2f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,

		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,
};

	Cube.vel = glm::vec3(0, 0, 0);
	Cube.size_x = 1;
	Cube.size_y = 1;
	Cube.size_z = 1;
	Cube.scale = 1.0f;
	Cube.x = 0;
	Cube.y = -1;
	Cube.z = 0;
	Cube.speed_mod = 1.0;
	Cube.state = NORMAL;
	Cube.tilt = glm::vec3(0, 0, 0);
	Cube.sprite = create3DObject(GL_TRIANGLES, 108, g_vertex_buffer_data, g_color_buffer_data, GL_FILL, g_normal_buffer_data);

	GLfloat shadow_vertex_buffer_data[] = {
		0.5f, 0.0f,-0.5f,
	   -0.5f, 0.0f,-0.5f,
		0.5f, 0.0f, 0.5f,
		0.5f, 0.0f, 0.5f,
	   -0.5f, 0.0f, 0.5f,
	   -0.5f, 0.0f,-0.5f,
	};
	GLfloat shadow_color_buffer_data[] = {
		0.25f, 0.25f, 0.25f,
		0.25f, 0.25f, 0.25f,
		0.25f, 0.25f, 0.25f,
		0.25f, 0.25f, 0.25f,
		0.25f, 0.25f, 0.25f,
		0.25f, 0.25f, 0.25f,
	};
	Shadow.size_x = 0.25;
	Shadow.size_y = 0.25;
	Shadow.size_z = 0.1;
	Shadow.angle = 0;
	Shadow.scale = 1.0f;
	Shadow.x = Cube.x;
	Shadow.y = -2;
	Shadow.z = Cube.y;
	Shadow.sprite = create3DObject(GL_TRIANGLES, 6, shadow_vertex_buffer_data, shadow_color_buffer_data, GL_FILL, g_normal_buffer_data);
}
