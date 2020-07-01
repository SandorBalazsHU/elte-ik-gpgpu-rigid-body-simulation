/*
	GPU accelerated rigid body simulation with OpenGL and OpenCL.
	Created by: Sándor Balázs - it.sandor.balazs@gmail.com
	AX400
	---
	Own camera implementation with sphere coordniates.
*/

#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

class Camera {
public:
	Camera(void);
	Camera(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up);
	~Camera(void);
	void Update();
	void SetView(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up);
	void SetProj(float _angle, float _aspect, float _zn, float _zf); 
	void LookAt(glm::vec3 _at);

	glm::vec3 GetEye() {
		return m_eye;
	}

	glm::vec3 GetAt() {
		return m_at;
	}

	glm::vec3 GetUp() {
		return m_up;
	}

	glm::mat4 GetProj() {
		return m_matProj;
	}

	glm::mat4 GetViewProj() {
		return m_matViewProj;
	}

	void Resize(int _w, int _h);
	void KeyboardDown(SDL_KeyboardEvent& key);
	void KeyboardUp(SDL_KeyboardEvent& key);
	void MouseMove(SDL_MouseMotionEvent& mouse);
	void MouseWheel(SDL_MouseWheelEvent& wheel);
	glm::mat4 GetViewMatrix();

private:

	void SphereUpdate(float r, float a, float b);

	//The view matrix of the camera
	glm::mat4	m_viewMatrix;

	glm::mat4	m_matViewProj;

	//The camera position.
	glm::vec3	m_eye;

	//The camera position in the sphere.
	glm::vec3	m_eye_sphare;


	//The vector pointing upwards
	glm::vec3	m_up;


	//The camera look at point.
	glm::vec3	m_at;

	//The u spherical coordinate of the spherical coordinate pair (u,v) denoting the
	//current viewing direction from the view position m_eye. 
	float	m_u;


	//The v spherical coordinate of the spherical coordinate pair (u,v) denoting the
	//current viewing direction from the view position m_eye. 
	float	m_v;


	//The distance of the look at point from the camera. 
	float	m_dist;

	//The unit vector pointing towards the viewing direction.
	glm::vec3	m_fw;

	//The unit vector pointing to the 'right'
	glm::vec3	m_st;

	glm::mat4	m_matProj;

	float	m_goFw;
	float	m_goRight;
};

