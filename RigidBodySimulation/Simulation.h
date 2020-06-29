#pragma once
#include <GL/glew.h>

#include <memory>

#include <SDL.h>
#include <SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <CL/cl.hpp>

#include "ObjParser_OGL3.h"
#include "ProgramObject.h"
#include "BufferObject.h"
#include "VertexArrayObject.h"
#include "TextureObject.h"
#include "Mesh_OGL3.h"
#include "Camera.h"

class Simulation {
public:
	Simulation(void);
	~Simulation(void);

	bool Init();
	bool InitCL();
	void Clean();
	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);

	float fps[100] = { 0.0f };

private:
	//Private helper methods
	float random(float lower, float upper);
	void ballInit();
	void wallBuilder();
	void wallCollision(size_t i);
	void ballCollision(size_t i);
	void Collision_CPU(size_t i);
	void Collision_GPU();
	void UpdateCL();
	bool CLisActive = true;

	//Simulation variables
	int numberOfBalls = 40;
	float boxSize;
	glm::vec3 gravity;
	float resistance;
	float ballInitSpeed;
	bool run = false;
	bool ballCollisionRun = false;
	bool randomXZ = false;
	bool randomY = false;
	static const int numberOfBallsArray = 1500;
	glm::vec3 positions[numberOfBallsArray];
	glm::vec3 velocities[numberOfBallsArray];
	glm::vec4 colors[numberOfBallsArray];
	bool collisionCheck[numberOfBallsArray];

	//Shader Variables
	ProgramObject			shader;
	Texture2D				texture;
	VertexArrayObject		wall;
	ArrayBuffer				wallCoordinates;
	IndexBuffer				wallIndices;
	ArrayBuffer				wallNormals;
	ArrayBuffer				wallTextures;
	std::unique_ptr<Mesh>	ball;
	Camera					camera;
	glm::vec4				wallColor = glm::vec4(1, 1, 1, 0.1);

	//OpenCL variables
	cl::Context context;
	cl::CommandQueue commandQueue;
	cl::Program program;
	cl::Kernel kernel;
	cl::Buffer CLvelocities, CLpositions, CLcollisionCheck;
};

