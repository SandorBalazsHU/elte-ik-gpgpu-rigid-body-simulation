/*
	GPU accelerated rigid body simulation with OpenGL and OpenCL.
	Created by: Sándor Balázs - it.sandor.balazs@gmail.com
	AX400
	---
	The main simulation object. Simulation initialisation and Input handling.
*/

#include <ctime>
#include "Simulation.h"

//Constructor
Simulation::Simulation(void) {
	//Random generator initialisation
	srand((unsigned int)time(NULL));

	//Simulation values initialisation
	gravity = glm::vec3(0.0f, 0.006f, 0.0f);
	resistance = 0.996f;
	ballInitSpeed = 0.2f;
	boxSize = 15.0f;
	barrierShift = glm::vec3(0, -boxSize + ((boxSize) / 6) + 0.01f, 0);
	float barrierSize = boxSize / 6;
}

//Destructor
Simulation::~Simulation(void){}

//Simulation rendering intitialisation
bool Simulation::Init() {
	glClearColor(0.0f, 0.47f, 0.87f, 1.0f);	//BG Color
	glEnable(GL_CULL_FACE);					//Face test
	glEnable(GL_DEPTH_TEST);				//Depth buffer
	glEnable(GL_BLEND);						//Alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Shader loading
	shader.Init({
		{ GL_VERTEX_SHADER, "vertexShader.vert" },
		{ GL_FRAGMENT_SHADER, "fragmentShader.frag" }
		},
	{
		{ 0, "vs_in_pos" },		// VAO 0	 --> vs_in_pos
		{ 1, "vs_in_normal" },	// VAO 1 chanel --> vs_in_normal
		{ 2, "vs_out_tex0" },	// VAO 2 chanel --> vs_in_tex0
	});

	//Building the wall modell
	wallBuilder();

	//Building the barrier modell
	barrierBuilder();

	//Load Texture
	texture.FromFile("texture.png");

	// mesh betöltése
	ball = ObjParser::parse("sphere.obj");

	//Set Camera
	//camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	//Balls start initialisation
	ballInit();

	return true;
}

//Balls start initialisation
void Simulation::ballInit() {
	float r = boxSize - 2.0f;
	float x = 0;
	float y = boxSize - (boxSize / 4);
	float z = 0;
	for (size_t i = 0; i < numberOfBallsArray; i++) {
		if (randomXZ) {
			x = random(-r, r);
			z = random(-r, r);
		}
		if (randomY)    y = random(-r, r);
		positions[i] = glm::vec3(x, y, z);
		velocities[i] = glm::vec3(random(-ballInitSpeed, ballInitSpeed), random(-ballInitSpeed, ballInitSpeed), random(-ballInitSpeed, ballInitSpeed));
		colors[i] = glm::vec4(random(0.0f, 1.0f), random(0.0f, 1.0f), random(0.0f, 1.0f), 1);
		collisionCheck[i] = true;
	}
	//Copy datas to GPU
	if (GPU_isActive) Update_GPU();
}

void Simulation::Clean() {
	shader.Clean();
}

//Update the Camera
void Simulation::Update() {
	//camera.update();
}

void Simulation::KeyboardDown(SDL_KeyboardEvent& key) {
	camera.keyboardDown(key);
}

void Simulation::KeyboardUp(SDL_KeyboardEvent& key) {
	camera.keyboardUp(key);
}

void Simulation::MouseMove(SDL_MouseMotionEvent& mouse) {
	camera.mouseMove(mouse);
	if (barrierIsOn) {
		if (mouse.state & SDL_BUTTON_RMASK) {
			glm::vec3 shifted = barrierShift;
			glm::vec3 shift = glm::vec3(mouse.xrel / 4.0f, 0, mouse.yrel / 4.0f);
			shifted = shifted + shift;
			if (shifted.x >= -boxSize + barrierSize && shifted.x <= boxSize - barrierSize) barrierShift.x = barrierShift.x + shift.x;
			if (shifted.z >= -boxSize + barrierSize && shifted.z <= boxSize - barrierSize) barrierShift.z = barrierShift.z + shift.z;
			Update_GPU(false);
		}
	}
}

void Simulation::MouseDown(SDL_MouseButtonEvent& mouse) {}
void Simulation::MouseUp(SDL_MouseButtonEvent& mouse) {}

void Simulation::MouseWheel(SDL_MouseWheelEvent& wheel) {
	camera.mouseWheel(wheel);
}

void Simulation::Resize(int _w, int _h) {
	glViewport(0, 0, _w, _h);
	camera.resize(_w, _h);
}

float Simulation::random(float lower, float upper) {
	float random = lower + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upper - lower)));
	return random;
}

std::string currentDateTime() {
	time_t now = time(0);
	std::string ret = ctime(&now);
	ret[ret.length() - 1] = ' ';
	return ret;
}