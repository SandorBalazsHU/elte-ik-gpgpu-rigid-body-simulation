#pragma once

// C++ includes
#include <memory>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "ProgramObject.h"
#include "BufferObject.h"
#include "VertexArrayObject.h"
#include "TextureObject.h"

#include "Mesh_OGL3.h"
#include "gCamera.h"

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
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
protected:
	static const int numberOfBalls = 2;
	static const int boxSize = 5;
	float random(float lower, float upper);
	void ballInit();
	void wallCollision(glm::vec3& position, glm::vec3& velocity);
	void ballCollision();
	glm::vec3 positions[numberOfBalls];
	glm::vec3 velocities[numberOfBalls];
	float veight[numberOfBalls];
	glm::vec3 gravity;
	float resistance;
	float ballInitSpeed;
	bool run = false;
	bool ballCollisionRun = false;
	
	// shaderekhez szükséges változók
	ProgramObject		m_program;		// shaderek programja

	Texture2D			m_textureMetal;

	VertexArrayObject	m_vao;			// VAO objektum
	IndexBuffer			m_gpuBufferIndices;		// indexek
	ArrayBuffer			m_gpuBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_gpuBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_gpuBufferTex;	// textúrakoordináták tömbje

	std::unique_ptr<Mesh>	m_mesh;

	gCamera				m_camera;

	glm::vec4			m_wallColor = glm::vec4(1,1,1,0.2);
};

