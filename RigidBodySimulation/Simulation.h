/*
	GPU accelerated rigid body simulation with OpenGL and OpenCL.
	Created by: Sándor Balázs - it.sandor.balazs@gmail.com
	AX400
	---
	The main simulation object. Simulation initialisation and Input handling.
*/

#pragma once
#include <ctime>
#include <GL/glew.h>

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
	//Debug mode (Be carefull! The log files grose fast!)
	bool debug = false;

	//Private helper methods
	float random(float lower, float upper);
	void ballInit();
	void wallBuilder();
	void barrierBuilder();
	void wallCollision(size_t i);
	void barrierCollision(size_t i);
	void ballCollision(size_t i);
	void Collision_CPU(size_t i);
	void Collision_GPU();
	void Update_GPU(bool updateAll = true);
	void UpdateVelocitiesFrom_GPU();
	bool GPU_isActive = true;

	//Simulation variables
	int numberOfBalls = 25;
	float boxSize;
	glm::vec3 gravity;
	float resistance;
	float ballInitSpeed;
	bool run = false;
	bool ballCollisionRun = true;
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
	std::unique_ptr<Mesh>	ball;
	Camera					camera;

	//wall
	VertexArrayObject		wall;
	ArrayBuffer				wallCoordinates;
	IndexBuffer				wallIndices;
	ArrayBuffer				wallNormals;
	ArrayBuffer				wallTextures;
	glm::vec4				wallColor = glm::vec4(1, 1, 1, 0.1);

	//barrier
	VertexArrayObject		barrier;
	ArrayBuffer				barrierCoordinates;
	IndexBuffer				barrierIndices;
	ArrayBuffer				barrierNormals;
	ArrayBuffer				barrierTextures;
	glm::vec4				barrierColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.8f);
	glm::vec3				barrierShift = glm::vec3(0, 0, 0);
	float					barrierSize = 0;
	bool					barrierIsOn = true;

	//OpenCL variables
	cl::Context context;
	cl::CommandQueue commandQueue;
	cl::Program program;
	cl::Kernel kernel;
	cl::Buffer CLvelocities, CLpositions, CLcollisionCheck;
};

std::string currentDateTime();